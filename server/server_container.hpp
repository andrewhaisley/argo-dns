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
