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

    private:

        http_impl *m_protocol;

    };
}
