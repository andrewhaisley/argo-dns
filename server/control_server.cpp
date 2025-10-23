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
 
#include <ctype.h>
#include <vector>
#include <memory>
#include <thread>

#include "types.hpp"
#include "control_server.hpp"
#include "config.hpp"
#include "run_state.hpp"
#include "ssl_server_socket.hpp"


using namespace std;
using namespace adns;
using namespace boost::log::trivial;

control_server::control_server(boost::asio::io_service &io_service, const server_config &config) :
    server(io_service),
    m_config(config)
{
    for (uint i = 0; i < m_config.control.num_threads; i++)
    {
        m_server_threads.add(new control_connection(m_config, m_server_threads));
    }
}

void control_server::run()
{
    m_receive_thread.reset(new thread(&control_server::accept_inbound, this));
}

void control_server::join()
{
    m_receive_thread->join();
    m_server_threads.join();
}

control_server::~control_server()
{
}

void control_server::connect()
{
    for (auto sa : m_config.socket_addresses)
    {
        socket_address s_addr(ip_address(sa.ip_address), sa.port);

        shared_ptr<tcp_server_socket> s;

        if (m_config.control.use_ssl)
        {
            s.reset(new ssl_server_socket(s_addr));
        }
        else
        {
            s.reset(new tcp_server_socket(s_addr));
        }

        s->listen(m_config.dns.allowed_connection_backlog);
        m_socket_set.add(s);
    }
}

void control_server::accept_inbound()
{
    try
    {
        connect();

        while (run_state::o_state != run_state::shutdown_e)
        {
            try
            {
                auto s = get<0>(m_socket_set.wait_one());

                for (auto cs : dynamic_cast<tcp_server_socket *>(s.get())->accept_many())
                {
                    m_server_threads.get()->serve(cs);
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
            catch (ssl_accept_failed_exception &e)
            {
                e.log(warning, "SSL_accept failed");
            }
        }
    }
    catch (adns::exception &e)
    {
        e.log(fatal, "shutting down");
        run_state::set(run_state::shutdown_e);
    }
}

string control_server::name() const
{
    return "control server";
}
