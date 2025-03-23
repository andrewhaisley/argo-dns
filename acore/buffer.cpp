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
#include <sstream>
#include <iomanip>

#include "types.hpp"
#include "buffer.hpp"
#include "util.hpp"

using namespace std;
using namespace adns;

buffer::buffer() : m_size(0)
{
}

buffer::~buffer()
{
}

buffer::buffer(size_t size, const octet *data) : m_size(size), m_data(new octet[size])
{
    memcpy(m_data.get(), data, size);
}

buffer::buffer(const string &s) : m_size(s.size()), m_data(new octet[m_size])
{
    memcpy(m_data.get(), s.c_str(), m_size);
}

buffer::buffer(istream &is)
{
    is.seekg(0, is.end);
    m_size = is.tellg();
    is.seekg(0, is.beg);

    m_data.reset(new octet[m_size]);

    is.read((char *)(m_data.get()), m_size);

    if (!is)
    {
        THROW(buffer_exception, "file read failed");
    }
}

void buffer::set_data(size_t size, const octet *data)
{
    octet *nd = new octet[size];
    memcpy(nd, data, size);
    m_size = size;
    m_data.reset(nd);
}

buffer::buffer(size_t size) : m_size(size), m_data(new octet[m_size])
{
}

void buffer::append(const buffer &b)
{
    octet *nd = new octet[m_size + b.m_size];
    memcpy(nd, m_data.get(), m_size);
    memcpy(nd + m_size, b.m_data.get(), b.m_size);
    m_size = m_size + b.m_size;
    m_data.reset(nd);
}

octet *buffer::get_data() const
{
    return m_data.get();
}

size_t buffer::get_size() const
{
    return m_size;
}

void buffer::set_size(size_t s)
{
    m_size = s;
}

string buffer::to_string() const
{
    string res;

    for (size_t i = 0; i < m_size; i++)
    {
        char buffer[4];
        sprintf(buffer, "%02x ", m_data.get()[i]);
        res += string(buffer);
    }

    return res;
}

void buffer::dump() const
{
    char location[100];

    for (size_t i = 0; i < m_size; i+= 8)
    {
        sprintf(location, "%05lu ", i);
        string line;
        
        for (size_t j = 0; (j < 8) && (i + j) < m_size; j++)
        {
            char buffer[4];
            sprintf(buffer, "%02x ", m_data[i + j]);
            line += buffer;
        }

        line += "    ";

        for (size_t j = 0; (j < 8) && (i + j) < m_size; j++)
        {
            if (isprint(m_data[i + j]))
            {
                line += m_data[i + j];
            }
            else
            {
                line += ".";
            }
        }

        LOG(debug) << location << " " << line;
    }
}

string buffer::to_ascii_string() const
{
    string res;

    for (size_t i = 0; i < m_size; i++)
    {
        if (isascii(m_data.get()[i]))
        {
            res += static_cast<char>(m_data.get()[i]);
        }
        else
        {
            res += '.';
        }
    }

    return res;
}

ostream &adns::operator<<(ostream& stream, const buffer &b)
{
    stream << b.to_string();
    return stream;
}
