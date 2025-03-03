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
