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

#include "buffer.hpp"
#include "thread_pool.hpp"
#include "handler_pool.hpp"
#include "dns_message_envelope.hpp"
#include "dns_handler.hpp"
#include "tcp_socket.hpp"
#include "server_config.hpp"
#include "http_request.hpp"
#include "http_response.hpp"

EXCEPTION_CLASS(dns_doh_connection_exception, exception)
EXCEPTION_CLASS(dns_doh_connection_timeout_exception, dns_doh_connection_exception)
EXCEPTION_CLASS(dns_doh_connection_invalid_method_exception, dns_doh_connection_exception)

namespace adns
{
    /**
     * Handle a single https connection. Reads messages off the wire, passes
     * them onto a handler, takes back the results and puts them back
     * on the wire.
     */
    class dns_doh_connection final
    {
    public:

        /** 
         * New connection ready to handle incoming requests. Automatically returns itself
         * to the thread pool when done and ready to service a new connection.
         */
        dns_doh_connection(
                    server_config                            &config, 
                    std::shared_ptr<dns_auth_resolver>       p_auth_solver,
                    std::shared_ptr<dns_recursive_resolver>  p_recursice_solver,
                    std::shared_ptr<dns_forwarding_resolver> p_forwarding_solver,
                    thread_pool<dns_doh_connection>          &pool);

        /**
         * kill threads etc.
         */
        virtual ~dns_doh_connection();

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

        // config items for this server instance
        server_config m_config;

        std::shared_ptr<tcp_socket> m_socket;

        bool m_connection_arrived;

        std::mutex m_lock;
        std::condition_variable m_condition;

        thread_pool<dns_doh_connection> &m_pool;
        std::shared_ptr<std::thread> m_receive_thread;

        /**
         * pool of exactly one handler for TCP
         */
        handler_pool<dns_message_envelope *, dns_handler, dns_handler::params_t> *m_handler_pool;

        /**
         * wait for a connection to arrive, then return
         */
        void wait_for_connection();

        /**
         * put the thread back in its pool
         */
        void done();
        
        /**
         * extract the DNS query from a POST request
         */
        void extract_post_request(std::shared_ptr<http_request> &ref, buffer &b);

        /**
         * extract the DNS query from a GET request
         */
        void extract_get_request(std::shared_ptr<http_request> &ref, buffer &b);
    
    };
}
