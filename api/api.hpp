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
#include "http_response.hpp"
#include "http_request.hpp"
#include "api_exception.hpp"
#include "api_v1.hpp"

namespace adns
{
    class api
    {
    public:

        /**
         * new API instance
         */
        api();

        /**
         * destructor
         */
        virtual ~api();

        /**
         * handle a request returning the result (or throwing an exception)
         */
        std::shared_ptr<http_response> handle_request(std::shared_ptr<http_request> &req);

    private:

        // API instances by version - only version 1 at the moment
        api_v1 m_v1;

    };
}
