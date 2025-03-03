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
#include <libpq-fe.h>

namespace adns
{
    namespace db
    {
        class postgres_row_helper
        {
        public:
            postgres_row_helper() = delete;

            static void insert_row(connection &conn, row &r);

            static void update_row(connection &conn, row &r);

            static void delete_row(connection &conn, row &r);

            static row *fetch_row(connection &conn, const column &c, uuid unique_key, const row &r);

            static std::vector<row *> fetch_rows(connection &conn, const column &c, uuid key, const row &r);

            static std::vector<row *> fetch_rows(connection &conn, const row &r);

            static void delete_rows(connection &conn, const column &c, uuid key, const row &r);

            static void execute(connection &conn, const std::string &sql);

            static void begin_transaction(connection &conn);

            static void commit_transaction(connection &conn);

            static void rollback_transaction(connection &conn);

            static std::string error_string(PGconn *conn);

        private:

            static std::vector<row *> fetch_rows(connection &conn, PGresult *res, const row &r);

            static void clear_and_throw_exception(connection &conn, PGresult *res);

            static std::string get_insert_placeholders_string(const row &r);

            static std::string get_update_placeholders_string(const row &r);

            static void set_placeholder_values(char *values[], const row &r);

            static std::string get_columns_string(const row &r);

            static void check_foreign_keys(connection &conn, const row &r);

            static void free_placeholder_values(char *values[], int size);
        };
    }
}
