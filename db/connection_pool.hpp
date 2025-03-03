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
