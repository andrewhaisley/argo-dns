//
//  Copyright 2025 Andrew Haisley
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
//  associated documentation files (the “Software”), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial 
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
//  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
 
#include <arpa/inet.h>
#include <unistd.h>

#include "types.hpp"
#include "dns_tcp_connection.hpp"
#include "thread_pool.hpp"
#include "dns_auth_resolver.hpp"
#include "dns_forwarding_resolver.hpp"
#include "run_state.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

dns_tcp_connection::dns_tcp_connection(
                        server_config                       &config, 
                        shared_ptr<dns_auth_resolver>       p_auth_resolver,
                        shared_ptr<dns_recursive_resolver>  p_recursive_resolver,
                        shared_ptr<dns_forwarding_resolver> p_forwarding_resolver,
                        thread_pool<dns_tcp_connection>     &pool) :
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

    // generally these queues will only have 1 message in them, but allow for more to handle the case of
    // zone transfers (not yet implemented)
    m_handler_pool = new handler_pool<dns_message_envelope *, dns_handler, dns_handler::params_t>(
                                    "DNS TCP connection mini-handler pool", params, 100, 100, 1);

    m_receive_thread.reset(new thread(&dns_tcp_connection::run, this));
}

dns_tcp_connection::~dns_tcp_connection()
{
    delete m_handler_pool;
}

void dns_tcp_connection::done()
{
    lock_guard<mutex> guard(m_lock);
    m_socket = nullptr;
    m_connection_arrived = false;
    m_pool.put(this);
}

void dns_tcp_connection::wait_for_connection()
{
    unique_lock<mutex> guard(m_lock);

    while (!m_connection_arrived)
    {
        if (m_condition.wait_for(guard, chrono::seconds(2)) == cv_status::timeout)
        {
            THROW(dns_tcp_connection_timeout_exception, "timeout");
        }
    }
}

void dns_tcp_connection::run()
{
    while (true)
    {
        try
        {
            unsigned short len;

            while (true)
            {
                try
                {
                    wait_for_connection();
                    break;
                }
                catch (dns_tcp_connection_timeout_exception &e)
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
                    // read the message length - first 2 bytes of the request
                    buffer b = m_socket->read(2, m_config.dns.tcp_max_len_wait_ms);
                    memcpy(&len, b.get_data(), 2);
                    len = ntohs(len);

                    // if the message isn't long enough to parse, close the connection
                    if (len < MIN_POSSIBLE_MESSAGE_SIZE)
                    {
                        done();
                        break;
                    }

                    // now read the actual request
                    buffer raw = m_socket->read(len, m_config.dns.tcp_max_body_wait_ms);

                    // pass it to the handler (via the multiplexer) for processing
                    dns_message_envelope *m = new dns_message_envelope(raw, m_socket->get_remote_address(), nullptr);
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
                        // write the response length to the socket
                        len = htons(m->get_raw().get_size());
                        buffer resp_len(2);
                        memcpy(resp_len.get_data(), &len, 2);
                        m_socket->write(resp_len, m_config.dns.tcp_max_len_wait_ms);

                        // write the response itself
                        m_socket->write(m->get_raw(), m_config.dns.tcp_max_body_wait_ms);

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

void dns_tcp_connection::serve(const shared_ptr<tcp_socket> &socket)
{
    lock_guard<mutex> guard(m_lock);
    m_connection_arrived = true;
    m_socket = socket;
    m_condition.notify_one();
}

void dns_tcp_connection::join()
{
    m_receive_thread->join();
    m_handler_pool->join();
}
