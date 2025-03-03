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

#include "reader.hpp"

namespace adns
{
    /// A derived class of reader that reads from POSIX file descriptors.
    class fd_reader : public reader
    {
    public:

        /**
         * Constructor.
         * \param   fd                  POSIX stream file descriptor. The instance does not take
         *                              ownership of the open file and it is up to the calling 
         *                              code to close it when done.
         * \param   max_message_length  Longest message that can be read.
         * \param   block_read          Whether to read in blocks or one byte at a time.
         */
        fd_reader(int fd, int max_message_length, bool block_read);

    protected:

        /**
         * Read a character from the file descriptor using the read() system call.
         * \throw json_io_exception Thrown in the event the read() call fails.
         */
        virtual int read_next_char();

        /**
         * Read the next block of data up to block_size bytes.
         * \return  true if some data was read, false if EOF.
         * \throw   json_io_error if the read failed for some reason other than EOF.
         */
        virtual bool read_next_block();

        /// Underlying POSIX file descriptor.
        int m_fd;
    };
}
