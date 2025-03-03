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
#include <string>

#include "json_parser_exception.hpp"

using namespace std;
using namespace adns;

const char *json_parser_exception::get_main_message()
{
    switch (m_type)
    {
    case invalid_character_e:
        return "invalid character";
    case invalid_number_e:
        return "invalid number format";
    case number_out_of_range_e:
        return "number out of range";
    case number_too_long_e:
        return "number too long";
    case string_too_long_e:
        return "string too long";
    case unexpected_eof_e:
        return "unexpected end of file";
    case unexpected_token_e:
        return "unexpected token";
    default:
        return "generic";
    }

}

json_parser_exception::json_parser_exception(exception_type et, size_t byte_index) noexcept:
                    json_exception(et),
                    m_byte_index(byte_index)
{
    snprintf(m_message, max_message_length, "parser exception, %s, at or near byte %lu", get_main_message(), m_byte_index);
}

json_parser_exception::json_parser_exception(exception_type et, char c, size_t byte_index) noexcept:
                    json_exception(et),
                    m_byte_index(byte_index)
{
    snprintf(m_message, max_message_length, "parser exception, %s, at or near byte %lu : 0x%02x", get_main_message(), m_byte_index, c);
}
 
json_parser_exception::json_parser_exception(exception_type et, const string &s, size_t byte_index) noexcept:
                    json_exception(et),
                    m_byte_index(byte_index)
{
    snprintf(m_message, max_message_length, "parser exception, %s, at or near byte %lu : %s", get_main_message(), m_byte_index, s.c_str());
}

json_parser_exception::json_parser_exception(exception_type et, size_t st, size_t byte_index) noexcept:
                    json_exception(et),
                    m_byte_index(byte_index)
{
    snprintf(m_message, max_message_length, "parser exception, %s, at or near byte %lu : %ld", get_main_message(), m_byte_index, st);
}

size_t json_parser_exception::get_byte_index() const noexcept
{
    return m_byte_index;
}
