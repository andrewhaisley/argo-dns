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

#include "dns_message_parser.hpp"
#include "types.hpp"
#include "buffer.hpp"
#include "socket_address.hpp"
#include "udp_socket.hpp"

namespace adns
{
    /**
     * holds a raw message (i.e. in binary form) along with the ip address from which it originated.
     */
    class dns_message_envelope final
    {
    public:

        /**
         * empty message
         */
        dns_message_envelope();

        /**
         * constructor. 
         */
        dns_message_envelope(
                const buffer         &raw, 
                const socket_address &remote_address,
                udp_socket           *s = nullptr);

        /**
         * Copy constructor - deep copy
         */
        dns_message_envelope(const dns_message_envelope &other);

        /**
         * destructor
         */
        virtual ~dns_message_envelope();

        /**
         * set the the raw message 
         */
        void set_raw(const buffer &raw);

        /**
         * set a parsed request - takes ownership
         */
        void set_request(dns_message *request);

        /**
         * set a response - takes ownership
         */
        void set_response(dns_message *response);

        /**
         * get the the raw message 
         */
        const buffer &get_raw() const;

        /**
         * get the remote address - i.e. where it was sent from.
         */
        const socket_address &get_remote_address() const;

        /**
         * get the parsed request (does not give up ownership)
         */
        dns_message *get_request() const;

        /**
         * get the parsed response (does not give up ownership)
         */
        dns_message *get_response() const;

        /** 
         * get the socket the message was received on
         */
        udp_socket *get_socket() const;

        /**
         * Set the channel (used for multiplexing)
         */
        void set_channel(int channel);

        /**
         * Get the channel (used for multiplexing)
         */
        int get_channel() const;

        /**
         * Set forwarding details
         */
        void set_forwarding(const ip_address &forward_ip, int forward_port);

        /**
         * Get forwarding ip address
         */
        const ip_address &get_forwarding_address() const;

        /**
         * Get forwarding port
         */
        int get_forwarding_port() const;

        /**
         * set the minimum TTL of a record in the contained message
         */
        void set_min_ttl(int min_ttl);

        /**
         * get the minimum TTL of a record in the contained message
         */
        int get_min_ttl() const;

    private:

        buffer         m_raw;
        socket_address m_remote_address;
        int            m_channel;
        int            m_min_ttl;

        udp_socket  *m_socket;

        dns_message *m_request;
        dns_message *m_response;

        ip_address m_forward_ip;
        int        m_forward_port;
    };
}
