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
#include "json_serializable.hpp"

EXCEPTION_CLASS(client_config_exception, exception)

namespace adns
{
    class client_config : public json_serializable
    {
    public:

        uuid client_id;

        uint server_port;
        bool use_ip4;
        bool use_ip6;
        bool use_udp;
        bool use_tcp;
        uint num_parallel_udp;
        uint total_timeout_ms;
        uint udp_timeout_ms;
        uint wait_udp_response_ms;
        uint connect_tcp_timeout_ms;
        uint read_tcp_timeout_ms;
        uint write_tcp_timeout_ms;

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
