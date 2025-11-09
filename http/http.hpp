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

EXCEPTION_CLASS(http_exception, exception)
EXCEPTION_CLASS(http_payload_too_large_exception, http_exception)
EXCEPTION_CLASS(http_header_line_too_long, http_exception)
EXCEPTION_CLASS(http_bad_format_exception, http_exception)

#define HTTP_MAX_HEADER_LINE_LENGTH 1000

namespace adns
{
    class http_impl;

    class http
    {
    public:

        typedef enum
        {
            api_e,
            doh_e,
            ui_e
        }
        usage_t;

        /**
         * new HTTP session. Note that this is stateful and designed to live for the length
         * of the HTTP connection. HTTP 1.1 and HTTP 2 are handled with the protocol choice
         * coming from the socket and negotiated at connection time (ALPN).
         */
        http(usage_t u, tcp_socket &s, uint timeout);

        /**
         * cleanup
         */
        virtual ~http();

        /**
         * Read a request from a stream socket. The payload (if any) is assumed
         * to be in JSON format. HTTP 1.1 / HTTP 2 is discovered from the socket.
         */
        virtual std::shared_ptr<http_request> from_wire();

        /**
         * Send a response to a stream socket. 
         */
        virtual void to_wire(http_response &res);

        /**
         * did the last request (or the protocol otherwise) specify the connection should be
         * kept alive?
         */
        bool keep_alive();

    private:

        http_impl *m_protocol;

    };
}
