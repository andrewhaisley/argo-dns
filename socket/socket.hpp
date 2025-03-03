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

#include "socket_address.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(socket_exception, exception)
EXCEPTION_CLASS(socket_timeout_exception, socket_exception)

namespace adns
{
    class socket
    {
    public:

        friend class socket_set;

        /**
         * no copies - don't want things sharing the same file descriptor
         */
        socket(const socket &other) = delete;

        /**
         * no assignment - don't want things sharing the same file descriptor
         */
        const socket &operator=(const socket &s) = delete;

        /**
         * close & destroy.
         */
        virtual ~socket();

        /**
         * make non-blocking
         */
        void set_non_blocking();

        /**
         * set a timeout in ms for receive operations
         */
        void set_receive_timeout(uint timeout_ms);

        /**
         * set a timeout in ms for send operations
         */
        void set_send_timeout(uint timeout_ms);

        /**
         * is this a streaming socket?
         */
        virtual bool is_stream() = 0;

    protected:

        // underlying file descriptor
        int m_fd;

        // ipv4 or ipv6
        ip_address::ip_type_t m_ip_type;

        /**
         * basic socket - all setup is done in the derived classes
         */
        socket();

        /**
         * socket where the FD and IP type are known (e.g. from accept call)
         */
        socket(int fd, ip_address::ip_type_t t);

        /**
         * create the underlying socket
         */
        void create_socket(ip_address::ip_type_t t, bool is_stream);

        /**
         * bind the socket to a given address
         */
        void bind_socket(const socket_address &sa);

        /** 
         * get the underlying file descriptor
         */
        int get_fd() const;
    };
}
