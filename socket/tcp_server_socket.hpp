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

#include "types.hpp"
#include "socket_address.hpp"
#include "buffer.hpp"
#include "tcp_socket.hpp"
#include "exception.hpp"

namespace adns
{
    /**
     * TCP server sockets.
     */
    class tcp_server_socket : public tcp_socket
    {
    public:

        friend class ssl_server_socket;

        /**
         * socket bound to the given address (IP type is derived from the address).
         */
        tcp_server_socket(const socket_address &sa);

        /**
         * destructor - close etc.
         */
        virtual ~tcp_server_socket();

        /**
         * listen for new connections.
         */
        void listen(int backlog);

        /**
         * accept a new connection - blocks until one arrives.
         */
        virtual std::shared_ptr<tcp_server_socket> accept();

        /**
         * accept any new connection queued up - non-blocking
         */
        virtual std::list<std::shared_ptr<tcp_server_socket>> accept_many();

    protected:

        /**
         * constructor used for the FD returned from ::accept
         */
        tcp_server_socket(int fd, const socket_address &sa);

    };
}
