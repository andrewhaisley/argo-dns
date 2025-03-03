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
