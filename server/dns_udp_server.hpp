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
#include <thread>

#include <boost/asio.hpp>

#include "types.hpp"
#include "config.hpp"
#include "server.hpp"
#include "server_config.hpp"
#include "handler_pool.hpp"
#include "socket_set.hpp"

#include "dns_handler.hpp"
#include "dns_message_envelope.hpp"
#include "udp_socket.hpp"
#include "dns_auth_resolver.hpp"
#include "dns_forwarding_resolver.hpp"
#include "dns_recursive_resolver.hpp"

namespace adns
{
    /**
     * Serve DNS requests of all kinds.
     */
    class dns_udp_server final : public server
    {
    public:

        /**
         * Construct a server ready to run...
         */
        dns_udp_server(
                boost::asio::io_service &io_service,
                const server_config     &config);

        virtual ~dns_udp_server();

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
         * Name
         */
        virtual std::string name() const;

    private:

        /**
         * config items for this server instance
         */
        server_config m_config;

        /**
         * sockets for in and outbound traffic (one per configured ip/port pair)
         */
        socket_set m_socket_set;

        /**
         * queue with attached pool of dns handler threads
         */
        handler_pool<dns_message_envelope *, dns_handler, dns_handler::params_t> *m_handler_pool;

        /**
         * threads to run the input side and receive messages.
         */
        std::list<std::shared_ptr<std::thread>> m_receive_threads;

        /**
         * threads to pick up messages back from the thread pool and send them out.
         */
        std::list<std::shared_ptr<std::thread>> m_send_threads;

        /** 
         * authoritatve resolver instance sharded by all threads
         */
        std::shared_ptr<dns_auth_resolver> m_auth_resolver;

        /** 
         * recursive resolver instance sharded by all threads
         */
        std::shared_ptr<dns_recursive_resolver> m_recursive_resolver;

        /** 
         * forwarding resolver instance sharded by all threads
         */
        std::shared_ptr<dns_forwarding_resolver> m_forwarding_resolver;

        /** 
         * Wait for things and handle them.
         */
        void receive_inbound();

        /**
         * Pick up messages from the thread pool and send them out.
         */
        void send_outbound();

    };
}
