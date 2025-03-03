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

    private:

        buffer         m_raw;
        socket_address m_remote_address;
        int            m_channel;

        udp_socket  *m_socket;

        dns_message *m_request;
        dns_message *m_response;

        ip_address m_forward_ip;
        int        m_forward_port;
    };
}
