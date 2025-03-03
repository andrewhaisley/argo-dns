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

#include "json_pointer_exception.hpp"

using namespace std;
using namespace adns;

json_pointer_exception::json_pointer_exception(exception_type et, int byte_index) noexcept : json_exception(et)
{
    if (et == syntax_error_in_pointer_string_e)
    {
        snprintf(m_message, max_message_length, "syntax error in pointer string at byte %d", byte_index);
    }
    else
    {
        strncpy(m_message, "generic", max_message_length - 1);
    }
}
