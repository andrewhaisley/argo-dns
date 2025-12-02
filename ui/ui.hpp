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

        /**
         * Get a response giving the control server port - this is used by the GUI client
         * to create a reasonable default URL for the API.
         */
        std::shared_ptr<http_response> get_api_port();

    };
}
