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
