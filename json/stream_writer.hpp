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

#include <ostream>

#include "writer.hpp"

namespace adns
{
    /// A class to write JSON messages to a C++ istream.
    class stream_writer : public writer
    {
    public:

        /**
         * Constructor. Does not take over owenership of the stream,
         * the caller is required to close it and deallocate (if needed)
         * when done.
         */
        stream_writer(std::ostream *s);

        /**
         * Write a string to the stream.
         */
        virtual void write(const std::string &s);

    private:

        std::ostream *m_stream;

    };

}
