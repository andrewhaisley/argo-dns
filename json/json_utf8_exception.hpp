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
    /// Exception class for errors translating to and from UTF-8 strings.
    class json_utf8_exception : public json_exception
    {
    public:

        /**
         * Constructor
         * \param et Type of the exeption.
         * \param s  Extra infomation to add to the message.
         */
        json_utf8_exception(exception_type et, const std::string &s) noexcept;

        /**
         * Constructor
         * \param et Type of the exeption.
         * \param c  Extra infomation to add to the message.
         */
        json_utf8_exception(exception_type et, char c) noexcept;

        /**
         * Constructor
         * \param et Type of the exeption.
         * \param i  Extra infomation to add to the message.
         */
        json_utf8_exception(exception_type et, unsigned int i) noexcept;

        /// Add the byte index where the error occured to the exception.
        void add_byte_index(size_t byte_index) noexcept;

        /// Get the (approximate) byte index at which the error occurred.
        size_t get_byte_index() const noexcept;

        /// Get the (approximate) column at which the error occurred.
        size_t get_column() const noexcept;

    private:

        const char *get_main_message();

        size_t m_byte_index;
    };
}
