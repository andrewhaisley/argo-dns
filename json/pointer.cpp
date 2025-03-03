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
#include <sstream>

#include "pointer.hpp"
#include "utf8.hpp"
#include "json_pointer_exception.hpp"

using namespace adns;
using namespace std;

pointer::pointer(const std::string &pointer)
{
    // is this a URI fragment type pointer?
    if (pointer[0] == '#')
    {
        build_from_uri_fragment(*utf8::json_string_to_utf8(pointer));
    }
    else
    {
        build_from_json_string(*utf8::json_string_to_utf8(pointer));
    }
}

void pointer::build_from_uri_fragment(const std::string &pointer)
{
    size_t start = 1;
    size_t end = 0;

    if (pointer.size() == 1)
    {
        m_path.push_back(token());
    }
    else
    {
        while (next_token(pointer, start, end))
        {
            m_path.push_back(translate_uri_token(pointer, start, end));
        }
    }
}

void pointer::build_from_json_string(const std::string &pointer)
{
    size_t start = 0;
    size_t end = 0;

    if (pointer.size() == 0)
    {
        m_path.push_back(token());
    }
    else
    {
        while (next_token(pointer, start, end))
        {
            m_path.push_back(translate_jsonp_token(pointer, start, end));
        }
    }
}

bool pointer::next_token(const string &pointer, size_t &start, size_t &end)
{
    if (start >= pointer.size())
    {
        return false;
    }
    else
    {
        end = start + 1;

        while (end < pointer.size())
        {
            if (pointer[end] == '/')
            {
                break;
            }
            else
            {
                end++;
            }
        }

        return true;
    }
}

pointer::token pointer::translate_uri_token(const string &pointer, size_t &start, size_t &end)
{
    string translated;

    if (pointer[start++] == '/')
    {
        while (start < end)
        {
            if (pointer[start] == '~')
            {
                if (++start < end)
                {
                    if (pointer[start] == '0')
                    {
                        translated += '~';
                    }
                    else if (pointer[start] == '1')
                    {
                        translated += '/';
                    }
                    else
                    {
                        throw json_pointer_exception(json_exception::syntax_error_in_pointer_string_e, start - 1);
                    }
                    start++;
                }
                else
                {
                    throw json_pointer_exception(json_exception::syntax_error_in_pointer_string_e, start - 1);
                }
            }
            else if (pointer[start] == '%')
            {
                if (++start < (end - 1))
                {
                    int c = from_hex(pointer, start++) * 16;
                    c += from_hex(pointer, start++);
                    translated += static_cast<char>(c);
                }
                else
                {
                    throw json_pointer_exception(json_exception::syntax_error_in_pointer_string_e, start - 1);
                }
            }
            else
            {
                translated += pointer[start++];
            }
        }

        return make_token(translated);
    }
    else
    {
        throw json_pointer_exception(json_exception::syntax_error_in_pointer_string_e, start - 1);
    }
}

pointer::token pointer::translate_jsonp_token(const string &pointer, size_t &start, size_t &end)
{
    string translated;

    if (pointer[start++] == '/')
    {
        while (start < end)
        {
            if (pointer[start] == '~')
            {
                if (++start < end)
                {
                    if (pointer[start] == '0')
                    {
                        translated += '~';
                    }
                    else if (pointer[start] == '1')
                    {
                        translated += '/';
                    }
                    else
                    {
                        throw json_pointer_exception(json_exception::syntax_error_in_pointer_string_e, start - 1);
                    }
                    start++;
                }
                else
                {
                    throw json_pointer_exception(json_exception::syntax_error_in_pointer_string_e, start - 1);
                }
            }
            else
            {
                translated += pointer[start++];
            }
        }

        return make_token(translated);
    }
    else
    {
        throw json_pointer_exception(json_exception::syntax_error_in_pointer_string_e, start - 1);
    }
}

pointer::token pointer::make_token(const string &s)
{
    istringstream is(s);

    size_t i;

    if (is >> i)
    {
        return token(i);
    }
    else
    {
        return token(s);
    }
}

int pointer::from_hex(const string &s, size_t index)
{
    char c = s[index];

    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    else
    {
        throw json_pointer_exception(json_exception::syntax_error_in_pointer_string_e, index - 1);
    }
}

pointer::token::token() : m_type(all_e), m_index(0)
{
}

pointer::token::token(const string &name) : m_type(object_e), m_name(name), m_index(0)
{
}

pointer::token::token(size_t index) : m_type(array_e), m_index(index)
{
}

const list<pointer::token> &pointer::get_path() const
{
    return m_path;
}

pointer::token::type_t pointer::token::get_type() const
{
    return m_type;
}

const string &pointer::token::get_name() const
{
    return m_name;
}

size_t pointer::token::get_index() const
{
    return m_index;
}

ostream &adns::operator<<(ostream &stream, const pointer &p)
{
    for (auto &t : p.get_path())
    {
        stream << "/";
        if (t.get_type() == pointer::token::object_e)
        {
            stream << t.get_name();
        }
        else if (t.get_type() == pointer::token::array_e)
        {
            stream << t.get_index();
        }
    }

    return stream;
}
