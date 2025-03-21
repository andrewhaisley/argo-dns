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
#include <errno.h>

#include "file_writer.hpp"
#include "json_io_exception.hpp"

using namespace std;
using namespace adns;

file_writer::file_writer(FILE *f) : writer(), m_file(f)
{
}

void file_writer::write(const string &s)
{
    if (fwrite(s.c_str(), s.size(), 1, m_file) != 1)
    {
        throw json_io_exception(json_io_exception::write_file_failed_e, errno);
    }
}
