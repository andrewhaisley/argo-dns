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

#include "types.hpp"

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
