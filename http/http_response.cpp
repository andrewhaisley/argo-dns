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
    { unauthorised_401, "Unauthorized" },
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

http_response::http_response(uint32_t stream_id, status_t status, const buffer &b) : m_stream_id(stream_id), m_status(status), m_buffer(b)
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
