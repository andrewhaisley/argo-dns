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
#include "socket.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(tcp_socket_exception, socket_exception)
EXCEPTION_CLASS(tcp_socket_timeout_exception, tcp_socket_exception)
EXCEPTION_CLASS(tcp_socket_eof_exception, tcp_socket_exception)

namespace adns
{
    /**
     * TCP sockets.
     */
    class tcp_socket : public socket
    {
    public:

        // the type of a protocol negotiated as part of Application-Layer Protocol Negotiation (ALPN)
        typedef enum
        {
            http_1_1_e,
            http_2_e,
            none_e
        }
        negotiated_protocol_e;

        /**
         * new, unbound socket
         */
        tcp_socket(ip_address::ip_type_t t = ip_address::ip_v4_e);

        /**
         * destructor - close etc.
         */
        virtual ~tcp_socket();

        /**
         * is a stream (yes, it's TCP)
         */
        virtual bool is_stream();

        /**
         * shutdown a connected socket - the underlying FD is not closed
         */
        void shutdown();

        /**
         * read N bytes with a timeout - throw tcp_socket_exception in that case.
         */
        virtual buffer read(int n, uint timeout_ms);

        /**
         * write buffer with a timeout - throw tcp_socket_exception in that case.
         */
        virtual void write(const buffer &data, uint timeout_ms);

        /**
         * write data with a timeout - throw tcp_socket_exception in that case.
         */
        virtual void write(const octet *data, uint n, uint timeout_ms);

        /**
         * get the remote address - only valid for connected sockets
         */
        const socket_address &get_remote_address();

        /**
         * the type of protocol negotiated at connection time
         */
        negotiated_protocol_e get_protocol() const;

        /**
         * get the unique ID of the socket - used for debug
         */
        uint get_socket_id() const;

    protected:

        /**
         * constructor for when the FD is known as a result of an accept call
         */
        tcp_socket(int fd, const socket_address &sa);

        /**
         * only relevant for connected sockets (e.g. those returned from accept or those
         * post a call to connect).
         */
        socket_address m_remote_address;

        // only currently relevant to SSH server sockets
        negotiated_protocol_e m_protocol;

        // socket ID - used for debug purposes
        uint m_socket_id;
    };
}
