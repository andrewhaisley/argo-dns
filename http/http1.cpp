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

#include "types.hpp"
#include "datetime.hpp"
#include "unparser.hpp"
#include "http1.hpp"
#include "http_request.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

http1::http1(http::usage_t u, tcp_socket &s, uint timeout) : m_usage(u), m_socket(s), m_timeout(timeout)
{
}

http1::~http1()
{
}

string http1::read_line()
{
    string res;

    res.reserve(HTTP_MAX_HEADER_LINE_LENGTH);

    while (res.size() < HTTP_MAX_HEADER_LINE_LENGTH)
    {
        buffer b = m_socket.read(1, m_timeout);
        if (b.get_size() == 1)
        {
            char c = static_cast<char>(b.get_data()[0]);
            if (c == '\n')
            {
                if (res.size() > 0 && res.back() == '\r')
                {
                    res.pop_back();
                }
                return res;
            }
            else    
            {
                res += c;
            }
        }
        else
        {
            THROW(http_bad_format_exception, "http line not terminated with CFLR");
        }
    }

    THROW(http_header_line_too_long, "http header line exceeded maximum length", HTTP_MAX_HEADER_LINE_LENGTH);
}

void http1::to_wire(http_response &res)
{
    m_socket.write(buffer(
                        "HTTP/1.1 " + 
                        boost::lexical_cast<string>(res.get_status()) + 
                        " " + 
                        res.get_status_text() +
                        "\r\n"), 
                   m_timeout);

    string json_string;

    if (m_usage == http::api_e) 
    {
        if (res.get_json().get_instance_type() == json::object_e)
        {
            json_string << res.get_json();
        }
        add_headers(m_usage, res, json_string.size());
    }
    else
    {
        add_headers(m_usage, res, res.get_data().get_size());
    }
    
    for (auto h : res.get_headers())
    {
        m_socket.write(buffer(h.first + ": " + h.second + "\r\n"), m_timeout);
    }

    m_socket.write(buffer("\r\n"), m_timeout);

    if (m_usage == http::api_e)
    {
        m_socket.write(buffer(json_string), m_timeout);
    }
    else
    {
        m_socket.write(res.get_data(), m_timeout);
    }
}

shared_ptr<http_request> http1::from_wire()
{
    shared_ptr<http_request> res(new http_request);
    res->add_request_line(read_line());

    vector<string> headers;
    while (true)
    {
        string line = read_line();
        if (line == "")
        {
            break;
        }
        else
        {
            headers.push_back(line);
        }
    }

    res->add_headers(headers);

    uint ps = res->get_payload_size();

    if (ps > 0)
    {
        res->m_raw_payload = m_socket.read(ps, m_timeout);
    }

    return res;
}
