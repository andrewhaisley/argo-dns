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
