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
    class json_io_exception : public json_exception
    {
    public:

        /// Constructor for generic error.
        json_io_exception(exception_type et) noexcept;

        /// Constructor with a POSIX errno.
        json_io_exception(exception_type et, int posix_errno) noexcept;

        /// Constructor with a size_t
        json_io_exception(exception_type et, size_t s) noexcept;

    private:

        /// Get the main part of the message based on the exception type
        const char *get_main_message();

    };
}
