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
#include "api_url_v1.hpp"
#include "http_response.hpp"
#include "http_request.hpp"

namespace adns
{
    class api_cache_configuration_v1
    {
    public:

        /**
         * no instantiation
         */
        api_cache_configuration_v1() = delete;

        /**
         * serve a api_cache_configuration_v1 request of any type
         */
        static std::shared_ptr<http_response> handle_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

    private:

        /**
         * serve a GET configuration request 
         */
        static std::shared_ptr<http_response> handle_get_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

        /**
         * serve a PUT/update configuration request 
         */
        static std::shared_ptr<http_response> handle_update_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

    };
}
