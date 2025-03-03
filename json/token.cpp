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
#include "token.hpp"

using namespace std;
using namespace adns;

token::token() noexcept : m_type(null_e)
{
}

token::token(const token &other)
{
    *this = other;
}

token::token(token &&other)
{
    *this = other;
}

token &token::operator=(const token &other)
{
    if (this != &other)
    {
        m_type = other.m_type;
        m_raw_value = other.m_raw_value;
    }
    return *this;
}

token &token::operator=(token &&other)
{
    if (this != &other)
    {
        m_type = other.m_type;
        other.m_type = null_e;
        m_raw_value = move(other.m_raw_value);
    }
    return *this;
}

token::token(token_type t) : m_type(t)
{
}

token::token(token_type t, const char *raw_value, size_t len) : m_raw_value(raw_value, len), m_type(t)
{
}

token::~token()
{
}

const string &token::get_raw_value() const
{
    return m_raw_value;
}

token::token_type token::get_type() const
{
    return m_type;
}
