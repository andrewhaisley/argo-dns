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

#include <string>
#include <list>

#include "types.hpp"
#include "exception.hpp"
#include "json_serializable.hpp"
#include "client_config.hpp"

EXCEPTION_CLASS(server_config_exception, exception)

namespace adns
{
    class address_list;

    class server_config : public json_serializable
    {
    public:

        typedef enum
        {
            all_e = 1,
            dns_udp_e = 2,
            dns_tcp_e = 3,
            dns_dot_e = 4,
            dns_doh_e = 5,
            control_e = 6,
            ui_e      = 7
        }
        server_type_t;

        typedef struct
        {
            std::string ip_address;
            uint        port;
        }
        socket_address_t;

        uuid                        server_id;
        bool                        enabled;
        server_type_t               type;
        std::string                 protocol;
        std::string                 transport;
        std::list<socket_address_t> socket_addresses;

        uint http2_deflate_table_size;

        class _dns
        {
        public:
            uint   max_in_message_queue_length;
            uint   max_out_message_queue_length;
            uint   num_udp_threads;
            uint   num_tcp_threads;
            uint   num_forwarding_threads;
            uint   num_send_threads;
            uint   num_receive_threads;
            uint   allowed_connection_backlog;
            uint   tcp_max_len_wait_ms;
            uint   tcp_max_body_wait_ms;
            uint   num_udp_recursive_slots;
            uint   num_tcp_recursive_slots;
            uint   recursive_timeout_ms;
            uint   max_recursion_depth;
            uint   max_external_resolutions;
            uint   min_cache_ttl;
            uint   udp_reply_timeout_ms;
            uint   udp_retry_count;
            uint   max_queued_per_question;

            // DoH specific items
            uint        doh_client_timeout_ms;
            uint        maximum_http_request_size;
            std::string doh_path;

            // base config for the DNS clients used by this server
            client_config client;
        };

        class _control
        {
        public:
            bool    use_ssl;
            uint    num_threads;
            uint    allowed_connection_backlog;
            uint    maximum_payload_size;
            uint    client_connection_timeout_ms;

            // list of addresses/networks from which control server connections may be made
            std::shared_ptr<address_list> control_address_list;
        };

        class _ui
        {
        public:
            bool        use_ssl;
            uint        num_threads;
            uint        allowed_connection_backlog;
            uint        client_connection_timeout_ms;
            std::string document_root; // location of HTLM/CSS/JS to serve

            // list of addresses/networks from which UI server connections may be made
            std::shared_ptr<address_list> ui_address_list;
        };

        _dns     dns;
        _control control;
        _ui      ui;

        /**
         * 'dns,udp' -> dns_udp_e etc
         */
        static server_type_t string_to_server_type(const std::string &protocol, const std::string &transport);

        /**
         * compare by server_id
         */
        bool operator==(const server_config &other) const;

    protected:

        /**
         * Turn into a json object
         */
        virtual void json_serialize() const;

        /**
         * Populated from json object
         */
        virtual void json_unserialize();

    };
}
