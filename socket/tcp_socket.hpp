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
