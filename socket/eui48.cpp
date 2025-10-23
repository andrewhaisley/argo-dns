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
