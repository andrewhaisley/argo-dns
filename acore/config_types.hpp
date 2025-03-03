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

EXCEPTION_CLASS(config_types_exception, exception)

namespace adns
{
    class config_types
    {
    public:

        typedef enum
        {
            all_e = 1,
            dns_udp_e = 2,
            dns_tcp_e = 3,
            dns_dot_e = 4,
            dns_doh_e = 5,
            control_e = 5
        }
        server_type_t;

        typedef struct
        {
            std::string ip_address;
            uint        port;
        }
        socket_address_t;

        struct server_config_t
        {
            server_type_t  type;

            std::list<socket_address_t> socket_addresses;

            struct
            {
                uint   max_in_message_queue_length;
                uint   max_out_message_queue_length;
                uint   num_threads;
                uint   allowed_connection_backlog;
                uint   tcp_max_len_wait_seconds;
                uint   tcp_max_body_wait_seconds;
                bool           support_edns;
                unsigned short edns_payload_size;
            }
            dns;

            struct
            {
                bool    use_ssl;
                uint    num_threads;
                uint    allowed_connection_backlog;
                uint    maximum_payload_size;
                uint    client_connection_timeout;
                uint    address_list_id;
            }
            control;
        };

        /**
         * 'dns,udp' -> dns_udp_e etc
         */
        static server_type_t string_to_server_type(const std::string &protocol, const std::string &transport);
    };
}
