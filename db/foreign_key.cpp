//
//  Copyright 2025 Andrew Haisley
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
//  associated documentation files (the “Software”), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial 
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
//  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
