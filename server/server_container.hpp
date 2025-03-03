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
#pragma once

#include <list>
#include <string>

#include <boost/asio.hpp>

#include "server.hpp"
#include "types.hpp"

namespace adns
{
    /**
     * Container for a collection of servers. Handles starting up, catching signals,
     * restarting and exiting.
     */
    class server_container final
    {
    public:

        /**
         * New container. Needs the bootstrap config so that it can re-read the real config after
         * a restart (via SIGHUP).
         */
        server_container(boost::asio::io_service &io_service);

        /**
         * Destructor
         */
        virtual ~server_container();

        /**
         * Add a server, which must be ready to run
         */
        void add(server *s);

        /**
         * Start running all the servers that have been added
         */
        void run();

        /**
         * Wait for all threads from all servers to join
         */
        void join();

    private:

        boost::asio::io_service &m_io_service;
        boost::asio::signal_set m_stop_signals;
        std::list<std::shared_ptr<server>> m_servers;

        /**
         * Handle one of the various stop signals (INT, TERM, QUIT)
         */
        void handle_stop(const boost::system::error_code &error, int signal_number);

        /**
         * Setup signal handlers
         */
        void set_signals();
    };
}
