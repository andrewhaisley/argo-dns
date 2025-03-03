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
#include <string.h>

#include <chrono>

#include "util.hpp"
#include "tcp_client_socket.hpp"
#include "run_state.hpp"

using namespace std;
using namespace chrono;
using namespace adns;

tcp_client_socket::tcp_client_socket(ip_address::ip_type_t t) : tcp_socket(t)
{
}

tcp_client_socket::~tcp_client_socket()
{
}

void tcp_client_socket::connect(const socket_address &remote_address)
{
    m_remote_address = remote_address;

    if (remote_address.get_ip_address().get_type() == ip_address::ip_v4_e)
    {
        if (::connect(m_fd, (const struct sockaddr *)&remote_address.m_saddr, sizeof(remote_address.m_saddr)) < 0)
        {
            THROW(tcp_socket_exception, "connect() failed", util::strerror(), errno);
        }
    }
    else if (remote_address.get_ip_address().get_type() == ip_address::ip_v6_e)
    {
        if (::connect(m_fd, (const struct sockaddr *)&remote_address.m_saddr_6, sizeof(remote_address.m_saddr_6)) < 0)
        {
            THROW(tcp_socket_exception, "connect() failed", util::strerror(), errno);
        }
    }
    else
    {
        THROW(tcp_socket_exception, "invalid socket type", remote_address.get_ip_address().get_type());
    }
}

void tcp_client_socket::start_connect(const socket_address &remote_address)
{
    m_remote_address = remote_address;

    set_non_blocking();

    int e = 0;

    if (remote_address.get_ip_address().get_type() == ip_address::ip_v4_e)
    {
        e = ::connect(m_fd, (const struct sockaddr *)&remote_address.m_saddr, sizeof(remote_address.m_saddr));
    }
    else if (remote_address.get_ip_address().get_type() == ip_address::ip_v6_e)
    {
        e = ::connect(m_fd, (const struct sockaddr *)&remote_address.m_saddr_6, sizeof(remote_address.m_saddr_6));
    }
    else
    {
        THROW(tcp_socket_exception, "invalid socket type", remote_address.get_ip_address().get_type());
    }

    if ((e != 0) && (errno != EINPROGRESS))
    {
        THROW(tcp_socket_exception, "start_connect() failed", util::strerror(), errno);
    }
}
