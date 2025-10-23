//
//  Copyright 2025 Andrew Haisley
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
//  associated documentation files (the “Software”), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial 
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
//  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
 
#pragma once

#include "types.hpp"
#include "api_url_v1.hpp"
#include "http_response.hpp"
#include "http_request.hpp"

namespace adns
{
    class api_address_list_v1
    {
    public:

        /**
         * no instantiation
         */
        api_address_list_v1() = delete;

        /**
         * serve a api_address_list_v1 request of any type
         */
        static std::shared_ptr<http_response> handle_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

    private:

        /**
         * serve a GET address_list request 
         */
        static std::shared_ptr<http_response> handle_get_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

        /**
         * serve a PUT/update address_list request 
         */
        static std::shared_ptr<http_response> handle_update_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

        /**
         * serve a POST/insert address_list request 
         */
        static std::shared_ptr<http_response> handle_insert_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

        /**
         * serve a DELETE address_list request 
         */
        static std::shared_ptr<http_response> handle_delete_request(std::shared_ptr<http_request> &req, const api_url_v1 &p);

    };
}
