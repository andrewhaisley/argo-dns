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
