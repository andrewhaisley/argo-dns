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
#include <stdio.h>

#include "json_io_exception.hpp"

using namespace std;
using namespace adns;

const char *json_io_exception::get_main_message()
{
    switch (m_type)
    {
    case fgetc_failed_e:
        return "fgetc() failed";
    case read_failed_e:
        return "read() failed";
    case unexpected_result_from_read_e:
        return "unexpected result from read()";
    case write_fd_failed_e:
        return "write() failed";
    case write_file_failed_e:
        return "fwrite() failed";
    case too_many_put_back_e:
        return "internal error, too many characters put back into a reader";
    case message_too_long_e:
        return "maximum configured message length exceeded";
    case file_open_failed_e:
        return "file open failed";
    default:
        return "generic";
    }
}

json_io_exception::json_io_exception(exception_type et) noexcept : json_exception(et)
{
    strncpy(m_message, get_main_message(), max_message_length - 1);
}

json_io_exception::json_io_exception(exception_type et, int posix_errno) noexcept : json_exception(et)
{
    char buffer[max_message_length];

    snprintf(m_message, max_message_length, "%s : %s", get_main_message(), strerror_r(posix_errno, buffer, max_message_length));
}

json_io_exception::json_io_exception(exception_type et, size_t s) noexcept : json_exception(et)
{
    snprintf(m_message, max_message_length, "%s : %ld", get_main_message(), s);
}
