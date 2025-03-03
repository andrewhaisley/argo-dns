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

#include <istream>

#include "reader.hpp"

namespace adns
{
    /// A class to read JSON messages from C++ istreams.
    class stream_reader : public reader
    {
    public:

        /**
         * Constructor.
         * \param   s                   IOSTREAMS input stream. The instance does not take
         *                              ownership of the stream and it is up to the calling 
         *                              code to close it when done.
         * \param   max_message_length  Longest message that can be read.
         * \param   block_read          Whether to read in blocks or one byte at a time.
         */
        stream_reader(std::istream *s, int max_message_length, bool block_read);

    protected:

        /**
         * Read a character from the file using fread()
         * \throw json_io_exception Thrown if fread fails in some way.
         */
        virtual int read_next_char();

        /**
         * Read the next block of data up to block_size bytes.
         * \return  true if some data was read, false if EOF.
         * \throw   json_io_error if the read failed for some reason other than EOF.
         */
        virtual bool read_next_block();

        /// The C++ stream to read from.
        std::istream *m_stream;
    };
}
