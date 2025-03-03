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
#include "http_request.hpp"
#include "http_response.hpp"
#include "exception.hpp"
#include "api_url_v1.hpp"

namespace adns
{
    class api_v1
    {
    public:

        /** 
         * V1 API instance
         */
        api_v1();

        /**
         * destructor
         */
        virtual ~api_v1();

        /**
         * handle a request
         */
        virtual std::shared_ptr<http_response> handle_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

    protected:

        /**
         * find out if the caller is authorised - uses basic authentication via headers
         */
        virtual bool authorised(std::shared_ptr<http_request> &req);

    };
}
