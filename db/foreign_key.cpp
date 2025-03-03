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
#include "foreign_key.hpp"

using namespace std;
using namespace adns;
using namespace db;

foreign_key::foreign_key(
    const string &referenced_table,
    const column &referenced_column,
    const string &referencing_table,
    const column &referencing_column)
{
    m_referenced_table = referenced_table;
    m_referenced_column = referenced_column;
    m_referencing_table = referencing_table;
    m_referencing_column = referencing_column;
}

foreign_key::~foreign_key()
{
}

const string &foreign_key::get_referenced_table() const
{
    return m_referenced_table;
}

const column &foreign_key::get_referenced_column() const
{
    return m_referenced_column;
}

const string &foreign_key::get_referencing_table() const
{
    return m_referencing_table;
}

const column &foreign_key::get_referencing_column() const
{
    return m_referencing_column;
}
