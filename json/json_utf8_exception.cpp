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

#include "json_utf8_exception.hpp"

using namespace std;
using namespace adns;

json_utf8_exception::json_utf8_exception(exception_type et, const string &s) noexcept :
                    json_exception(et), m_byte_index(0)
{
    snprintf(m_message, max_message_length, "%s : %s", get_main_message(), s.c_str());
}

json_utf8_exception::json_utf8_exception(exception_type et, char c) noexcept :
                    json_exception(et), m_byte_index(0)
{
    snprintf(m_message, max_message_length, "%s : 0x%02x", get_main_message(), c);
}

json_utf8_exception::json_utf8_exception(exception_type et, unsigned int i) noexcept :
                    json_exception(et), m_byte_index(0)
{
    snprintf(m_message, max_message_length, "%s : %d", get_main_message(), i);
}

void json_utf8_exception::add_byte_index(size_t byte_index) noexcept
{
    snprintf(m_message, max_message_length, "%s at or near byte %ld", get_main_message(), byte_index);
}

size_t json_utf8_exception::get_byte_index() const noexcept
{
    return m_byte_index;
}

const char *json_utf8_exception::get_main_message()
{
    switch (m_type)
    {
    case invalid_string_escape_e:
        return "invalid escape character found in string";
    case invalid_string_encoding_e:
        return "string encoding is invalid";
    case invalid_unicode_e:
        return "invalid unicode value";
    case invalid_utf8_sequence_length_e:
        return "invalid UTF-8 sequence length";
    case invalid_hex_char_e:
        return "invalid hex character";
    case invalid_hex_number_e:
        return "invalid hex number (must be 0-15)";
    case invalid_utf8_char_e:
        return "0 byte detected in UTF-8 sequence other than code point 0";
    default:
        return "generic";
    }
}
