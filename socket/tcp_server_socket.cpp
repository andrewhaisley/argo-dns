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
#include "tcp_server_socket.hpp"
#include "run_state.hpp"

using namespace std;
using namespace chrono;
using namespace adns;

tcp_server_socket::tcp_server_socket(const socket_address &sa) : tcp_socket()
{
    create_socket(sa.get_ip_address().get_type(), true);
    bind_socket(sa);
}

tcp_server_socket::tcp_server_socket(int fd, const socket_address &sa) : tcp_socket(fd, sa)
{
}

tcp_server_socket::~tcp_server_socket()
{
}

void tcp_server_socket::listen(int backlog)
{
    if (::listen(m_fd, backlog) != 0)
    {
        THROW(tcp_socket_exception, "listen() failed", util::strerror(), errno);
    }
}

shared_ptr<tcp_server_socket> tcp_server_socket::accept()
{
    int res;

    struct sockaddr remote;
    socklen_t remote_len = sizeof(remote);

    res = ::accept(m_fd, (struct sockaddr *)&remote, &remote_len);
    if (res == -1)
    {
        if (errno == EAGAIN)
        {
            THROW(tcp_socket_timeout_exception, "read timed out");
        }
        else
        {
            THROW(tcp_socket_exception, "accept() failed", util::strerror(), errno);
        }
    }
    else
    {
        if (remote.sa_family == AF_INET)
        {
            struct sockaddr_in *sa = (struct sockaddr_in *)&remote;
            return shared_ptr<tcp_server_socket>(new tcp_server_socket(res, socket_address(*sa)));
        }
        else if (remote.sa_family == AF_INET6)
        {
            struct sockaddr_in6 *sa = (struct sockaddr_in6 *)&remote;
            return shared_ptr<tcp_server_socket>(new tcp_server_socket(res, socket_address(*sa)));
        }
        else
        {
            THROW(tcp_socket_exception, "uknown protocol family returned from accept", remote.sa_family);
        }
    }
}

list<shared_ptr<tcp_server_socket>> tcp_server_socket::accept_many()
{
    list<shared_ptr<tcp_server_socket>> sockets;

    int res;
    struct sockaddr remote;
    socklen_t remote_len = sizeof(remote);

    while (true)
    {
        res = ::accept(m_fd, (struct sockaddr *)&remote, &remote_len);

        if (res == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return sockets;
            }
            else
            {
                THROW(tcp_socket_exception, "accept() failed", util::strerror(), errno);
            }
        }
        else
        {
            if (remote.sa_family == AF_INET)
            {
                struct sockaddr_in *sa = (struct sockaddr_in *)&remote;
                sockets.push_back(shared_ptr<tcp_server_socket>(new tcp_server_socket(res, socket_address(*sa))));
            }
            else if (remote.sa_family == AF_INET6)
            {
                struct sockaddr_in6 *sa = (struct sockaddr_in6 *)&remote;
                sockets.push_back(shared_ptr<tcp_server_socket>(new tcp_server_socket(res, socket_address(*sa))));
            }
            else
            {
                THROW(tcp_socket_exception, "uknown protocol family returned from accept", remote.sa_family);
            }
        }
    }
}
