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

namespace adns
{
    /**
     * \brief A class to read JSON messages from various types of input stream.
     *
     * A class that acts as a proxy for the various places that JSON messages
     * can be parsed from (or from which they can be parsed even). The base
     * class tracks how many characters have been read and handles put back logic,
     * actually reading from somewhere is in in derived classes.
     */
    class reader
    {
    public:

        /// Amount of data to read at once when in block read mode.
        static const int block_size = 8192;

        /**
         * Constructor.
         * \param   max_message_length  maximum number bytes to be read in total
         * \param   block_read          if true read in blocks, if false read one
         *                              byte at a time
         */
        reader(size_t max_message_length, bool block_read);

        /// Destructor to shut gcc up.
        virtual ~reader() {};

        /**
         * \brief Get the next character from the reader. 
         * \return  A character or EOF on end of file.
         * \throw   json_io_exception after an IO error of some type.
         * \throw   json_parser_exception if the message is too long
         */
        virtual int next();

        /// Put back a character so that it is returned by the next call to next().
        virtual void put_back(int c);

        /// Get the current byte index in the input
        size_t get_byte_index() const;

        /// Reset the byte index at the start of parsing a messege
        void reset_byte_index(); 

    protected:

        /// When in block read mode, get the next available character
        int read_next_char_from_block();

        /// Special character value indicating that no character has been put back.
        static const int no_put_back = 1000;

        /// Implemented by derived classes to do the actual reading.
        virtual int read_next_char() = 0;

        /**
         * Implemented by derived classes to read the next block of data
         * up to block_size bytes.
         * \return  true if some data was read, false if EOF.
         * \throw   json_io_error if the read failed for some reason other than EOF.
         */
        virtual bool read_next_block() = 0;

        /// Character last put back. If equal to no_put_back then none was.
        int m_put_back;

        /// Total number of characters read.
        size_t m_byte_index;

        /// Maximum length of a message.
        size_t m_max_message_length;

        /// Whether to read in blocks or one byte at a time.
        bool m_block_read;

        /// If we're reading blocks, the block data.
        unsigned char m_block[block_size];

        /// If we're reading blocks, the number of bytes in the current block
        int m_block_num_bytes;

        /// If we're reading blocks, the index into the current block
        int m_block_index;
    };
}
