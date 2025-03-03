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
#include <boost/algorithm/string.hpp>    

#include <vector>

#include "util.hpp"
#include "url.hpp"

using namespace std;
using namespace adns;

url::url()
{
}

url::~url()
{
}

url::url(const string &s) : m_raw(s)
{
    string path, query;

    auto p = s.find('?');

    if (p == string::npos)
    {
        path = s;
    }
    else
    {
        path = s.substr(0, p);
        if (path.size() > 0 && path.back() != '/')
        {
            path += '/';
        }

        query = s.substr(p + 1);
    }

    m_path = util::fromhttp(path);

    if (query != "")
    {
        vector<string> bits1;
        split(bits1, query, boost::is_any_of("&"));

        for (auto p : bits1)
        {
            vector<string> bits2;
            split(bits2, p, boost::is_any_of("="));

            if (bits2.size() == 2)
            {
                m_parameters[util::fromhttp(bits2[0])] = util::fromhttp(bits2[1]);
            }
            else
            {
                THROW(url_exception, "badly formed query parameter", p);
            }
        }
    }
}

const string &url::get_path() const
{
    return m_path;
}

const string &url::get_raw() const
{
    return m_raw;
}

string url::get_parameter(const string &name, const string &default_value) const
{
    auto x = m_parameters.find(name);
    if (x == m_parameters.end())
    {
        return default_value;
    }
    else
    {
        return x->second;
    }
}

const map<string, string> url::get_parameters() const
{
    return m_parameters;
}

void url::dump() const
{
    LOG(debug) << "RAW " << m_raw;
    LOG(debug) << "URL PATH " << m_path;

    for (auto x : m_parameters)
    {
        LOG(debug) << "PARAM " << x.first << "=" << x.second;
    }
}
