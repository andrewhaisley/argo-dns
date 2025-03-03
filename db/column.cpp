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
#include "column.hpp"

using namespace std;
using namespace adns;
using namespace db;

column::column() :
    m_type(column::invalid_e),
    m_position(0),
    m_nullable(false)
{
}

column::column(type_t type, const string &name, int position, bool nullable) :
    m_type(type),
    m_name(name),
    m_position(position),
    m_nullable(nullable)
{
}

column::~column()
{
}

column::type_t column::get_type() const
{
    return m_type;
}

const string &column::get_name() const
{
    return m_name;
}

bool column::get_nullable() const
{
    return m_nullable;
}

int column::get_position() const
{
    return m_position;
}
