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
