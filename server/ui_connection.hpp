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

#include <thread>
#include <mutex>

#include "types.hpp"

#include "thread_pool.hpp"
#include "tcp_socket.hpp"
#include "server_config.hpp"

EXCEPTION_CLASS(ui_connection_timeout_exception, exception)
EXCEPTION_CLASS(ui_connection_json_exception, exception)

namespace adns
{
    /**
     * Handle a single tcp ui connection. Reads http requests messages off the 
     * wire, handles them, then sends back the results.
     */
    class ui_connection final
    {
    public:

        /** 
         * New connection ready to handle incoming requests. Automatically returns itself
         * to the thread pool when done and ready to service a new connection.
         */
        ui_connection(server_config &config, thread_pool<ui_connection> &pool);

        /**
         * kill threads etc.
         */
        virtual ~ui_connection();

        /**
         * wait for a connection to be available and then serve it
         */
        void run();

        /**
         * signal a connection arrival
         */
        void serve(const std::shared_ptr<tcp_socket> &socket);

        /**
         * wait for the service thread to complete
         */
        void join();

    private:

        /**
         * config items for this server instance
         */
        server_config m_config;

        std::shared_ptr<tcp_socket> m_socket;

        bool m_connection_arrived;

        std::mutex m_lock;
        std::condition_variable m_condition;

        thread_pool<ui_connection> &m_pool;

        std::shared_ptr<std::thread> m_receive_thread;

        /**
         * wait for a connection to arrive, then return
         */
        void wait_for_connection();

        /**
         * handle a stream of 1 or more requests over a connection
         */
        void handle_requests();

        /**
         * put the thread back in its pool
         */
        void done();
    };
}
