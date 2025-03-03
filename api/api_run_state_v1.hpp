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
    class api_run_state_v1
    {
    public:

        /**
         * no instantiation
         */
        api_run_state_v1() = delete;

        /**
         * serve a run state request of any type
         */
        static std::shared_ptr<http_response> handle_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

    private:

        /**
         * handle a dynamic server restart
         */
        static std::shared_ptr<http_response> handle_restart(const std::string &version);
    };
}
