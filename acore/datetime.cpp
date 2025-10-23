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
 
#include "types.hpp"
#include <string.h>
#include "datetime.hpp"

using namespace std;
using namespace adns;

datetime::datetime()
{
    memset(&m_time, 0, sizeof(struct tm));
    memset(&m_gm_time, 0, sizeof(struct tm));
}

datetime datetime::now()
{
    return datetime(time(nullptr));
}

datetime::~datetime()
{
}

datetime::datetime(const string &s)
{
    memset(&m_time, 0, sizeof(struct tm));
    memset(&m_gm_time, 0, sizeof(struct tm));

    if (strptime(s.c_str(), "%Y-%m-%d %H:%M:%S", &m_time) == nullptr)
    {
        THROW(datetime_exception, "invalid string format", s);
    }

    time_t t = to_time_t();
    gmtime_r(&t, &m_gm_time);
}

const string &datetime::to_string() const
{
    char buffer[100];

    strftime(buffer, 100, "%Y-%m-%d %H:%M:%S", &m_time);
    m_string_form = string(buffer);

    return m_string_form;
}

const string &datetime::to_http_string() const
{
    char buffer[100];

    strftime(buffer, 100, "%a, %e %b %Y %H:%M:%S %Z", &m_gm_time);
    m_string_form = string(buffer);

    return m_string_form;
}

datetime::datetime(time_t t)
{
    localtime_r(&t, &m_time);
    gmtime_r(&t, &m_gm_time);
}

time_t datetime::to_time_t() const
{
    struct tm x = m_time;
    return mktime(&x) - timezone;
}

time_t datetime::operator-(const datetime &other) const
{
    return to_time_t() - other.to_time_t();
}

ostream &adns::operator<<(ostream& stream, const datetime &dt)
{
    stream << dt.to_string();
    return stream;
}
