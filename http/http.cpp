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
#include <boost/algorithm/string.hpp>    

#include "http.hpp"
#include "http1.hpp"
#include "http2.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

http::http(usage_t u, tcp_socket &s, uint timeout)
{
    if (s.get_protocol() == tcp_socket::http_1_1_e)
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
