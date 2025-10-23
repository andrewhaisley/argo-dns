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

#include "config.hpp"
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

bool http_request::authorized() const
{
    string a = get_header("authorization");

    if (a == "")
    {
        return false;
    }
    else
    {
        vector<string> bits;
        split(bits, a, boost::is_any_of(" "));

        if (bits.size() == 2)
        {
            if (bits[0] == "Basic")
            {
                try
                {
                    string s = util::frombase64(bits[1]).to_ascii_string();
                    split(bits, s, boost::is_any_of(":"));
                    if (bits.size() == 2)
                    {
                        return bits[0] == config::server_config_username() &&
                               bits[1] == config::server_config_password();
                    }
                    else
                    {
                        THROW(http_request_bad_format_exception, "malformed basic authentication username/password");
                    }
                }
                catch (util_exception &e)
                {
                    THROW(http_request_bad_format_exception, "malformed basic authentication username/password base64 string");
                }
            }
            else
            {
                THROW(http_request_bad_format_exception, "only Basic authentication is supported", bits[0]);
            }
        }
        else
        {
            THROW(http_request_bad_format_exception, "malformed basic authentication header");
        }
    }
}
