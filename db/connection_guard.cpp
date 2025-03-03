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
#include "types.hpp"
#include "connection_guard.hpp"
#include "connection_pool.hpp"

#undef shared_ptr

using namespace std;
using namespace adns;
using namespace db;

connection_guard::connection_guard(const shared_ptr<connection> &conn) : m_connection(conn)
{
}

connection_guard::~connection_guard()
{
    if (m_connection)
    {
        connection_pool::put_connection(m_connection);
    }
}

connection &connection_guard::get()
{
    return *m_connection;
}

connection_guard::operator connection&() const
{
    return *m_connection;
}
