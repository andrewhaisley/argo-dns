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

#include "fd_writer.hpp"
#include "json_io_exception.hpp"

using namespace std;
using namespace adns;

fd_writer::fd_writer(int fd) : writer(), m_fd(fd)
{
}

void fd_writer::write(const string &s)
{
    if (::write(m_fd, s.c_str(), s.size()) == -1)
    {
        throw json_io_exception(json_io_exception::write_fd_failed_e, errno);
    }
}
