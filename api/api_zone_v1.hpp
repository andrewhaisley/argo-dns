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

#include "types.hpp"
#include "json.hpp"
#include "api_url_v1.hpp"
#include "http_response.hpp"
#include "http_request.hpp"
#include "dns_horizon.hpp"

namespace adns
{
    class api_zone_v1
    {
    public:

        /**
         * no instantiation
         */
        api_zone_v1() = delete;

        /**
         * serve a api_zone_v1 request of any type
         */
        static std::shared_ptr<http_response> handle_request(std::shared_ptr<http_request> &req, const api_url_v1 &u);

    private:

        /**
         * handle a GET request
         */
        static std::shared_ptr<http_response> handle_get_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

        /**
         * handle an insert request
         */
        static std::shared_ptr<http_response> handle_insert_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

        /**
         * handle an update request
         */
        static std::shared_ptr<http_response> handle_update_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

        /**
         * handle a DELETE request
         */
        static std::shared_ptr<http_response> handle_delete_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);
    };
}
