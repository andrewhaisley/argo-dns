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
#include "tcp_socket.hpp"
#include "run_state.hpp"

using namespace std;
using namespace chrono;
using namespace adns;

static atomic<uint> o_next_id = 0;

tcp_socket::tcp_socket(ip_address::ip_type_t t) : socket()
{
    m_socket_id = o_next_id++;
    create_socket(t, true);
    m_protocol = none_e;
    set_receive_timeout(1000);
    set_send_timeout(1000);
}

tcp_socket::tcp_socket(int fd, const socket_address &sa) : socket(fd, sa.get_ip_address().get_type())
{
    m_socket_id = o_next_id++;
    m_remote_address = sa;
    m_protocol = none_e;
    set_receive_timeout(1000);
    set_send_timeout(1000);
}

tcp_socket::negotiated_protocol_e tcp_socket::get_protocol() const
{
    return m_protocol;
}

tcp_socket::~tcp_socket()
{
}

uint tcp_socket::get_socket_id() const
{
    return m_socket_id;
}

bool tcp_socket::is_stream()
{
    return true;
}

buffer tcp_socket::read(int n, uint timeout_ms)
{
    if (n == 0)
    {
        return buffer();
    }

    unique_ptr<octet[]> data(new octet[n]);
    int bytes_read;
    int read_so_far = 0;

    time_point<steady_clock> start_time = steady_clock::now();
    duration<int, milli> timeout(timeout_ms);

    while (true)
    {
        bytes_read = ::read(m_fd, data.get() + read_so_far, n - read_so_far);

        if (bytes_read > 0)
        {
            read_so_far += bytes_read;

            if (read_so_far >= n)
            {
                return buffer(n, data.get());
            }
        }
        else
        {
            if (bytes_read == 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    THROW(tcp_socket_eof_exception, "end of file");
                }
            }
            else
            {
                if (errno != EAGAIN)
                {
                    THROW(tcp_socket_exception, "read() failed", util::strerror(), errno);
                }
            }
        }

        if (run_state::o_state == run_state::shutdown_e)
        {
            THROW(tcp_socket_timeout_exception, "read timed out on shutdown");
        }

        time_point<steady_clock> time_now = steady_clock::now();

        if ((time_now - start_time) > timeout)
        {   
            THROW(tcp_socket_timeout_exception, "read timed out");
        }
    }
}

void tcp_socket::write(const buffer &data, uint timeout_ms)
{
    write(data.get_data(), data.get_size(), timeout_ms);
}

void tcp_socket::write(const octet *data, uint n, uint timeout_ms)
{
    if (n == 0)
    {
        return;
    }

    int bytes_written;
    size_t written_so_far = 0;

    time_point<steady_clock> start_time = steady_clock::now();
    duration<int, milli> timeout(timeout_ms);

    while (true)
    {
        bytes_written = ::write(m_fd, data + written_so_far, n - written_so_far);

        if (bytes_written > 0)
        {
            written_so_far += bytes_written;

            if (written_so_far >= n)
            {
                return;
            }
        }
        else
        {
            if (bytes_written == 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    THROW(tcp_socket_eof_exception, "end of file");
                }
            }
            else
            {
                if (errno != EAGAIN)
                {
                    THROW(tcp_socket_exception, "write() failed", util::strerror(), errno);
                }
            }
        }

        if (run_state::o_state == run_state::shutdown_e)
        {
            THROW(tcp_socket_timeout_exception, "read timed out on shutdown");
        }

        time_point<steady_clock> time_now = steady_clock::now();

        if ((time_now - start_time) > timeout)
        {   
            THROW(tcp_socket_timeout_exception, "write timed out");
        }
    }
}

const socket_address &tcp_socket::get_remote_address()
{
    return m_remote_address;
}

void tcp_socket::shutdown()
{
    (void)::shutdown(m_fd, SHUT_RDWR);
}
