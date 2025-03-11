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
#include <arpa/inet.h>
#include <unistd.h>

#include <boost/lexical_cast.hpp>

#include "types.hpp"
#include "util.hpp"
#include "http.hpp"
#include "dns_doh_connection.hpp"
#include "thread_pool.hpp"
#include "dns_auth_resolver.hpp"
#include "dns_forwarding_resolver.hpp"
#include "run_state.hpp"
#include "http.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

dns_doh_connection::dns_doh_connection(
                        server_config                       &config, 
                        shared_ptr<dns_auth_resolver>       p_auth_resolver,
                        shared_ptr<dns_recursive_resolver>  p_recursive_resolver,
                        shared_ptr<dns_forwarding_resolver> p_forwarding_resolver,
                        thread_pool<dns_doh_connection>     &pool) :
                            m_config(config),
                            m_pool(pool)
{
    m_connection_arrived = false;

    m_config.dns.max_in_message_queue_length = 1;
    m_config.dns.max_out_message_queue_length = 1;
    m_config.dns.num_tcp_threads = 1;
    m_config.dns.num_udp_threads = 1;
    m_config.dns.num_forwarding_threads = 1;

    dns_handler::params_t params;

    params.config = config;
    params.is_tcp = true;
    params.p_auth_resolver = p_auth_resolver;
    params.p_recursive_resolver = p_recursive_resolver;
    params.p_forwarding_resolver = p_forwarding_resolver;
    params.check_message_length = false;

    m_handler_pool = new handler_pool<dns_message_envelope *, dns_handler, dns_handler::params_t>(
                                    "DNS DoH connection mini-handler pool", params, 100, 100, 1);

    m_receive_thread.reset(new thread(&dns_doh_connection::run, this));
}

dns_doh_connection::~dns_doh_connection()
{
    delete m_handler_pool;
}

void dns_doh_connection::done()
{
    lock_guard<mutex> guard(m_lock);
    m_socket = nullptr;
    m_connection_arrived = false;
    m_pool.put(this);
}

void dns_doh_connection::wait_for_connection()
{
    unique_lock<mutex> guard(m_lock);

    while (!m_connection_arrived)
    {
        if (m_condition.wait_for(guard, chrono::seconds(2)) == cv_status::timeout)
        {
            THROW(dns_doh_connection_timeout_exception, "timeout");
        }
    }
}

void dns_doh_connection::run()
{
    while (true)
    {
        try
        {
            while (true)
            {
                try
                {
                    wait_for_connection();
                    break;
                }
                catch (dns_doh_connection_timeout_exception &e)
                {
                    if (run_state::o_state == run_state::shutdown_e)
                    {
                        return;
                    }
                }
            }

            while (true)
            {
                try
                {
                    http h(http::doh_e, *m_socket, m_config.dns.doh_client_timeout_ms);

                    auto req = h.from_wire();

                    buffer b;
                    dns_message_envelope *m = nullptr;

                    if (req->get_method() == "GET")
                    {
                        extract_get_request(req, b);
                        m = new dns_message_envelope(b, m_socket->get_remote_address());
                    }
                    else if (req->get_method() == "POST")
                    {
                        extract_post_request(req, b);
                        m = new dns_message_envelope(b, m_socket->get_remote_address());
                    }
                    else
                    {
                        THROW(dns_doh_connection_exception, "invalid method", req->get_method());
                    }

                    // pass it to the handler (via the multiplexer) for processing
                    if (!m_handler_pool->enqueue(m))
                    {
                        delete m;
                        done();
                        break;
                    }

                    // wait for a response
                    while (true)
                    {
                        try
                        {
                            m = m_handler_pool->dequeue();
                            break;
                        }
                        catch (message_queue_timeout_exception &e)
                        {
                            if (run_state::o_state == run_state::shutdown_e)
                            {
                                done();
                                return;
                            }
                        }
                    }

                    try
                    {
                        auto res = make_shared<http_response>(req->get_stream_id(), http_response::ok_200, m->get_raw());
                        res->add_header("cache-control", "private, max-age=" + boost::lexical_cast<string>(m->get_response()->get_min_ttl()));
                        h.to_wire(*res);

                        // cleanup
                        delete m;
                    }
                    catch (...)
                    {
                        // cleanup
                        delete m;

                        throw;
                    }

                }
                catch (tcp_socket_timeout_exception &e)
                {
                    done();
                    break;
                }
                catch (tcp_socket_eof_exception &e)
                {
                    done();
                    break;
                }

                if (run_state::o_state == run_state::shutdown_e)
                {
                    return;
                }
            }
        }
        catch (adns::exception &e)
        {
            done();

            if (run_state::o_state == run_state::shutdown_e)
            {
                return;
            }
        }
    }
}

void dns_doh_connection::extract_get_request(shared_ptr<http_request> &req, buffer &b)
{
    b = util::frombase64(req->get_url().get_parameter("dns"));
}

void dns_doh_connection::extract_post_request(shared_ptr<http_request> &req, buffer &b)
{
    b = req->get_payload();
}

void dns_doh_connection::serve(const shared_ptr<tcp_socket> &socket)
{
    lock_guard<mutex> guard(m_lock);
    m_connection_arrived = true;
    m_socket = socket;
    m_condition.notify_one();
}

void dns_doh_connection::join()
{
    m_receive_thread->join();
    m_handler_pool->join();
}
