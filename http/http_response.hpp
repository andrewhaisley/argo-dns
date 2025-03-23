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

#include <map>
#include <string>

#include "types.hpp"
#include "buffer.hpp"
#include "exception.hpp"
#include "json.hpp"

namespace adns
{
    class http_response
    {
    public:

        typedef enum
        {
            ok_200                      = 200,
            bad_request_400             = 400,
            unauthorised_401            = 401,
            not_found_404               = 404,
            method_not_allowed_405      = 405,
            conflict_407                = 407,
            timeout_408                 = 408,
            entity_too_large_413        = 413,
            header_fields_too_large_431 = 431,
            internal_server_error_500   = 500,
            not_implemented_501         = 501
        }
        status_t;

        static std::map<status_t, std::string> o_response_codes;

        /**
         * no default constructor, need at least a status code
         */
        http_response() = delete;

        /**
         * empty response
         */
        http_response(uint32_t stream_id, status_t status);

        /**
         * response containing a json encoded exception
         */
        http_response(uint32_t stream_id, status_t status, const exception &e);

        /**
         * response containing a json message
         */
        http_response(uint32_t stream_id, status_t status, const json &j);

        /**
         * response containing binary data
         */
        http_response(uint32_t stream_id, status_t status, const buffer &b, const std::string &content_type="");

        /**
         * destructor
         */
        virtual ~http_response();

        /**
         * get the json payload
         */
        const json &get_json() const;

        /**
         * get the data payload
         */
        const buffer &get_data() const;

        /** 
         * add an HTTP header (aka field)
         */
        void add_header(const std::string &name, const std::string &value);

        /**
         * get the headers (aka fields)
         */
        const std::map<std::string, std::string> &get_headers() const;

        /**
         * get the status
         */
        status_t get_status() const;

        /**
         * get the stream ID (only relevant for http2)
         */
        uint32_t get_stream_id() const;

        /**
         * get the status text
         */
        const std::string get_status_text() const;

        /**
         * get the content type
         */
        const std::string &get_content_type() const;

        /**
         * dump out for debug purposes
         */
        void dump() const;

    private:

        uint32_t m_stream_id;

        status_t m_status;
        json     m_json;
        buffer   m_buffer;

        std::string m_content_type;

        std::map<std::string, std::string> m_headers;
    };
}
