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
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>

#include "types.hpp"
#include "util.hpp"
#include "socket.hpp"

using namespace std;
using namespace adns;

socket::socket() : m_fd(-1)
{
}

socket::socket(int fd, ip_address::ip_type_t t) : m_fd(fd), m_ip_type(t)
{
}

socket::~socket()
{
    if (m_fd != -1)
    {
        ::close(m_fd);
        m_fd = -1;
    }
}

void socket::bind_socket(const socket_address &sa)
{
    if (sa.get_ip_address().get_type() == ip_address::none_e)
    {
        THROW(socket_exception, "attempt to bind() to uninitialised ip address");
    }

    int res;
    int opt = 1;

    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
    {
        THROW(socket_exception, "setsockopt() SO_REUSEADDR failed", util::strerror(), errno);
    }

    if (sa.get_ip_address().get_type() == ip_address::ip_v4_e)
    {
        res = ::bind(m_fd, (const struct sockaddr *)&sa.m_saddr, sizeof(sa.m_saddr));
    }
    else
    {
        res = ::bind(m_fd, (const struct sockaddr *)&sa.m_saddr_6, sizeof(sa.m_saddr_6));
    }

    if (res == -1)
    {
        THROW(socket_exception, "bind() failed", util::strerror(), errno);
    }
    else
    {
        LOG(info) << "successfully bound to " << sa;
    }
}

int socket::get_fd() const
{
    return m_fd;
}

void socket::create_socket(ip_address::ip_type_t t, bool is_stream)
{
    int opt = 1;
    m_ip_type = t;

    if (t == ip_address::ip_v4_e)
    {
        m_fd = ::socket(AF_INET, is_stream ? SOCK_STREAM : SOCK_DGRAM, 0);

        if (m_fd == -1)
        {
            THROW(socket_exception, "IP v4 socket() call failed", util::strerror(), errno);
        }
    }
    else
    {
        m_fd = ::socket(AF_INET6, is_stream ? SOCK_STREAM : SOCK_DGRAM, 0);

        if (m_fd == -1)
        {
            THROW(socket_exception, "IP v6 socket() failed", util::strerror(), errno);
        }

        if (setsockopt(m_fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(int)) == -1)
        {
            THROW(socket_exception, "setsockopt() IPV6_V6ONLY failed", util::strerror(), errno);
        }
    }
}

void socket::set_receive_timeout(uint n)
{
    struct timeval tv;
    tv.tv_sec = n / 1000;
    tv.tv_usec = n % 1000;

    if (setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) != 0)
    {
        THROW(socket_exception, "setsockopt() call failed set_receive_timeout()", util::strerror(), errno);
    }
}

void socket::set_send_timeout(uint n)
{
    struct timeval tv;
    tv.tv_sec = n / 1000;
    tv.tv_usec = n % 1000;

    if (setsockopt(m_fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) != 0)
    {
        THROW(socket_exception, "setsockopt() call failed set_send_timeout()", util::strerror(), errno);
    }
}

void socket::set_non_blocking()
{
    int flags;
  
    flags = fcntl(m_fd, F_GETFL, 0);
  
    if (flags == -1)
    {
        THROW(socket_exception, "fcntl() call failed in set_non_blocking", util::strerror(), errno);
    }

    flags |= O_NONBLOCK;

    if (fcntl(m_fd, F_SETFL, flags) == -1)
    {
        THROW(socket_exception, "fcntl() call failed in set_non_blocking", util::strerror(), errno);
    }
}
