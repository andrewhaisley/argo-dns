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

#include <memory>
#include <string>

#include "types.hpp"
#include "exception.hpp"
#include "tcp_socket.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "http.hpp"
#include "http_impl.hpp"

EXCEPTION_CLASS(http1_exception, exception)

namespace adns
{
    class http1 : public http_impl
    {
    public:

        /**
         * new HTTP session. Note that this is stateful and designed to live for the length
         * of the HTTP connection. HTTP 1.1 and HTTP 2 are handled with the protocol choice
         * coming from the socket and negotiated at connection time.
         */
        http1(http::usage_t u, tcp_socket &s, uint timeout);

        /**
         * cleanup
         */
        virtual ~http1();

        /**
         * Read a request from a stream socket. The payload (if any) is assumed
         * to be in JSON format. HTTP 1.1 / HTTP 2 is discovered from the socket.
         */
        std::shared_ptr<http_request> from_wire();

        /**
         * Send a response to a stream socket. 
         */
        virtual void to_wire(http_response &res);

    private:

        // what the session is being used for
        http::usage_t m_usage;

        // socket for the session
        tcp_socket &m_socket;

        // TCP read/write timeout in ms
        uint m_timeout;

        /**
         * read a single line of text from the socket
         */
        std::string read_line();

    };
}
