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
#include <ostream>

#include "json_exception.hpp"

using namespace std;
using namespace adns;

json_exception::json_exception(exception_type et) noexcept : exception(), m_type(et)
{
    switch (et)
    {
    case invalid_json_type_e:
        strncpy(m_message, "internal error, invalid json instance type", max_message_length - 1);
        break;
    case not_number_e:
        strncpy(m_message, "instance is not a number", max_message_length - 1);
        break;
    case cant_compare_raw_e:
        strncpy(m_message, "raw values can't be compared", max_message_length - 1);
        break;
    case cant_cast_raw_e:
        strncpy(m_message, "raw values can't be cast", max_message_length - 1);
        break;
    case pointer_not_matched_e:
        strncpy(m_message, "pointer doesn't match a location in the instance", max_message_length - 1);
        break;
    case pointer_token_type_invalid_e:
        strncpy(m_message, "pointer token type is invalid", max_message_length - 1);
        break;
    default:
        strncpy(m_message, "generic", max_message_length - 1);
        break;
    }
}

json_exception::json_exception(exception_type et, const char *json_type_name) noexcept : exception(), m_type(et)
{
    switch (et)
    {
    case not_an_array_e:
        snprintf(m_message, max_message_length, "instance type is %s not array", json_type_name);
        break;
    case not_an_object_e:
        snprintf(m_message, max_message_length, "instance type is %s not object", json_type_name);
        break;
    case not_number_int_or_boolean_e:
        snprintf(m_message, max_message_length, "instance type is %s not integer number or boolean", json_type_name);
        break;
    case not_string_e:
        snprintf(m_message, max_message_length, "instance type is %s not string", json_type_name);
        break;
    case not_number_or_string_e:
        snprintf(m_message, max_message_length, "attempt to set a raw value of an instance of type %s (only works for number and string)", json_type_name);
        break;
    default:
        strncpy(m_message, "generic", max_message_length - 1);
        break;
    }
}

json_exception::json_exception(
                    exception_type et, 
                    const char     *first_json_type_name,
                    const char     *second_json_type_name) noexcept : exception(), m_type(et)
{
    if (et == types_not_comparable_e)
    {
        snprintf(m_message, max_message_length, "attempt to compare %s with %s", first_json_type_name, second_json_type_name);
    }
    else
    {
        strncpy(m_message, "generic", max_message_length - 1);
    }
}

const char *json_exception::what() const noexcept
{
    return m_message;
}

json_exception::exception_type json_exception::get_type() const noexcept
{
    return m_type;
}

ostream &adns::operator<<(ostream& stream, const adns::json_exception &e)
{
    return stream << e.what();
}
