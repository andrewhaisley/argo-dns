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
#include "stream_reader.hpp"
#include "json_io_exception.hpp"

using namespace std;
using namespace adns;


stream_reader::stream_reader(istream *s, int max_message_length, bool block_read) : 
                                reader(max_message_length, block_read), 
                                m_stream(s)
{
}

int stream_reader::read_next_char()
{
    return m_stream->get();
}

bool stream_reader::read_next_block()
{
    m_stream->read(reinterpret_cast<char *>(m_block), block_size);

    streamsize n = m_stream->gcount();

    if (n > 0)
    {
        m_block_num_bytes = static_cast<int>(n);
        m_block_index = 0;
        return true;
    }
    else
    {
        if (m_stream->eof())
        {
            return false;
        }
        else
        {
            throw json_io_exception(json_io_exception::read_failed_e, errno);
        }
    }
}
