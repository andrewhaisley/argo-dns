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
#include <sstream>
#include <iomanip>

#include "writer.hpp"

using namespace std;
using namespace adns;

writer::writer()
{
}

writer &adns::operator<<(writer& w, const std::string &s)
{
    w.write(s);
    return w;
}

writer &adns::operator<<(writer& w, char c)
{
    char s[2];
    s[0] = c;
    s[1] = '\0';
    w.write(s);
    return w;
}

writer &adns::operator<<(writer& w, int i)
{
    ostringstream ss;
    ss << i;
    w.write(ss.str());
    return w;
}

writer &adns::operator<<(writer& w, double d)
{
    ostringstream ss;
    ss << setprecision(17) << fixed << d;
    w.write(ss.str());
    return w;
}
