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
 
#include <boost/bind/bind.hpp>

#include "types.hpp"
#include "server_container.hpp"
#include "config.hpp"
#include "run_state.hpp"

using namespace std;
using namespace adns;

server_container::server_container(boost::asio::io_service &io_service) : m_io_service(io_service), m_stop_signals(m_io_service)
{
    set_signals();
}

void server_container::set_signals()
{
    // catch SIGPIPE so that writes on broken sockets fail rather than
    // cause us to terminate.
    m_stop_signals.add(SIGPIPE);
    // all the rest of these cause termination.
    m_stop_signals.add(SIGINT);
    m_stop_signals.add(SIGTERM);
    m_stop_signals.add(SIGQUIT);
    m_stop_signals.async_wait(
                        boost::bind(&server_container::handle_stop, this, 
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::signal_number));
}

server_container::~server_container()
{
}

void server_container::add(server *s)
{
    m_servers.push_back(shared_ptr<server>(s));
}

void server_container::run()
{
    for (auto iter : m_servers)
    {
        iter->run();
    }

    while (true)
    {
        if (run_state::o_state == run_state::shutdown_e)
        {
            return;
        }
        m_io_service.poll_one();

        // the only thing we use the io_service for is run control, so sleeping for a second here
        // doesn't have any impact on throughput
        sleep(1);
    }
}

void server_container::join()
{
    for (auto iter : m_servers)
    {
        LOG(info) << "joining " << iter->name();
        iter->join();
    }
}

void server_container::handle_stop(const boost::system::error_code &error, int signal_number)
{
    if (signal_number == SIGPIPE)
    {
        return;
    }
    else
    {
        LOG(info) << "error code " << error << " caught signal " << signal_number << " shutting down";
        run_state::set(run_state::shutdown_e);
        m_io_service.stop();
    }
}
