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

#include "file_reader.hpp"
#include "json_io_exception.hpp"

using namespace std;
using namespace adns;


file_reader::file_reader(FILE *f, int max_message_length, bool block_read) : 
                            reader(max_message_length, block_read), 
                            m_file(f)
{
}

int file_reader::read_next_char()
{
    int c = fgetc(m_file);

    if (c == EOF && ferror(m_file))
    {
        throw json_io_exception(json_io_exception::fgetc_failed_e, errno);
    }
    else
    {
        return c;
    }
}

bool file_reader::read_next_block()
{
    size_t n = fread(m_block, 1, block_size, m_file);

    if (n > 0)
    {
        m_block_num_bytes = static_cast<int>(n);
        m_block_index = 0;
        return true;
    }
    else
    {
        if (ferror(m_file))
        {
            throw json_io_exception(json_io_exception::read_failed_e, errno);
        }
        else
        {
            return false;
        }
    }
}
