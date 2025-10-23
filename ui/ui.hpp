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

namespace adns
{
    // Note that there no authentication or authorization needed for serving up the UI
    // as that's all handled by the API. All this class does is serve up the VUE app and
    // relevant assets.
    class ui
    {
    public:

        /**
         * new UI instance
         */
        ui(const std::string &document_root);

        /**
         * destructor
         */
        virtual ~ui();

        /**
         * handle a request returning the result (or throwing an exception)
         */
        std::shared_ptr<http_response> handle_request(std::shared_ptr<http_request> &req);

    private:

        std::string m_document_root;

        /**
         * handle a get request returning the result (or throwing an exception)
         */
        std::shared_ptr<http_response> handle_get_request(std::shared_ptr<http_request> &req);

    };
}
