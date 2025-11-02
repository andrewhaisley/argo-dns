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
#include <boost/beast/core/detail/base64.hpp>

#include "types.hpp"
#include "exception.hpp"
#include "util.hpp"

using namespace std;
using namespace adns;
using namespace util;
using namespace boost::beast::detail::base64;

string util::tobase64(const buffer &binary)
{
    char dest[encoded_size(binary.get_size())];
    auto len = encode(dest, binary.get_data(), binary.get_size());
    return string(dest, len);
}

buffer util::frombase64(const string &base64)
{
    octet dest[decoded_size(base64.size())];
    auto len = decode(dest, base64.c_str(), base64.size()).first;
    return buffer(len, dest);
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
