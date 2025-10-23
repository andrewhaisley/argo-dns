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
 
#include "schema_manager.hpp"
#include "connection_pool.hpp"
#include "row_schema_version.hpp"
#include "table.hpp"
#include "mysql_row_helper.hpp"
#include "sqlite_row_helper.hpp"
#include "mongodb_row_helper.hpp"
#include "postgres_row_helper.hpp"

using namespace std;
using namespace adns;
using namespace db;

void schema_manager::update_or_create()
{
    LOG(info) << "checking schema version";
    auto conn = connection_pool::get_connection();

    try
    {
        auto sv = row_schema_version::get_rows(*conn);

        if (sv.size() == 0)
        {
            THROW(schema_manager_exception, "schema version table is empty");
        }
        else if (sv.size() > 1)
        {
            THROW(schema_manager_exception, "schema version table contains more than one row");
        }
        else
        {
            update(*conn, sv[0]->get_version_string());
        }
    }
    catch (row_table_doesnt_exist_exception &e)
    {
        // no schema at all, create it
        create(*conn);
    }
}

void schema_manager::create(connection &conn)
{
    LOG(info) << "first use, creating schema";

    for (auto tsql : table::get_table_sql(conn.m_db_type))
    {
        switch (conn.m_db_type)
        {
            case mysql_e:
                mysql_row_helper::execute(conn, tsql.second);
                break;
            case sqlite_e:
                sqlite_row_helper::execute(conn, tsql.second);
                break;
            case mongodb_e:
                mongodb_row_helper::create_collection(conn, tsql.second);
                break;
            case postgres_e:
                postgres_row_helper::execute(conn, tsql.second);
                break;
            default:
                THROW(schema_manager_exception, "DB type not implemented", conn.m_db_type);
        }
    }

    table::create_static(conn);

    LOG(info) << "creating indices";
    for (auto tsql : table::get_index_sql(conn.m_db_type))
    {
        for (auto isql : tsql.second)
        {
            switch (conn.m_db_type)
            {
                case mysql_e:
                    mysql_row_helper::execute(conn, isql);
                    break;
                case sqlite_e:
                    sqlite_row_helper::execute(conn, isql);
                    break;
                case mongodb_e:
                    mongodb_row_helper::create_index(conn, isql);
                    break;
                case postgres_e:
                    postgres_row_helper::execute(conn, isql);
                    break;
                default:
                    THROW(schema_manager_exception, "DB type not implemented", conn.m_db_type);
            }
        }
    }

    LOG(info) << "creating foreign keys";
    for (auto tsql : table::get_foreign_key_sql(conn.m_db_type))
    {
        for (auto fksql : tsql.second)
        {
            switch (conn.m_db_type)
            {
                case mysql_e:
                    mysql_row_helper::execute(conn, fksql);
                    break;
                case sqlite_e:
                    sqlite_row_helper::execute(conn, fksql);
                    break;
                case mongodb_e:
                    // out of luck. Mongo support is experimental until
                    // it can be reworked to support consistency in the event
                    // of a failure. This will likely be solved by some careful
                    // ordering of operations followed by a consistency check
                    // at startup time.
                    break;
                case postgres_e:
                    postgres_row_helper::execute(conn, fksql);
                    break;
                default:
                    THROW(schema_manager_exception, "DB type not implemented", conn.m_db_type);
            }
        }
    }
}

void schema_manager::update(connection &conn, const string &v)
{
    if (v == "1")
    {
        // this is version 1 of the server software, so nothing to do
    }
    else
    {
        LOG(error) << "schema version is " << v << " which is not one this server software knows about";
        THROW(schema_manager_exception, "unrecognized schema version found", v);
    }
}
