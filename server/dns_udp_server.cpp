// 
// Copyright 2025 Andrew Haisley
// 
// This program is free software: you can redistribute it and/or modify it under the terms 
// of the GNU General Public License as published by the Free Software Foundation, either 
// version 3 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with this program. 
// If not, see https://www.gnu.org/licenses/.
// 
#include <ctype.h>
#include <unistd.h>
#include <vector>
#include <memory>

#include "types.hpp"
#include "dns_udp_server.hpp"
#include "config.hpp"
#include "run_state.hpp"
#include "dns_zone.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

dns_udp_server::dns_udp_server(
        boost::asio::io_service &io_service,
        const server_config     &server_config) :
                        server(io_service), 
                        m_config(server_config)
        
{
    dns_handler::params_t params;

    for (auto sa : m_config.socket_addresses)
    {
        socket_address s_addr(ip_address(sa.ip_address), sa.port);
        auto s = make_shared<udp_socket>(s_addr);
        m_socket_set.add(s);
    }

    params.config = m_config;
    m_auth_resolver = params.p_auth_resolver = dns_auth_resolver::get_instance(server_config);
    m_recursive_resolver = params.p_recursive_resolver = make_shared<dns_recursive_resolver>(server_config, false);
    m_forwarding_resolver = params.p_forwarding_resolver = make_shared<dns_forwarding_resolver>(m_config);
    params.check_message_length = true;
    params.is_tcp = false;

    m_handler_pool = new handler_pool<dns_message_envelope *, dns_handler, dns_handler::params_t>(
                                                                "DNS UDP server handler pool",
                                                                params,
                                                                m_config.dns.max_in_message_queue_length,
                                                                m_config.dns.max_out_message_queue_length,
                                                                m_config.dns.num_udp_threads);

    dns_zone::init();
}

void dns_udp_server::send_outbound()
{
    while (true)
    {
        try
        {
            auto m = m_handler_pool->dequeue();

            try
            {
                if (m->get_socket())
                {
                    m->get_socket()->send(m->get_raw(), m->get_remote_address());
                }
                delete m;
            }
            catch (...)
            {
                delete m;
                throw;
            }
        }
        catch (message_queue_timeout_exception &e)
        {
            if (run_state::o_state == run_state::shutdown_e)
            {
                return;
            }
            else if (run_state::o_state == run_state::pause_e)
            {
                sleep(1);
            }
        }
        catch (adns::exception &e)
        {
            e.log(error, "failed to send message, dropping");
        }
    }
}

void dns_udp_server::run()
{
    for (uint i = 0; i < m_config.dns.num_receive_threads; i++)
    {
        m_receive_threads.push_back(make_shared<thread>(&dns_udp_server::receive_inbound, this));
    }

    for (uint i = 0; i < m_config.dns.num_send_threads; i++)
    {
        m_send_threads.push_back(make_shared<thread>(&dns_udp_server::send_outbound, this));
    }
}

void dns_udp_server::join()
{
    m_handler_pool->join();
    while (!m_receive_threads.empty())
    {
        m_receive_threads.front()->join();
        m_receive_threads.pop_front();
    }
    while (!m_send_threads.empty())
    {
        m_send_threads.front()->join();
        m_send_threads.pop_front();
    }
    m_recursive_resolver->join();
    m_forwarding_resolver->join();
    dns_recursive_cache::get_instance()->join();
}

dns_udp_server::~dns_udp_server()
{
    delete m_handler_pool;
}

void dns_udp_server::receive_inbound()
{
    while (true)
    {
        try
        {
            if (run_state::o_state == run_state::pause_e)
            {
                sleep(1);
            }
            else if (run_state::o_state == run_state::shutdown_e)
            {
                return;
            }
            else
            {
                shared_ptr<socket> ready_socket = get<0>(m_socket_set.wait_one());

                udp_socket::message *messages[100];

                int n = dynamic_cast<udp_socket *>(ready_socket.get())->receive_many(100, messages);

                try
                {
                    for (int i = 0; i < n; i++)
                    {
                        // black hole anything too small to parse
                        if (messages[i]->get_message().get_size() >= MIN_POSSIBLE_MESSAGE_SIZE)
                        {
                            auto m = new dns_message_envelope(
                                        messages[i]->get_message(), 
                                        messages[i]->get_remote_address(), 
                                        dynamic_cast<udp_socket *>(ready_socket.get()));

                            if (!m_handler_pool->enqueue(m))
                            {
                                delete m;
                            }
                        }
                        else
                        {
                            LOG(warning) << "short packet received, ignoring";
                        }
                    }

                    for (int i = 0; i < n; i++)
                    {
                        delete messages[i];
                    }
                }
                catch (...)
                {
                    for (int i = 0; i < n; i++)
                    {
                        delete messages[i];
                    }
                    throw;
                }
            }
        }
        catch (socket_set_timeout_exception &e)
        {
            // epoll timeout
        }
        catch (udp_socket_timeout_exception &e)
        {
            // socket read timeout
        }
    }
}

string dns_udp_server::name() const
{
    return "DNS UDP server";
}
