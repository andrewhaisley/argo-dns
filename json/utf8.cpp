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
#include "utf8.hpp"
#include "json_utf8_exception.hpp"

using namespace std;
using namespace adns;

unsigned int utf8::from_hex(char c)
{
    c = tolower(c);

    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    else
    {
        throw json_utf8_exception(json_utf8_exception::invalid_hex_char_e, c);
    }
}

char utf8::to_hex(unsigned int i)
{
    if (i <= 9)
    {
        return '0' + i;
    }
    else if (i <= 15)
    {
        return 'a' + i - 10;
    }
    else
    {
        throw json_utf8_exception(json_exception::invalid_hex_number_e, i);
    }
}

char utf8::next_char(const string &s, size_t &index)
{
    if (index < s.size())
    {
        return s[index++];
    }
    else
    {
        throw json_utf8_exception(json_utf8_exception::invalid_string_encoding_e, s);
    }
}

char32_t utf8::parse_hex(const string &src, size_t &src_index)
{
    char32_t res = 0;

    res |= from_hex(next_char(src, src_index)) << 12;
    res |= from_hex(next_char(src, src_index)) << 8;
    res |= from_hex(next_char(src, src_index)) << 4;
    res |= from_hex(next_char(src, src_index));

    return res;
}

void utf8::set_and_check_unicode_byte(string &dst, size_t &dst_index, char c)
{
    if (c == 0)
    {
        throw json_utf8_exception(json_utf8_exception::invalid_utf8_char_e, c);
    }
    else
    {
        dst[dst_index++] = c;
    }
}

void utf8::parse_unicode(
                const string &src, 
                size_t       &src_index, 
                string       &dst,
                size_t       &dst_index)
{
    char32_t uc = 0;

    // is this basic multilingual or a UTF-16 surrogate pair?
    if ((src_index + 10) <= src.size() && 
        src[src_index + 4] == '\\' &&
        src[src_index + 5] == 'u')
    {
        char32_t high_surrogate = parse_hex(src, src_index);

        (void)next_char(src, src_index);
        (void)next_char(src, src_index);

        char32_t low_surrogate = parse_hex(src, src_index);

        uc = ((high_surrogate - 0xD800) << 10 | (low_surrogate - 0xDC00)) + 0x10000;
    }
    else
    {
        uc = parse_hex(src, src_index);
    }

    // I would use the nice new C++11 standard conversion templates for this but
    // they're not fully supported by compilers in the wild just yet.

    if (valid_unicode(uc))
    {
        if (uc < 0x80)
        {
            dst[dst_index++] = static_cast<char>(uc);
        }
        else if (uc < 0x800) 
        {
            set_and_check_unicode_byte(dst, dst_index, static_cast<char>((uc >> 6) | 0xc0));
            set_and_check_unicode_byte(dst, dst_index, static_cast<char>((uc & 0x3f) | 0x80));
        }
        else if (uc < 0x10000) 
        {
            set_and_check_unicode_byte(dst, dst_index, static_cast<char>((uc >> 12) | 0xe0));
            set_and_check_unicode_byte(dst, dst_index, static_cast<char>(((uc >> 6) & 0x3f) | 0x80));
            set_and_check_unicode_byte(dst, dst_index, static_cast<char>((uc & 0x3f) | 0x80));
        }
        else 
        {
            set_and_check_unicode_byte(dst, dst_index, static_cast<char>((uc >> 18) | 0xf0));
            set_and_check_unicode_byte(dst, dst_index, static_cast<char>(((uc >> 12) & 0x3f) | 0x80));
            set_and_check_unicode_byte(dst, dst_index, static_cast<char>(((uc >> 6) & 0x3f) | 0x80));
            set_and_check_unicode_byte(dst, dst_index, static_cast<char>((uc & 0x3f) | 0x80));
        }
    }
    else
    {
        throw json_utf8_exception(json_utf8_exception::invalid_unicode_e, uc);
    }
}

unique_ptr<string> utf8::json_string_to_utf8(const string &src)
{
    size_t src_index = 0;
    size_t dst_index = 0;

    string *dst = new string(src.size(), '\0');

    unique_ptr<string> res(dst);

    while (src_index < src.size())
    {
        if (src[src_index] != '\\')
        {
            (*dst)[dst_index++] = src[src_index++];
        }
        else
        {
            char c = next_char(src, ++src_index);

            switch (c)
            {
            case '"':
            case '\\':
            case '/':
                (*dst)[dst_index++] = c;
                break;
            case 'b':
                (*dst)[dst_index++] = '\b';
                break;
            case 'f':
                (*dst)[dst_index++] = '\f';
                break;
            case 'n':
                (*dst)[dst_index++] = '\n';
                break;
            case 'r':
                (*dst)[dst_index++] = '\r';
                break;
            case 't':
                (*dst)[dst_index++] = '\t';
                break;
            case 'u':
                parse_unicode(src, src_index, *dst, dst_index);
                break;
            default: 
                throw json_utf8_exception(json_utf8_exception::invalid_string_escape_e, c);
            }
        }
    }

    dst->resize(dst_index);
    return res;
}

unsigned int utf8::utf8_length(uint8_t c)
{
    if ((c & 0x80) == 0)
    {
        return 1;
    }
    else if ((c & 0xe0) == 0xc0)
    {
        return 2;
    }
    else if ((c & 0xf0) == 0xe0)
    {
        return 3;
    }
    else if ((c & 0xf8) == 0xf0)
    {
        return 4;
    }
    else
    {
       return 0;
    }
}

unique_ptr<string> utf8::utf8_to_json_string(const string &src)
{
    unique_ptr<string> res(new string);

    // We need at least the same number of characters as the source.
    res->reserve(src.size());

    size_t src_index = 0;

    while (src_index < src.size())
    {
        char32_t uc = src[src_index++] & 0xff;

        unsigned int l = utf8_length(uc);

        switch (l)
        {
        case 1:
            break;
        case 2:
            uc = ((uc << 6) & 0x7ff) | (next_char(src, src_index) & 0x3f);
            break;
        case 3:
            uc = ((uc << 12) & 0xffff) | ((next_char(src, src_index) << 6) & 0xfff);
            uc |= next_char(src, src_index) & 0x3f;
            break;
        case 4:
            uc = ((uc << 18) & 0x1fffff) | ((next_char(src, src_index) << 12) & 0x3ffff);
            uc |= (next_char(src, src_index) << 6) & 0xfff;
            uc |= next_char(src, src_index) & 0x3f;  
            break;
        default:
            throw json_utf8_exception(json_utf8_exception::invalid_utf8_sequence_length_e, l);
        }

        if (uc <= 0xff)
        {
            switch (uc)
            {
            case '"':
                *res += "\\\"";
                break;
            case '\\':
                *res += "\\\\";
                break;
            case '\b':
                *res += "\\b";
                break;
            case '\f':
                *res += "\\f";
                break;
            case '\n':
                *res += "\\n";
                break;
            case '\r':
                *res += "\\r";
                break;
            case '\t':
                *res += "\\t";
                break;
            default: 
                *res += static_cast<char>(uc);
            }
        }
        else if (uc <= 0xffff)
        {
            add_hex_string(res, uc);
        }
        else
        {
            uc -= 0x10000;
            add_hex_string(res, (uc >> 10) + 0xD800);
            add_hex_string(res, (uc & 0x3ff) + 0xDC00);
        }
    }

    return res;
}

bool utf8::valid_unicode(char32_t uc)
{
    return (uc <= 0x0010ffffu) && !(uc >= 0xd800u && uc <= 0xdfffu);
}

void utf8::add_hex_string(unique_ptr<string> &dst, char32_t uc)
{
    *dst += "\\u";
    *dst += to_hex((uc >> 12) & 0x0f);
    *dst += to_hex((uc >> 8) & 0x0f);
    *dst += to_hex((uc >> 4) & 0x0f);
    *dst += to_hex(uc & 0x0f);
}
