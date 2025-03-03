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
#include <ctype.h>
#include <memory>
#include <mutex>

#include "types.hpp"
#include "dns_label.hpp"

using namespace std;
using namespace adns;

dns_label dns_label::wildcard("*");

dns_label::dns_label() : m_size(0)
{
}

dns_label::~dns_label()
{
}

dns_label::dns_label(const octet *s, octet size)
{
    if (size > MAX_LABEL_LENGTH)
    {
        THROW(label_exception, "attempt to construct a label of more than the maximum allowed size", size);
    }

    memcpy(m_data, s, size);
    m_size = size;
}

dns_label::dns_label(const string &s)
{
    if (s.length() > MAX_LABEL_LENGTH)
    {
        THROW(label_exception, "attempt to construct a label of more than the maximum allowed size", s.length());
    }

    memcpy(m_data, s.c_str(), s.length());
    m_size = s.length();
}

bool dns_label::operator==(const dns_label &other) const
{
    if (m_size != other.m_size)
    {
        return false;
    }

    for (size_t i = 0; i < m_size; i++)
    {
        if (tolower(m_data[i]) != tolower(other.m_data[i]))
        {
            return false;
        }
    }

    return true;
}

bool dns_label::operator!=(const dns_label &other) const
{
    return !(*this == other);
}

/**
 * byte by byte comparison
 */
bool dns_label::operator<(const dns_label &other) const
{
    size_t s;

    if (m_size < other.m_size)
    {
        s = m_size;
    }
    else
    {
        s = other.m_size;
    }

    for (size_t i = 0; i < s; i++)
    {
        if (tolower(m_data[i]) < tolower(other.m_data[i]))
        {
            return true;
        }
        if (tolower(m_data[i]) > tolower(other.m_data[i]))
        {
            return false;
        }
    }

    return false;
}

string dns_label::to_string() const
{
    string s;

    s.reserve(m_size);
    for (size_t i = 0; i < m_size; i++)
    {
        if (isprint(m_data[i]))
        {
            s += m_data[i];
        }
        else    
        {
            s += '.';
        }
    }

    return s;
}

size_t dns_label::get_hash() const
{
    return hash<string>()(to_string());

    size_t res = 0;
    octet *s = reinterpret_cast<octet *>(&res);

    for (octet i = 0; i < m_size; i++)
    {
        *s |= m_data[i];
    }

    return res;

}

octet dns_label::get_length() const
{
    return m_size;
}

ostream &adns::operator<<(ostream& stream, const dns_label &l)
{
    stream << l.to_string();
    return stream;
}

