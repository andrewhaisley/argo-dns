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

#include "exception.hpp"
#include "types.hpp"
#include "connection.hpp"

EXCEPTION_CLASS(schema_manager_exception, exception)

namespace adns
{
    namespace db
    {
        class schema_manager
        {
        public:
            schema_manager() = delete;

            /**
             * Update the schema or, if it doesn't exist at all, create it from scratch
             */
            static void update_or_create();

        private:

            /**
             * Create the ADNS schema in whatever DB we're using.
             */
            static void create(connection &conn);

            /**
             * If the schema version in the DB is lower than the version required
             * for this server version, update it.
             */
            static void update(connection &conn, const std::string &v);

            /**
             * Create the static data needed for a base install 
             */
            static void create_static(connection &conn);
        };
    }
}
