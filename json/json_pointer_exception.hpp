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
#pragma once

#include "json_exception.hpp"

namespace adns
{
    /// Specific class of exceptions for IO errors of various types.
    class json_pointer_exception : public json_exception
    {
    public:

        /// Constructor with a byte index where a syntax error occurred.
        json_pointer_exception(exception_type et, int byte_index) noexcept;
    };
}
