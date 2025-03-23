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

#include <string>

#include "types.hpp"
#include "exception.hpp"
#include "buffer.hpp"

EXCEPTION_CLASS(util_exception, exception)

namespace adns
{
    /**
     * various utility functions
     */
    namespace util
    {
        /**
         * binary to base64 string
         */
        std::string tobase64(const buffer &binary);

        /**
         * base64 string to binary
         */
        buffer frombase64(const std::string &base64);

        /**
         * binary to hex string
         */
        std::string tohex(const buffer &binary);

        /**
         * hex string to binary
         */
        buffer fromhex(const std::string &hex);

        /**
         * from http encoded (e.g. %20=' ') to a normal string
         */
        std::string fromhttp(const std::string &s);

        /**
         * get the current error string
         */
        std::string strerror();

        /**
         * string version of true/false
         */
        const std::string &bool_to_string(bool b);

        /**
         * create a hash value from some binary data
         */
        size_t hash_by_byte(const void *data, size_t len);

        /**
         * turn a single hex character into a number (0-15)
         */
        octet fromhexchar(char c);

        /**
         * turn a single character into a hex char (0-f)
         */
        char tohexchar(octet o);

        /** 
         * is a path from a URL safe. Safe means it's not an attempt to     
         * access something outside of the normal local directory
         * hierachy.
         */
        bool path_is_safe(std::string &path);

        /**
         * guess the mime type of a given file. Uses file extensions
         * but could be extended to look at contents if needed.
         */
        std::string mime_type(const std::string &path);
    }
}
