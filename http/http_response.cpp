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

#include "http_response.hpp"
#include "util.hpp"
#include "unparser.hpp"
#include "datetime.hpp"

using namespace std;
using namespace adns;

map<http_response::status_t, string> http_response::o_response_codes = 
{
    { ok_200, "OK" },
    { bad_request_400, "Bad Request" },
    { timeout_408, "Request Timeout" },
    { entity_too_large_413, "Request Entity Too Large" },
    { header_fields_too_large_431, "Request Header Fields Too Large" },
    { internal_server_error_500, "Internal Server Error" },
    { not_implemented_501, "Not Implemented" },
    { unauthorized_401, "Unauthorized" },
    { not_found_404, "Not Found" },
    { method_not_allowed_405, "Method Not Allowed" }
};

http_response::http_response(uint32_t stream_id, status_t status) : m_stream_id(stream_id), m_status(status)
{
}

http_response::status_t http_response::get_status() const
{
    return m_status;
}

const string http_response::get_status_text() const
{
    if (o_response_codes.find(m_status) == o_response_codes.end())
    {
        return "Unknown Status";
    }
    else
    {
        return o_response_codes[m_status];
    }
}

http_response::http_response(uint32_t stream_id, status_t status, const exception &e) : m_stream_id(stream_id), m_status(status)
{
    m_json = json(json::object_e);

    m_json["method"] = e.get_method();
    m_json["file"] = e.get_file();
    m_json["line"] = e.get_line();
    m_json["description"] = e.get_description();
    m_json["details"] = e.get_details();
    m_json["code"] = int(e.get_code());
}

http_response::http_response(uint32_t stream_id, status_t status, const json &j) : m_stream_id(stream_id), m_status(status), m_json(j)
{
}

http_response::http_response(uint32_t stream_id, status_t status, const buffer &b, const string &content_type) : 
                    m_stream_id(stream_id), m_status(status), m_buffer(b), m_content_type(content_type)
{
}

http_response::~http_response()
{
}

void http_response::add_header(const string &name, const string &value)
{
    auto n = name;
    boost::algorithm::to_lower(n);
    m_headers[n] = value;
}

const map<string, string> &http_response::get_headers() const
{
    return m_headers;
}

void http_response::dump() const
{
    LOG(info) << "dump response";
    LOG(info) << "status code " << m_status << " " << o_response_codes[m_status];

    LOG(info) << "json contents " << m_json;
    LOG(info) << "binary contents " << m_buffer;
}

const json &http_response::get_json() const
{
    return m_json;
}

const buffer &http_response::get_data() const
{
    return m_buffer;
}

uint32_t http_response::get_stream_id() const
{
    return m_stream_id;
}

const string &http_response::get_content_type() const
{
    return m_content_type;
}
