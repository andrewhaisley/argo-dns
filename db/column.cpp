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
