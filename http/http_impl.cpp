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
        res.add_header("www-authenticate", "Basic realm=\"Argo DNS\", Bearer");
        res.add_header("cache-control", "no-cache");
        res.add_header("access-control-allow-origin", "*");
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
