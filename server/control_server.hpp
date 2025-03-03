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

#include <memory>

#include <boost/asio.hpp>

#include "types.hpp"
#include "server.hpp"
#include "config.hpp"
#include "server_config.hpp"
#include "handler_pool.hpp"
#include "thread_pool.hpp"
#include "tcp_socket.hpp"
#include "socket_set.hpp"
#include "control_connection.hpp"

EXCEPTION_CLASS(control_server_exception, exception)

namespace adns
{
    /**
     * Serve control requests through a REST API
     */
    class control_server final : public server
    {
    public:

        /**
         * Construct a server ready to run...
         */
        control_server(boost::asio::io_service &io_service, const server_config &config);

        /**
         * destructor - destroy threads etc.
         */
        virtual ~control_server();

        /**
         * Start running and processing requests - returns immediately as the processing is done
         * in its own thread.
         */
        void run();

        /**
         * Wait for running threads to join. Called after run() has been called to create the 
         * receiving and processing threads.
         */
        void join();

        /**
         * tell all threads to exit
         */
        void stop();

        /**
         * Name
         */
        virtual std::string name() const;

    private:

        /**
         * config items for this server instance
         */
        server_config m_config;

        /**
         * thread to run the input side and trigger the reception of requests.
         */
        std::shared_ptr<std::thread> m_receive_thread;

        /** 
         * pool of threads to service connections
         */
        thread_pool<control_connection> m_server_threads;

        /**
         * sockets on which we accept connections
         */
        socket_set m_socket_set;

        /**
         * Wait for connections to be made and then pass them to threads from the pool to be serviced.
         */
        void accept_inbound();

        /**
         * connect the inbound socket
         */
        void connect();
    };
}
