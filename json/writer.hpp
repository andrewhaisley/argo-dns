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

namespace adns
{
    /**
     * \brief Simple proxy for write operations on various types of stream.
     *
     * A class that acts as a proxy for the various places that JSON messages
     * can be written to (or to which they can be written even).
     * Actually writing to somewhere is in in derived classes.
     */
    class writer
    {
    public:

        /// Constructor.
        writer();

        /// Destructor to shut gcc up.
        virtual ~writer() {};

        /// Write a string - implemented by derived classes.
        virtual void write(const std::string &s) = 0;

    };

    /// Write a string to a writer.
    writer &operator<<(writer& w, const std::string &s);
    /// Write a single character.
    writer &operator<<(writer& w, char c);
    /// Write an int (formatted as a string. i.e. 123 is written as "123").
    writer &operator<<(writer& w, int i);
    /// Write a double (formatted as a string. i.e. 123e23 is written as "123e23").
    writer &operator<<(writer& w, double d);
}
