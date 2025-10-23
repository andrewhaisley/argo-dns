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
