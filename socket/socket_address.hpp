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

#include "types.hpp"
#include "ip_address.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(socket_address_exception, exception)

namespace adns
{
    /**
     * Socket address - i.e. IP address + port
     */
    class socket_address
    {
    public:
        friend class socket;
        friend class udp_socket;
        friend class tcp_client_socket;
        friend class tcp_server_socket;

        /**
         * invalid address
         */
        socket_address();

        /**
         * new address. If port is 0, then, at bind time, it will be set to a random
         * number.
         */
        socket_address(const ip_address &addr, int port = 0);

        /**
         * destructor
         */
        virtual ~socket_address();

        /**
         * get the IP address associated with the socket address
         */
        const ip_address &get_ip_address() const;

        /**
         * get the port
         */
        int get_port() const;

        /**
         * test for equality - same address, same port
         */
        bool operator==(const socket_address &other) const;

        /**
         * test for inequality -different address or port
         */
        bool operator!=(const socket_address &other) const;

        /**
         * less than test - ip4 addresses are always less than ip6 addresses
         */
        bool operator<(const socket_address &other) const;

        /**
         * string representation
         */
        std::string to_string() const;

    private:

        ip_address m_ip_address;
        int m_port;

        struct sockaddr_in   m_saddr;
        struct sockaddr_in6  m_saddr_6;

        /**
         * constructor from low level IP 4 socket address
         */
        socket_address(struct sockaddr_in a);

        /**
         * constructor from low level IP 6 socket address
         */
        socket_address(struct sockaddr_in6 a);

    };

    std::ostream &operator<<(std::ostream& stream, const socket_address &sa);
}
