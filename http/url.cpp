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

    if (!util::path_is_safe(path))
    {
        THROW(url_exception, "badly formed path", p);
    }

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
