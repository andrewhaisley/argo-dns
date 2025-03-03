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
    /// Exception class for parser errors.
    class json_parser_exception : public json_exception
    {
    public:

        /**
         * Constructor.
         * \param et         Type of the exeption.
         * \param byte_index Place in the input the error occurred.
         */
        json_parser_exception(exception_type et, size_t byte_index) noexcept;

        /**
         * Constructor for a character based parser exception.
         * \param et         Type of the exeption.
         * \param c          The character that caused the exception.
         * \param byte_index Place in the input the error occurred.
         */
        json_parser_exception(exception_type et, char c, size_t byte_index) noexcept;

        /**
         * Constructor for a parser exception where a string needs to be reported back 
         * (e.g. unexpected token).
         * \param et         Type of the exeption.
         * \param s          The string related to the exception.
         * \param byte_index Place in the input the error occurred.
         */
        json_parser_exception(exception_type et, const std::string &s, size_t byte_index) noexcept;

        /**
         * Constructor for a parser exception where a size_t needs to be reported back
         * (e.g. too many characters in a string).
         * \param et         Type of the exeption.
         * \param s          The size_t related to the exception.
         * \param byte_index Place in the input the error occurred.
         */
        json_parser_exception(exception_type et, size_t s, size_t byte_index) noexcept;

        /// Get the (approximate) byte index at which the error occurred.
        size_t get_byte_index() const noexcept;

    private:

        const char *get_main_message();

        size_t m_byte_index;
    };
}
