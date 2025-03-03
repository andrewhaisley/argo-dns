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
#include "connection.hpp"
#include "row.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(mongodb_exception, exception)

namespace adns
{
    namespace db
    {
        class mongodb_row_helper
        {
        public:
            mongodb_row_helper() = delete;

            static void insert_row(connection &conn, row &r);

            static void update_row(connection &conn, row &r);

            static void delete_row(connection &conn, row &r);

            static row *fetch_row(connection &conn, const column &c, uuid unique_key, const row &r);

            static std::vector<row *> fetch_rows(connection &conn, const column &c, uuid key, const row &r);

            static std::vector<row *> fetch_rows(connection &conn, const row &r);

            static void delete_rows(connection &conn, const column &c, uuid key, const row &r);

            static void begin_transaction(connection &conn);

            static void commit_transaction(connection &conn);

            static void rollback_transaction(connection &conn);

            static void create_collection(connection &conn, const std::string &name);

            static void create_index(connection &conn, const std::string &json_params);
        };
    }
}
