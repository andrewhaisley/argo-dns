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

#include "dns_message_envelope.hpp"
#include "message_queue.hpp"
#include "dns_message_envelope.hpp"
#include "types.hpp"
#include "socket_address.hpp"
#include "server_config.hpp"
#include "dns_recursive_resolver.hpp"
#include "dns_auth_resolver.hpp"
#include "dns_forwarding_resolver.hpp"

namespace adns
{
    class dns_handler final
    {
    public:

        struct params_t
        {
            server_config                            config;
            std::shared_ptr<dns_auth_resolver>       p_auth_resolver;
            std::shared_ptr<dns_recursive_resolver>  p_recursive_resolver;
            std::shared_ptr<dns_forwarding_resolver> p_forwarding_resolver;
            bool                                     is_tcp;
            bool                                     check_message_length;
        };

        /**
         * constructor - set up any thread specific data needed
         */
        dns_handler(const params_t &params, message_queue<dns_message_envelope *> &q);

        /**
         * destructor
         */
        virtual ~dns_handler();

        /**
         * process a single raw message
         */
        void process(dns_message_envelope *m);

        /**
         * join any running threads
         */
        void join();

    private:

        // DNS message parser/unparser instance
        dns_message_parser m_parser;

        // server configuration options
        server_config m_config;

        // authoritative resolver instance
        std::shared_ptr<dns_auth_resolver> m_auth_resolver;

        // recursive resolver instance
        std::shared_ptr<dns_recursive_resolver> m_recursive_resolver;

        // forwarding resolver instance
        std::shared_ptr<dns_forwarding_resolver> m_forwarding_resolver;

        // thread that gets responses from the recursive resolver and forwards them
        std::shared_ptr<std::thread> m_resolver_thread;

        // thread that gets responses from the forwarded resolver and sends them back
        std::shared_ptr<std::thread> m_forwarder_thread;

        // outgoing message queue
        message_queue<dns_message_envelope *> &m_out_queue;

        // queues when using a multiplexer resolver (for TCP)
        std::shared_ptr<message_queue<dns_message_envelope *>> m_to_resolver;
        std::shared_ptr<message_queue<dns_message_envelope *>> m_from_resolver;

        // whether to check the outgoing message length - for UDP, yes, for TCP just
        // check lengths over 64K
        bool m_check_message_length;

        // TCP or UDP origin?
        bool m_is_tcp;

        /**
         * Send an error response to a message that couldn't be fully parsed.
         */
        void respond_with_parse_error(
                        dns_message_envelope         *m, 
                        dns_message::response_code_t rc);

        /**
         * Send an error response to a message that could be fully parsed.
         */
        void respond_with_error(
                        dns_message_envelope         *m,
                        bool                         allow_recursion,
                        dns_message::response_code_t rc);

        /**
         * Given a request that could be parsed, figure out if it's asking for something
         * we don't support (yet).
         */
        bool feature_not_supported(dns_message *request, std::string &notes);

        /**
         * handle a request message
         */
        void handle_request(dns_message_envelope *m, const dns_horizon *h);

        /**
         * send a response dealing with EDNS sizing etc.
         */
        void send_response(dns_message_envelope *m, bool allow_recursion);

        /**
         * get responses from the recursive resolver and forward them on - works in its own thread
         */
        void get_resolver_responses();

        /**
         * get responses from the forwarded resolver and forward them on - works in its own thread
         */
        void get_forwarder_responses();

        /**
         * Dequeue from the resolver - will be direct for UDP, via the multiplexer for TCP
         */
        dns_message_envelope *dequeue_from_resolver();

        /**
         * Enqueue on the resolver - will be direct for UDP, via the multiplexer for TCP
         */
        void enqueue_to_resolver(dns_message_envelope *m);

        /**
         * Send the result back out of the handler pool
         */
        void enqueue_to_return(dns_message_envelope *m);
    };
}
