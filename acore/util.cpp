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
#include <stdarg.h>
#include <string.h>

#include <atomic>

#include "types.hpp"
#include "exception.hpp"
#include "util.hpp"

using namespace std;
using namespace adns;
using namespace util;

static const char *base64_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline uint extract_6_bits(const octet *data, size_t size, size_t &byte, size_t &bit)
{
    // not the most elegant approach, but seems to be the quickest to execute
    if (bit == 0)
    {
        bit = 6;
        return data[byte] >> 2;
    }
    else if (bit == 2)
    {
        bit = 0;
        return data[byte++] & 0x3f;
    }
    else if (bit == 4)
    {
        uint res = (data[byte] & 0x0f) << 2;
        if (byte < (size - 1))
        {
            res |= (data[byte + 1] >> 6);
        }
        
        bit = 2;
        byte++;

        return res;
    }
    else if (bit == 6)
    {
        uint res = (data[byte] & 0x03) << 4;
        if (byte < (size - 1))
        {
            res |= (data[byte + 1] >> 4);
        }
        
        bit = 4;
        byte++;

        return res;
    }
    else
    {
        THROW(util_exception, "base64 encode bug");
    }
}

string util::tobase64(const buffer &binary)
{
    string res;

    res.reserve(((binary.get_size() + 3) / 3) * 4);

    size_t byte = 0;
    size_t bit = 0;
    size_t size = binary.get_size();
    const octet *data = binary.get_data();

    while (byte < size)
    {
        res += base64_map[extract_6_bits(data, size, byte, bit)];
    }

    if ((byte % 3) == 1)
    {
        res += "=";
    }
    else if ((byte % 3) == 2)
    {
        res += "==";
    }

    return res;
}

octet from_base_64(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c - 'A';
    }
    else if (c >= 'a' && c <= 'z')
    {
        return c - 'a' + 26;
    }
    else if (c >= '0' && c <= '9')
    {
        return c - '0' + 52;
    }
    else if (c == '+')
    {
        return 62;
    }
    else if (c == '/')
    {
        return 63;
    }
    else
    {
        THROW(util_exception, "illegal base64 character", c);
    }
}

buffer util::frombase64(const string &base64)
{
    buffer res(((base64.size() * 6) / 8) + 1);

    size_t byte = 0;
    size_t bit = 0;

    octet *data = res.get_data();

    for (char c : base64)
    {
        if (c == '=')
        {
            break;
        }

        octet o = from_base_64(c);

        if (bit == 0)
        {
            data[byte] = (o << 2);
            bit = 6;
        }
        else if (bit == 2)
        {
            data[byte++] |= o;
            bit = 0;
        }
        else if (bit == 4)
        {
            data[byte++] |= o >> 2;
            data[byte] = o << 6;
            bit = 2;
        }
        else if (bit == 6)
        {
            data[byte++] |= o >> 4;
            data[byte] = o << 4;
            bit = 4;
        }
        else
        {
            THROW(util_exception, "base64 decode bug");
        }
    }

    res.set_size(byte);
    return res;
}

char util::tohexchar(octet o)
{
    if (o > 15)
    {
        THROW(util_exception, "tohex bug");
    }

    return o > 9 ? o - 10 + 'A' : o + '0';
}

string util::tohex(const buffer &binary)
{
    string res;

    size_t size = binary.get_size();
    const octet *data = binary.get_data();

    res.reserve(binary.get_size() * 2);

    for (size_t i = 0; i < size; i++)
    {
        res += tohexchar(data[i] >> 4);
        res += tohexchar(data[i] & 0x0f);
    }

    return res;
}

octet util::fromhexchar(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    else
    {
        THROW(util_exception, "illegal hex character", c);
    }
}

buffer util::fromhex(const string &hex)
{
    buffer res(hex.size() / 2);

    size_t byte = 0;
    size_t bit = 0;

    octet *data = res.get_data();
    
    for (char c : hex)
    {
        if (c != ' ')
        {
            if (bit == 0)
            {
                data[byte] = fromhexchar(c) << 4;
                bit = 4;
            }
            else if (bit == 4)
            {
                data[byte++] |= fromhexchar(c);
                bit = 0;
            }
            else
            {
                THROW(util_exception, "fromhex bug");
            }
        }
    }

    return res;
}

string util::fromhttp(const string &s)
{
    string res;

    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == '%')
        {
            if (i < s.size() - 2)
            {
                res += static_cast<char>(fromhexchar(s[i + 1]) * 16 + fromhexchar(s[i + 2]));
                i += 2;
            }
            else
            {
                THROW(util_exception, "badly encoded http string", s);
            }
        }
        else
        {
            res += s[i];
        }
    }

    return res;
}

string util::strerror()
{
    char buf[1000];

    char *s = strerror_r(errno, buf, 1000);
    if (s == nullptr)
    {
        return "<none>";
    }
    else
    {
        return s;
    }
}

const string &util::bool_to_string(bool b)
{
    static string t("true");
    static string f("false");

    return b ? t : f;
}

size_t util::hash_by_byte(const void *data, size_t len)
{
    size_t hash = 5381;

    for (size_t i = 0; i < len; i++)
    {
        hash = ((hash << 5) + hash) + (reinterpret_cast<const octet *>(data))[i];
    }

    return hash;
}
