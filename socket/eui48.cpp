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
#include <string.h>

#include "types.hpp"
#include "util.hpp"
#include "eui48.hpp"

using namespace std;
using namespace adns;

eui48::~eui48()
{
}

eui48::eui48()
{
    memset(&m_eui48, 0, 6);
}

eui48::eui48(const string &s)
{
    init(s);
}

string eui48::to_string() const
{
    string res;
    char buffer[4];
    for (int i = 0; i < 6; i++)
    {
        if (i == 0)
        {
            sprintf(buffer, "%02x", m_eui48[i]);
        }
        else
        {
            sprintf(buffer, "-%02x", m_eui48[i]);
        }
        res += buffer;
    }
    return res;
}

void eui48::json_serialize() const
{
    set_json("address", to_string());
}

void eui48::json_unserialize()
{
    init(*m_json_object);
}

void eui48::init(const std::string &s)
{
    if (s.size() != 17)
    {
        THROW(eui48_exception, "string isn't 17 characters", s);
    }

    for (int i = 0; i < 6; i++)
    {
        auto j = i * 3;
        m_eui48[i] = util::fromhexchar(s[j]) * 16 + util::fromhexchar(s[j + 1]);
    }
}
