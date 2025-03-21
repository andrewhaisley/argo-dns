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
#include "stream_writer.hpp"

using namespace std;
using namespace adns;

stream_writer::stream_writer(ostream *s) : writer(), m_stream(s)
{
}

void stream_writer::write(const string &s)
{
    // for some reason writing the string direct confuses Visual C++ hence the .c_str() call
    (*m_stream) << s.c_str();
}
