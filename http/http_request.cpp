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

#include "http_request.hpp"
#include "util.hpp"
#include "parser.hpp"

using namespace std;
using namespace adns;

http_request::http_request() : m_stream_id(0)
{
}

http_request::~http_request()
{
}

void http_request::dump() const
{
    LOG(debug) << "RAW URL:" << m_raw_url;
    m_url.dump();
    LOG(debug) << "METHOD:" << m_raw_method;

    for (auto x : m_raw_headers)
    {
        LOG(debug) << "HEADER:" << x.first << ":" << x.second;
    }

    LOG(debug) << "PAYLOAD:" << m_raw_payload;
    LOG(debug) << "END";
}

uint http_request::get_payload_size() const
{
    string v = get_header("content-length");

    if (v == "")
    {
        return 0;
    }
    else
    {
        try
        {
            return boost::lexical_cast<uint>(v);
        }
        catch (boost::bad_lexical_cast&)
        {
            THROW(http_request_bad_format_exception, "content-length header is malformed", v);
        }
    }
}

string http_request::get_header(const string &name) const
{
    auto x = m_raw_headers.find(name);

    if (x == m_raw_headers.end())
    {
        return "";
    }
    else
    {
        return x->second;
    }
}

bool http_request::close_connection() const
{
    return get_header("connection") != "keep-alive";
}

const string &http_request::get_method() const
{
    return m_raw_method;
}

const buffer &http_request::get_payload() const
{
    return m_raw_payload;
}

json http_request::get_json_payload() const
{
    return *(parser::parse(m_raw_payload.to_ascii_string()));
}

const url &http_request::get_url() const
{
    return m_url;
}

void http_request::add_request_line(const string &s)
{
    vector<string> bits;
    split(bits, s, boost::is_any_of(" "));

    if (bits.size() != 3)
    {
        THROW(http_request_bad_format_exception, "http request line is malformed", s);
    }

    if (bits[2] != "HTTP/1.0" && bits[2] != "HTTP/1.1")
    {
        THROW(http_request_bad_format_exception, "http request protocol unknown", bits[2]);
    }

    m_raw_method = bits[0];
    m_raw_url = bits[1];

    try
    {
        m_url = url(m_raw_url);
    }
    catch (url_exception &e)
    {
        THROW(http_request_bad_format_exception, "malformed url", e.what());
    }
}

void http_request::add_header(const string &name, const string &value)
{
    m_raw_headers[name] = value;
}

void http_request::add_headers(const vector<string> &headers)
{
    string last_header_name;

    for (auto h : headers)
    {
        if (h.size() == 0)
        {
            THROW(http_request_bad_format_exception, "header is malformed - zero size");
        }
        else if (h[0] == ' ')
        {
            if (last_header_name == "")
            {
                THROW(http_request_bad_format_exception, "header is malformed", h);
            }
            else
            {
                boost::algorithm::to_lower(last_header_name);
                m_raw_headers[last_header_name] += h.substr(1);
            }
        }
        else
        {
            auto i = h.find(':');
            if (i == string::npos)
            {
                THROW(http_request_bad_format_exception, "header is malformed", h);
            }

            last_header_name = h.substr(0, i);

            if (i < (h.length() - 2))
            {
                boost::algorithm::to_lower(last_header_name);
                m_raw_headers[last_header_name] = h.substr(i + 2);
            }
        }
    }
}

http_request::http_request(
    uint32_t                                      stream_id,
    const unordered_map<std::string, std::string> &raw_headers,
    const string                                  &raw_method,
    const string                                  &raw_url,
    const url                                     &p_url,
    const buffer                                  &raw_payload) :
                            m_stream_id(stream_id),
                            m_raw_headers(raw_headers),
                            m_raw_method(raw_method),
                            m_raw_url(raw_url),
                            m_url(p_url),
                            m_raw_payload(raw_payload)
{
}

uint32_t http_request::get_stream_id() const
{
    return m_stream_id;
}
