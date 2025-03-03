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
#include "reader.hpp"
#include "json_io_exception.hpp"

using namespace adns;

reader::reader(size_t max_message_length, bool block_read) : 
            m_put_back(reader::no_put_back), 
            m_byte_index(0),
            m_max_message_length(max_message_length),
            m_block_read(block_read),
            m_block_num_bytes(0),
            m_block_index(-1)
{
}

int reader::read_next_char_from_block()
{
    if (m_block_index == -1 || m_block_index >= m_block_num_bytes)
    {
        if (!read_next_block())
        {
            return EOF;
        }
    }

    return m_block[m_block_index++];
}

int reader::next()
{
    if (m_put_back == no_put_back)
    {
        int res = m_block_read ?  read_next_char_from_block() : read_next_char();

        if (++m_byte_index > m_max_message_length)
        {
            throw json_io_exception(json_exception::message_too_long_e, m_max_message_length);
        }

        return res;
    }
    else
    {
        int c = m_put_back;
        m_put_back = no_put_back;
        return c;
    }
}

void reader::put_back(int c)
{
    if (m_put_back == no_put_back)
    {
        m_put_back = c;
    }
    else
    {
        throw json_io_exception(json_io_exception::too_many_put_back_e);
    }
}

size_t reader::get_byte_index() const
{
    return m_byte_index;
}

void reader::reset_byte_index()
{
    m_byte_index = 0;
}
