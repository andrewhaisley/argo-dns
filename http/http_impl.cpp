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
#include <boost/lexical_cast.hpp>

#include "unparser.hpp"
#include "datetime.hpp"
#include "json.hpp"
#include "http_impl.hpp"

using namespace std;
using namespace adns;

http_impl::http_impl()
{
}

http_impl::~http_impl()
{
}

void http_impl::add_headers(http::usage_t u, http_response &res, int content_length)
{
    datetime now = datetime::now();

    res.add_header("server", "Argo-DNS");
    res.add_header("date", now.to_http_string());
    res.add_header("expires", now.to_http_string());

    string j;

    if (u == http::doh_e)
    {
        // add DNS headers
        res.add_header("content-type", "application/dns-message");
        res.add_header("access-control-allow-origin", "*");
        res.add_header("content-length", boost::lexical_cast<string>(res.get_data().get_size()));
    }
    else if (u == http::api_e)
    {
        // add API headers
        res.add_header("content-type", "application/json");
        res.add_header("www-authenticate", "Basic realm=\"ADNS\"");
        res.add_header("cache-control", "no-cache");
        res.add_header("content-length", boost::lexical_cast<string>(content_length));
    }
    else
    {
        // add UI headers
        // set cache expiry to 24 hours - we don't expect content to change
        // very often
        res.add_header("cache-control", "max-age=86400");
        res.add_header("content-type", res.get_content_type());
        res.add_header("content-length", boost::lexical_cast<string>(content_length));
    }
}
