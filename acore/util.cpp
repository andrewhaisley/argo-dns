//
//  Copyright 2025 Andrew Haisley
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
//  associated documentation files (the “Software”), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial 
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
//  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
 
#include <stdarg.h>
#include <string.h>

#include <boost/algorithm/string.hpp>

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

bool util::path_is_safe(std::string &path)
{
    // does the path contain anything other than the following characters?
    //  a-z
    //  0-9
    //  A-Z
    //  /
    //  -
    //  _
    //  .
    // are there any consecutive '.' characters? e.g. a/../../.. etc
    // are there any consecutive '/' characters?

    // this should be quicker than a regex
    for (size_t i = 0; i < path.size(); i++)
    {
        char c1 = path[i];

        if (!((c1 >= 'a' && c1 <= 'z') ||
            (c1 >= 'A' && c1 <= 'Z') ||
            (c1 >= '0' && c1 <= '9') ||
            (c1 == '/') ||
            (c1 == '-') ||
            (c1 == '_') ||
            (c1 == '.')))
        {
            return false;
        }

        if (i < (path.size() - 1))
        {
            char c2 = path[i + 1];
            if ((c1 == c2) && ((c1 == '/') || (c1 == '.')))
            {
                return false;
            }
        }
    }

    return true;
}

string util::mime_type(const string &path)
{
    // types handled:
    // image/png
    // text/css
    // text/html
    // text/javascript
    // text/json
    // image/x-icon
    // application/manifest+json
    auto i = path.rfind('.');

    if (i == string::npos || (i == path.size() - 1))
    {
        THROW(util_exception, "mime type cannot be determined", path);
    }
    
    string ext = path.substr(i + 1);

    if (ext == "html")
    {
        return "text/html";
    }
    else if (ext == "png")
    {
        return "image/png";
    }
    else if (ext == "ico")
    {
        return "image/x-icon";
    }
    else if (ext == "css")
    {
        return "text/css";
    }
    else if (ext == "js" || ext == "javascript")
    {
        return "text/javascript";
    }
    else if (ext == "json")
    {
        return "text/json";
    }
    else if (ext == "manifest")
    {
        return "application/manifest+json";
    }
    else
    {
        THROW(util_exception, "mime type cannot be determined", path);
    }
}
