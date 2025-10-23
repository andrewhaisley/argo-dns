//
//  Copyright 2025 Andrew Haisley
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
//  associated documentation files (the “Software”), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial 
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
//  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
