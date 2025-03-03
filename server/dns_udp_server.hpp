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
