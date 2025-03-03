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

#include "writer.hpp"

namespace adns
{
    /// A derived class of writer that writes to POSIX file descriptors.
    class fd_writer : public writer
    {
    public:

        /// constructor
        fd_writer(int fd);

        /**
         * Write a character to the file descriptor using the write() system call.
         * \throw json_io_exception Thrown in the event the write() call fails.
         */
        virtual void write(const std::string &s);

    private:

        int m_fd;
    };
}
