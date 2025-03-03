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
