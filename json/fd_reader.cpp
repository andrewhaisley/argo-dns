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
#include <errno.h>
#include <unistd.h>

#include "fd_reader.hpp"
#include "json_io_exception.hpp"

using namespace std;
using namespace adns;

/// \file fd_reader.cpp The fd_reader class implementation.

fd_reader::fd_reader(int fd, int max_message_length, bool block_read) :
                            reader(max_message_length, block_read),
                            m_fd(fd)
{
}

int fd_reader::read_next_char()
{
    char buf[1];
    ssize_t n = read(m_fd, buf, 1);
    if (n == 0)
    {
        return EOF;
    }
    else if (n == 1)
    {
        return buf[0];
    }
    else if (n == -1)
    {
        throw json_io_exception(json_io_exception::read_failed_e, errno);
    }
    else
    {
        throw json_io_exception(json_io_exception::unexpected_result_from_read_e);
    }
}

bool fd_reader::read_next_block()
{
    ssize_t n = read(m_fd, m_block, block_size);

    if (n == 0)
    {
        return false;
    }
    else if (n == -1)
    {
        throw json_io_exception(json_io_exception::read_failed_e, errno);
    }
    else
    {
        m_block_num_bytes = static_cast<int>(n);
        m_block_index = 0;
        return true;
    }
}
