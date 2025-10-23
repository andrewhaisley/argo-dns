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
