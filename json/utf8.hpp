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

#include <memory>
#include <string>

namespace adns
{
    /**
     * \brief Methods to convert JSON format string to UTF-8 and vice-versa. 
     *
     * This only really exists because the C++ standard libraries for this are
     * not widely implemented yet. 
     */
    class utf8
    {
    public:

        /**
         * Create a UTF-8 encoded string from a JSON string. Allocated dynamically
         * to reduce copying.
         * \param src   A string in JSON format. E.g. "jkhjkh\uAABB\t".
         */
        static std::unique_ptr<std::string> json_string_to_utf8(const std::string &src);

        /**
         * Create a JSON encoded string from a UTF-8 string. Allocated dynamically
         * to reduce copying.
         * \param src   A UTF-8 string.
         */
        static std::unique_ptr<std::string> utf8_to_json_string(const std::string &src);

    private:

        static char next_char(const std::string &s, size_t &index);

        static unsigned int from_hex(char c);

        static char to_hex(unsigned int i);

        static void set_and_check_unicode_byte(std::string &dst, size_t &dst_index, char c);

        static void parse_unicode(
                const std::string &src, 
                size_t            &src_index, 
                std::string       &dst,
                size_t            &dst_index);

        static char32_t parse_hex(
                const std::string &src, 
                size_t            &src_index);

        static unsigned int utf8_length(uint8_t c);

        static bool valid_unicode(char32_t uc);

        static void add_hex_string(std::unique_ptr<std::string> &dst, char32_t uc);
    };
}
