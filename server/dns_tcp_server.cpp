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
#include <vector>
#include <memory>
#include <thread>

#include "types.hpp"
#include "dns_tcp_server.hpp"
#include "config.hpp"
#include "run_state.hpp"
#include "dns_zone.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

dns_tcp_server::dns_tcp_server(boost::asio::io_service &io_service, const server_config &config) :
    server(io_service),
    m_config(config),
    m_auth_resolver(dns_auth_resolver::get_instance(config)),
    m_recursive_resolver(new dns_recursive_resolver(config, true)),
    m_forwarding_resolver(new dns_forwarding_resolver(config))
{
    for (uint i = 0; i < m_config.dns.num_tcp_threads; i++)
    {
        m_server_threads.add(new dns_tcp_connection(m_config, m_auth_resolver, m_recursive_resolver, m_forwarding_resolver, m_server_threads));
    }
    dns_zone::init();
}

void dns_tcp_server::run()
{
    m_receive_thread = make_shared<thread>(&dns_tcp_server::accept_inbound, this);
}

void dns_tcp_server::join()
{
    m_receive_thread->join();
    m_server_threads.join();
    m_recursive_resolver->join();
    m_forwarding_resolver->join();
    dns_recursive_cache::get_instance()->join();
}

dns_tcp_server::~dns_tcp_server()
{
}

void dns_tcp_server::connect()
{
    for (auto sa : m_config.socket_addresses)
    {
        socket_address s_addr(ip_address(sa.ip_address), sa.port);

        auto s = make_shared<tcp_server_socket>(s_addr);
        s->listen(m_config.dns.allowed_connection_backlog);
        m_socket_set.add(s);
    }
}

void dns_tcp_server::accept_inbound()
{
    try
    {
        connect();

        while (run_state::o_state != run_state::shutdown_e)
        {
            try
            {
                if (run_state::o_state == run_state::pause_e)
                {
                    sleep(1);
                }
                else
                {
                    auto s = get<0>(m_socket_set.wait_one());

                    for (auto cs : dynamic_cast<tcp_server_socket *>(s.get())->accept_many())
                    {
                        m_server_threads.get()->serve(cs);
                    }
                }
            }
            catch (tcp_socket_timeout_exception &e)
            {
            }
            catch (socket_set_timeout_exception &e)
            {
            }
            catch (thread_pool_timeout_exception &e)
            {
            }
        }
    }
    catch (adns::exception &e)
    {
        e.log(fatal, "shutting down");
        run_state::set(run_state::shutdown_e);
    }
}

string dns_tcp_server::name() const
{
    return "DNS TCP server";
}
