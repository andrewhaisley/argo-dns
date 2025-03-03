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
#include <string>

#include "json_invalid_key_exception.hpp"

using namespace std;
using namespace adns;

json_invalid_key_exception::json_invalid_key_exception(exception_type et, const string &key) noexcept : json_exception(et)
{
    if (et == invalid_key_e)
    {
        strncpy(m_message, "invalid object key: ", max_message_length - 1);
        strncpy(m_message, key.c_str(), max_message_length - 1);
    }
    else
    {
        strncpy(m_message, "generic", max_message_length - 1);
    }
}
