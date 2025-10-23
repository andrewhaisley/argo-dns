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
