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
#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "types.hpp"
#include "buffer.hpp"
#include "exception.hpp"
#include "url.hpp"
#include "json.hpp"

EXCEPTION_CLASS(http_request_exception, exception)
EXCEPTION_CLASS(http_request_bad_format_exception, http_request_exception)

namespace adns
{
    class http_request
    {
    public:

        friend class http;
        friend class http1;
        friend class http2;

        /**
         * construct from parts
         */
        http_request(
            uint32_t                                           stream_id,
            const std::unordered_map<std::string, std::string> &raw_headers,
            const std::string                                  &raw_method,
            const std::string                                  &raw_url,
            const url                                          &p_url,
            const buffer                                       &raw_payload);

        /**
         * destructor
         */
        virtual ~http_request();

        /**
         * close connection once handled?
         */
        bool close_connection() const;

        /**
         * gets a header value - return "" if not found (or found and empty)
         */
        std::string get_header(const std::string &name) const;

        /**
         * sets a header value
         */
        void add_header(const std::string &name, const std::string &value);

        /**
         * get the method
         */
        const std::string &get_method() const;

        /**
         * extract the content-length header value
         */
        uint get_payload_size() const;

        /**
         * get the payload
         */
        const buffer &get_payload() const;

        /**
         * get the payload as a json object
         */
        json get_json_payload() const;

        /**
         * get the url
         */
        const url &get_url() const;

        /**
         * get the http2 stream ID
         */
        uint32_t get_stream_id() const;

        /**
         * dump for debug
         */
        void dump() const;

    private:

        // only relevant for requests sent via http2
        uint32_t    m_stream_id;

        std::unordered_map<std::string, std::string> m_raw_headers;

        std::string m_raw_method;
        std::string m_raw_url;
        url         m_url;
        buffer      m_raw_payload;

        /**
         * invalid request
         */
        http_request();

        /**
         * take the HTTP request line and break it out into parts
         */
        void add_request_line(const std::string &s);

        /**
         * add a header line from a raw string
         */
        void add_headers(const std::vector<std::string> &headers);

    };
}
