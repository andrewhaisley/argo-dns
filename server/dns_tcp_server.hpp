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
#include "socket_set.hpp"

#include "dns_handler.hpp"
#include "dns_message_envelope.hpp"
#include "dns_tcp_connection.hpp"
#include "tcp_server_socket.hpp"
#include "dns_auth_resolver.hpp"
#include "dns_forwarding_resolver.hpp"

EXCEPTION_CLASS(dns_tcp_server_exception, exception)

namespace adns
{
    /**
     * Serve DNS requests of all kinds.
     */
    class dns_tcp_server final : public server
    {
    public:

        /**
         * Construct a server ready to run...
         */
        dns_tcp_server(boost::asio::io_service &io_service, const server_config &config);

        /**
         * destructor - destroy threads etc.
         */
        virtual ~dns_tcp_server();

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
         * thread to run the input side and trigger the reception of messages.
         */
        std::shared_ptr<std::thread> m_receive_thread;

        /** 
         * pool of threads to service connections
         */
        thread_pool<dns_tcp_connection> m_server_threads;

        // authoritative resolver instance shared by all connections
        std::shared_ptr<dns_auth_resolver> m_auth_resolver;

        // recursive resolver instance shared by all connections
        std::shared_ptr<dns_recursive_resolver> m_recursive_resolver;

        // forwarding resolver instance shared by all connections
        std::shared_ptr<dns_forwarding_resolver> m_forwarding_resolver;

        // sockets for in and outbound traffic (one per configured ip/port pair)
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
