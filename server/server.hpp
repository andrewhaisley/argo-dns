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

#include <boost/asio.hpp>

#include "types.hpp"

namespace adns
{
    /**
     * Abstract base class for servers managed by the server_container class.
     */
    class server
    {
    public:

        /**
         * Constructor
         */
        server(boost::asio::io_service &io_service);

        /**
         * Destructor
         */
        virtual ~server();

        /**
         * Start threads running and return.
         */
        virtual void run() = 0;

        /**
         * Wait until all running threads have joined.
         */
        virtual void join() = 0;

        /**
         * Name for the server.
         */
        virtual std::string name() const = 0;

    protected:

        boost::asio::io_service &m_io_service;

    };
}
