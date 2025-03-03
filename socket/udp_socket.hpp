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

#include "socket.hpp"
#include "socket_address.hpp"
#include "buffer.hpp"

#define MAX_UDP_MESSAGE_SIZE 65536

EXCEPTION_CLASS(udp_socket_exception, socket_exception)
EXCEPTION_CLASS(udp_socket_timeout_exception, udp_socket_exception)

namespace adns
{
    /**
     * UDP socket.
     */
    class udp_socket : public socket
    {
    public:

        /** 
         * wrapper of a message from a socket along with its source address
         */
        class message final
        {
        public:

            message(buffer &b, socket_address &remote_address);
            const buffer &get_message() const noexcept;
            const socket_address &get_remote_address() const noexcept;

        private:
        
            buffer m_message;
            socket_address m_remote_address;
        };

        /**
         * socket not bound to any address. Default to IPV4 socket.
         */
        udp_socket(ip_address::ip_type_t t = ip_address::ip_v4_e);

        /**
         * socket bound to the given address (IP type is derived from the address).
         */
        udp_socket(const socket_address &sa);

        /**
         * close and destroy
         */
        virtual ~udp_socket();

        /**
         * is a stream socket? (returns false)
         */
        virtual bool is_stream();

        /**
         * receive a message on a previously bound socket.
         */
        buffer receive(socket_address &remote_address, uint timeout_ms);

        /**
         * receive up to N available messages (could be 0) from a previously bound socket in non-blocking mode.
         * @param   IN  max      - maximum number of messages to be read
         * @param   OUT messages - messages read
         * @return  number of messages read
         */
        int receive_many(int max, message **messages);

        /**
         * send a message on a socket (shouldn't be bound first).
         */
        void send(const buffer &data, const socket_address &remote_address);

    };
}
