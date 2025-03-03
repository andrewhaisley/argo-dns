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
#include <mutex>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "types.hpp"
#include "connection.hpp"
#include "sqlite_row_helper.hpp"
#include "mongodb_row_helper.hpp"
#include "postgres_row_helper.hpp"

using namespace std;
using namespace adns;
using namespace db;
using namespace sql;

bool mongodb_initialized = false;
mutex mongodb_initialized_lock;

connection::connection() : m_db_type(invalid_e), m_sqlite_connection(nullptr), m_mongodb_client(nullptr), m_mongodb_database(nullptr), m_mongodb_session(nullptr), m_postgres_connection(nullptr)
{
}

connection::~connection()
{
    if (m_sqlite_connection)
    {
        sqlite3_close(m_sqlite_connection);
        m_sqlite_connection = nullptr;
    }

    close_mongodb_session();

    if (m_mongodb_database)
    {
        mongoc_database_destroy(m_mongodb_database);
        m_mongodb_database = nullptr;
    }

    if (m_mongodb_client)
    {
        mongoc_client_destroy(m_mongodb_client);
        m_mongodb_client = nullptr;
    } 

    if (m_postgres_connection)
    {
        PQfinish(m_postgres_connection);
        m_postgres_connection = nullptr;
    }
}

void connection::begin_transaction()
{
    if (!m_use_transactions)
    {
        return;
    }

    switch (m_db_type)
    {
        case mysql_e:
            m_mysql_connection->setAutoCommit(false);
            break;
        case sqlite_e:
            sqlite_row_helper::begin_transaction(*this);
            break;
        case mongodb_e:
            mongodb_row_helper::begin_transaction(*this);
            break;
        case postgres_e:
            postgres_row_helper::begin_transaction(*this);
            break;
        default:
            THROW(connection_exception, "unimplemented DB type in begin_transaction", m_db_type);
     }
}

connection::connection(
        type_t       db_type,
        const string &url, 
        const string &username, 
        const string &password, 
        const string &database,
        bool         use_transactions) : m_sqlite_connection(nullptr), m_mongodb_client(nullptr), m_mongodb_database(nullptr), m_mongodb_session(nullptr), m_postgres_connection(nullptr)
{
    m_db_type = db_type;
    m_use_transactions = use_transactions;

    switch (m_db_type)
    {
        case mysql_e:
            m_mysql_connection.reset(get_driver_instance()->connect(url, username, password));
            m_mysql_connection->setSchema(database);
            m_mysql_connection->setAutoCommit(true);
            break;
        case sqlite_e:
            {
                int rc = sqlite3_open_v2(url.c_str(), &m_sqlite_connection, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
                if (rc)
            {
                    THROW(connection_exception, sqlite3_errmsg(m_sqlite_connection));
                    sqlite3_close(m_sqlite_connection);
                }
            }
            break;
        case mongodb_e:
            {
                bson_error_t error;

                std::unique_lock<std::mutex> guard(mongodb_initialized_lock);

                if (!mongodb_initialized)
                {
                    mongodb_initialized = true;
                    mongoc_init();
                }

                auto uri = mongoc_uri_new_with_error(url.c_str(), &error);

                if (!uri) 
                {
                    THROW(connection_exception, error.message, url);
                }

                m_mongodb_client = mongoc_client_new_from_uri(uri);

                if (!m_mongodb_client)
                {
                    mongoc_uri_destroy(uri);
                    THROW(connection_exception, "mongoc_client_new_from_uri returned NULL");
                }

                mongoc_client_set_appname(m_mongodb_client, VERSION);
                mongoc_uri_destroy(uri);
        
                m_mongodb_database = mongoc_client_get_database(m_mongodb_client, database.c_str());

                if (!m_mongodb_database)
                {
                    THROW(connection_exception, "mongoc_client_get_database returned NULL");
                }
            }
            break;
        case postgres_e:
            {
                m_postgres_connection = PQconnectdb(url.c_str());
                if (PQstatus(m_postgres_connection) != CONNECTION_OK)
                {
                    auto err = postgres_row_helper::error_string(m_postgres_connection);
                    PQfinish(m_postgres_connection);
                    m_postgres_connection = nullptr;
                    THROW(connection_exception, "PQconnectdb failed", err);
                }
            }
            break;
        default:
            THROW(connection_exception, "unimplemented DB type in connect", m_db_type);
     }
}

void connection::close()
{
    switch (m_db_type)
    {
        case mysql_e:
            m_mysql_connection->close();
            m_mysql_connection = nullptr;
            break;
        case sqlite_e:
            sqlite3_close(m_sqlite_connection);
            m_sqlite_connection = nullptr;
            break;
        case mongodb_e:
            mongoc_database_destroy(m_mongodb_database);
            mongoc_client_destroy(m_mongodb_client);
            m_mongodb_database = nullptr;
            if (m_mongodb_session)
            {
                mongoc_client_session_destroy(m_mongodb_session);
                m_mongodb_session = nullptr;
            }
            m_mongodb_client = nullptr;
            break;
        case postgres_e:
            PQfinish(m_postgres_connection);
            m_postgres_connection = nullptr;
            break;
        default:
            THROW(connection_exception, "unimplemented DB type in close", m_db_type);
     }
}

void connection::rollback()
{
    if (!m_use_transactions)
    {
        return;
    }

    switch (m_db_type)
    {
        case mysql_e:
            m_mysql_connection->rollback();
            m_mysql_connection->setAutoCommit(true);
            break;
        case sqlite_e:
            sqlite_row_helper::rollback_transaction(*this);
            break;
        case mongodb_e:
            mongodb_row_helper::rollback_transaction(*this);
            break;
        case postgres_e:
            postgres_row_helper::rollback_transaction(*this);
            break;
        default:
            THROW(connection_exception, "unimplemented DB type in rollback", m_db_type);
     }
}

void connection::commit()
{
    if (!m_use_transactions)
    {
        return;
    }

    switch (m_db_type)
    {
        case mysql_e:
            m_mysql_connection->commit();
            m_mysql_connection->setAutoCommit(true);
            break;
        case sqlite_e:
            sqlite_row_helper::commit_transaction(*this);
            break;
        case mongodb_e:
            mongodb_row_helper::commit_transaction(*this);
            break;
        case postgres_e:
            postgres_row_helper::commit_transaction(*this);
            break;
        default:
            THROW(connection_exception, "unimplemented DB type in commit", m_db_type);
     }
}

bool connection::can_go_stale() const
{
    switch (m_db_type)
    {
        case mysql_e : 
            return true;
        case sqlite_e : 
            return false;
        case mongodb_e:
            return true;
        default:
            return false;
    }
}

void connection::close_mongodb_session()
{
    if (m_mongodb_session)
    {
        mongoc_client_session_destroy(m_mongodb_session);
        m_mongodb_session = nullptr;
    }
}

void connection::into_pool()
{
    close_mongodb_session();
}

void connection::out_of_pool()
{
    if (m_db_type == mongodb_e)
    {
        bson_error_t error;
        m_mongodb_session = mongoc_client_start_session (m_mongodb_client, nullptr, &error);
        if (!m_mongodb_session)
        {
            THROW(connection_exception, "mongodb session start failed", error.message);
        }
    }
}

mongoc_client_session_t *connection::get_mongodb_session()
{
    if (m_mongodb_session)
    {
        return m_mongodb_session;
    }
    else
    {
        THROW(connection_exception, "no current mongodb session exists");
    }
}
