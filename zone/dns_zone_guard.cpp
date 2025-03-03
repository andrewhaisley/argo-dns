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
#include "dns_zone_guard.hpp"

using namespace std;
using namespace adns;

read_write_lock dns_zone_guard::o_lock;

dns_zone_guard::dns_zone_guard(type_t t) : m_type(t)
{
    if (m_type == read_e)
    {
        o_lock.lock_read();
    }
    else
    {
        o_lock.lock_write();
    }
}

dns_zone_guard::~dns_zone_guard()
{
    if (m_type == read_e)
    {
        o_lock.unlock_read();
    }
    else
    {
        o_lock.unlock_write();
    }
}
