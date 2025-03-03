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
#include <memory.h>

#include <boost/lexical_cast.hpp>

#include "types.hpp"
#include "socket_address.hpp"

using namespace std;
using namespace adns;

socket_address::socket_address()
{
    m_port = 0;
    memset(&m_saddr, 0, sizeof(struct sockaddr_in));
    memset(&m_saddr_6, 0, sizeof(struct sockaddr_in6));
}

socket_address::socket_address(const ip_address &addr, int port)
{
    m_ip_address = addr;
    m_port = port;

    memset(&m_saddr, 0, sizeof(struct sockaddr_in));
    memset(&m_saddr_6, 0, sizeof(struct sockaddr_in6));

    m_saddr.sin_family = AF_INET;
    m_saddr.sin_port = htons(m_port);

    memcpy(&m_saddr.sin_addr, &m_ip_address.m_in_addr, sizeof(m_ip_address.m_in_addr));

    m_saddr_6.sin6_family = AF_INET6;
    m_saddr_6.sin6_port = htons(m_port);

    memcpy(&m_saddr_6.sin6_addr, &m_ip_address.m_in6_addr, sizeof(m_ip_address.m_in6_addr));
}

socket_address::~socket_address()
{
}

const ip_address &socket_address::get_ip_address() const
{
    return m_ip_address;
}

int socket_address::get_port() const
{
    return m_port;
}

socket_address::socket_address(struct sockaddr_in a)
{
    m_saddr = a;
    m_port = ntohs(a.sin_port);
    m_ip_address = ip_address(a.sin_addr);
}

socket_address::socket_address(struct sockaddr_in6 a)
{
    m_saddr_6 = a;
    m_port = ntohs(a.sin6_port);
    m_ip_address = ip_address(a.sin6_addr);
}

bool socket_address::operator==(const socket_address &other) const
{
    return (m_ip_address == other.m_ip_address) && (m_port == other.m_port);
}

bool socket_address::operator!=(const socket_address &other) const
{
    return (m_ip_address != other.m_ip_address) || (m_port != other.m_port);
}

bool socket_address::operator<(const socket_address &other) const
{
    return (m_ip_address < other.m_ip_address) || ((m_ip_address == other.m_ip_address) && (m_port < other.m_port));
}

string socket_address::to_string() const
{
    return m_ip_address.to_string() + ":" + boost::lexical_cast<string>(m_port);
}

ostream &adns::operator<<(ostream& stream, const socket_address &sa)
{
    stream << sa.to_string();
    return stream;
}
