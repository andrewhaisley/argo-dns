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
