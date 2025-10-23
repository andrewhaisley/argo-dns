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
#include "ui_connection.hpp"

EXCEPTION_CLASS(ui_server_exception, exception)

namespace adns
{
    /**
     * Serve ui requests through http1/http2
     */
    class ui_server final : public server
    {
    public:

        /**
         * Construct a server ready to run...
         */
        ui_server(boost::asio::io_service &io_service, const server_config &config);

        /**
         * destructor - destroy threads etc.
         */
        virtual ~ui_server();

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
        thread_pool<ui_connection> m_server_threads;

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
