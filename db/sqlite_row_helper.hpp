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
#include "connection.hpp"
#include "row.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(sqlite_exception, exception)

namespace adns
{
    namespace db
    {
        class sqlite_row_helper
        {
        public:
            sqlite_row_helper() = delete;

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

        private:

            static std::vector<row *> fetch_rows(connection &conn, const std::string &sql, const row &r);

            static std::string get_columns_string(const row &r);

            static int select_callback(void *rp, int count, char **data, char **columns);

            static std::string get_insert_placeholders_string(const row &r);

            static std::string get_update_placeholders_string(const row &r);

            static void set_placeholder_values(sqlite3_stmt *stmt, bool include_key, const row &r);

        };
    }
}
