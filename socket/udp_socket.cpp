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

#include <sys/types.h>
#include <sys/socket.h>

#include <chrono>

#include "udp_socket.hpp"
#include "util.hpp"
#include "run_state.hpp"

using namespace std;
using namespace chrono;
using namespace adns;

udp_socket::udp_socket(const socket_address &sa) : socket()
{
    create_socket(sa.get_ip_address().get_type(), false);
    bind_socket(sa);
}

udp_socket::udp_socket(ip_address::ip_type_t t): socket()
{
    create_socket(t, false);
}

udp_socket::~udp_socket()
{
}

buffer udp_socket::receive(socket_address &remote_address, uint timeout_ms)
{
    duration<int, milli> timeout(timeout_ms);
    time_point<steady_clock> start_time = steady_clock::now();

    int res = -1;
    socklen_t remote_len;

    octet data[MAX_UDP_MESSAGE_SIZE];

    // don't wait forever on receives - check every 2 seconds for an interrupt allowing
    // the server to shut down cleanly
    set_receive_timeout(2000);

    while (true)
    {
        if (m_ip_type == ip_address::ip_v4_e)
        {
            struct sockaddr_in remote;
            remote_len = sizeof(remote);

            res = ::recvfrom(m_fd, data, MAX_UDP_MESSAGE_SIZE, 0, (struct sockaddr *)&remote, &remote_len);
            if (res != -1)
            {
                remote_address = socket_address(remote);
            }
        }
        else
        {
            struct sockaddr_in6 remote;
            remote_len = sizeof(remote);

            res = ::recvfrom(m_fd, data, MAX_UDP_MESSAGE_SIZE, 0, (struct sockaddr *)&remote, &remote_len);
            if (res != -1)
            {
                remote_address = socket_address(remote);
            }
        }

        if (res ==-1)
        {
            if (errno == EAGAIN)
            {
                time_point<steady_clock> time_now = steady_clock::now();

                if ((time_now - start_time) > timeout)
                {
                    THROW(udp_socket_timeout_exception, "timeout");
                }
            }
            else
            {
                THROW(udp_socket_exception, "recvfrom() failed", util::strerror(), errno);
            }
        }
        else
        {
            return buffer(res, data);
        }

        if (run_state::o_state == run_state::shutdown_e)
        {
            THROW(udp_socket_timeout_exception, "read timed out on shutdown");
        }
    }
}

int udp_socket::receive_many(int max, message **messages)
{
    int num_read = 0;

    int res = -1;
    socklen_t remote_len;

    octet data[MAX_UDP_MESSAGE_SIZE];

    try
    {
        while (true)
        {
            if (run_state::o_state == run_state::shutdown_e)
            {
                THROW(udp_socket_timeout_exception, "read timed out on shutdown");
            }

            socket_address remote_address;

            if (m_ip_type == ip_address::ip_v4_e)
            {
                struct sockaddr_in remote;
                remote_len = sizeof(remote);

                res = ::recvfrom(m_fd, data, MAX_UDP_MESSAGE_SIZE, 0, (struct sockaddr *)&remote, &remote_len);
                if (res != -1)
                {
                    remote_address = socket_address(remote);
                }
            }
            else
            {
                struct sockaddr_in6 remote;
                remote_len = sizeof(remote);

                res = ::recvfrom(m_fd, data, MAX_UDP_MESSAGE_SIZE, 0, (struct sockaddr *)&remote, &remote_len);
                if (res != -1)
                {
                    remote_address = socket_address(remote);
                }
            }

            if (res ==-1)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    return num_read;
                }
                else
                {
                    THROW(udp_socket_exception, "recvfrom() failed", util::strerror(), errno);
                }
            }
            else
            {
                buffer b(res, data);

                messages[num_read++] = new message(b, remote_address);
            
                if (num_read >= max)
                {
                    return num_read;
                }
            }
        }
    }
    catch (...)
    {
        for (auto i = 0; i < num_read; i++)
        {
            delete messages[i];
        }
        throw;
    }
}

void udp_socket::send(const buffer &data, const socket_address &remote_address)
{
    int res = -1;
    if (remote_address.get_ip_address().get_type() == ip_address::ip_v4_e)
    {
        res = ::sendto(m_fd, data.get_data(), data.get_size(), 0, (const struct sockaddr *)&(remote_address.m_saddr), sizeof(struct sockaddr_in));
    }
    else
    {
        res = ::sendto(m_fd, data.get_data(), data.get_size(), 0, (const struct sockaddr *)&(remote_address.m_saddr_6), sizeof(struct sockaddr_in6));
    }

    if (res == -1)
    {
        THROW(udp_socket_exception, "sendto() failed", util::strerror(), errno);
    }
}

udp_socket::message::message(buffer &b, socket_address &remote_address) : 
                                            m_message(b), m_remote_address(remote_address)
{
}

const buffer &udp_socket::message::get_message() const noexcept
{
    return m_message;
}

const socket_address &udp_socket::message::get_remote_address() const noexcept
{
    return m_remote_address;
}

bool udp_socket::is_stream()
{
    return false;
}
