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

#include <istream>

#include "types.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(buffer_exception, exception)

namespace adns
{
    /**
     * Blocks of memory with an associated size. Copying does not copy the underlying data, just
     * the pointer to it.
     */
    class buffer
    {
    public:

        /**
         * Empty block.
         */
        buffer();

        /**
         * New block from raw data - a copy is made
         */
        buffer(size_t size, const octet *data);

        /**
         * New block from a string - a copy is made
         */
        buffer(const std::string &s);

        /**
         * New block from the contents of a stream
         */
        buffer(std::istream &is);

        /**
         * New block of given size.
         */
        buffer(size_t size);

        /**
         * Doesn't free the block of memory unless this is the last reference.
         */
        virtual ~buffer();

        /**
         * Set the contents from a memory pointer and length
         */
        void set_data(size_t size, const octet *data);

        /** 
         * Append another buffer to this one.
         */
        void append(const buffer &b);

        /**
         * Pointer to the data block
         */
        octet *get_data() const;

        /**
         * Size of the block
         */
        size_t get_size() const;

        /**
         * Sets the size - note, doesn't reallocate or deallocate anything.
         */
        void set_size(size_t s);

        /**
         * gets an ascii string representation if possible. Any non ascii characters cause an
         * exception to be thrown.
         */
        std::string to_ascii_string() const;

        /**
         * gets a hex string representation - mainly for debug
         */
        std::string to_string() const;

        /**
         * dump out for debug
         */
        void dump() const;

    private:

        size_t m_size;

        std::shared_ptr<octet[]> m_data;
    };

    std::ostream &operator<<(std::ostream& stream, const buffer &b);
}
