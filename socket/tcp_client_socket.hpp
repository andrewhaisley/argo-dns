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

#include <list>

#include "tcp_socket.hpp"
#include "exception.hpp"

namespace adns
{
    /**
     * TCP client sockets.
     */
    class tcp_client_socket : public tcp_socket
    {
    public:

        /**
         * open but not connected socket of given IP type
         */
        tcp_client_socket(ip_address::ip_type_t t = ip_address::ip_v4_e);

        /**
         * destructor - close etc.
         */
        virtual ~tcp_client_socket();

        /**
         * connect to a remote server
         */
        void connect(const socket_address &remote_address);

        /**
         * start an asychronous connect operation
         */
        void start_connect(const socket_address &remote_address);

    };
}
