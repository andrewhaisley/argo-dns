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
#include <boost/algorithm/string.hpp>    

#include "http.hpp"
#include "http1.hpp"
#include "http2.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

http::http(usage_t u, tcp_socket &s, uint timeout)
{
    if ((s.get_protocol() == tcp_socket::http_1_1_e) || (s.get_protocol() == tcp_socket::none_e))
    {
        m_protocol = new http1(u, s, timeout);
    }
    else if (s.get_protocol() == tcp_socket::http_2_e)
    {
        m_protocol = new http2(u, s, timeout);
    }
    else
    {
        THROW(http_exception, "unknown HTTP protocol version", s.get_protocol());
    }
}

http::~http()
{
    delete m_protocol;
}

shared_ptr<http_request> http::from_wire()
{
    return m_protocol->from_wire();
}

void http::to_wire(http_response &res)
{
    m_protocol->to_wire(res);
}
