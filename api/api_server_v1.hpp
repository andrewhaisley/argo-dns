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

#include <string>

#include "types.hpp"
#include "api_url_v1.hpp"
#include "http_request.hpp"
#include "http_response.hpp"

namespace adns
{
    /**
     * server config API
     */
    class api_server_v1
    {
    public:

        /**
         * no instantiation
         */
        api_server_v1() = delete;

        /**
         * serve a server request of any type
         */
        static std::shared_ptr<http_response> handle_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

    private:

        /**
         * serve a GET request 
         */
        static std::shared_ptr<http_response> handle_get_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

        /**
         * serve a PUT/update request 
         */
        static std::shared_ptr<http_response> handle_update_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

        /**
         * serve a POST/insert request 
         */
        static std::shared_ptr<http_response> handle_insert_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

        /**
         * serve a DELETE request 
         */
        static std::shared_ptr<http_response> handle_delete_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);
    };
}
