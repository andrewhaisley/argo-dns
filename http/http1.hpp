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
