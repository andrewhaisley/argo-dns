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

#include <list>
#include <string>
#include <mutex>
#include <chrono>
#include <ctime>

#include "types.hpp"
#include "connection_guard.hpp"
#include "connection.hpp"

namespace adns
{
    namespace db
    {
        class connection_pool final
        {
        public:

            /**
             * used to hold a connection in the pool. The last used time is needed
             * as connections for some DB types go stale after a certain period 
             * (e.g. due to tcp/ip connection timeouts) and need to be discarded.
             */
            struct connection_t
            {
                std::shared_ptr<connection>                        conn;
                std::chrono::time_point<std::chrono::steady_clock> last_used;
            };

            /**
             * no construction of instances
             */
            connection_pool() = delete;

            /**
             * initialise connection login/access details
             */
            static void init(
                    type_t             db_type, 
                    const std::string  &url, 
                    const std::string  &username, 
                    const std::string  &password, 
                    const std::string  &database,
                    bool               use_transactions);

            /**
             * get a connection from the pool
             */
            static std::shared_ptr<connection_guard> get_connection();

            /**
             * put a connection back in the pool when done with it - not needed if the
             * connection_guard is used.
             */
            static void put_connection(std::shared_ptr<connection> &conn);

        private:

            static std::list<connection_t> o_pool;
            static std::mutex o_lock;

            static type_t o_db_type;

            static std::string o_url;
            static std::string o_username;
            static std::string o_password;
            static std::string o_database;
            static bool o_use_transactions;
        };
    }
}
