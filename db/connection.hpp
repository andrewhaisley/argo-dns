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

#include <sqlite3.h>
#include <mysql_connection.h>
#include <mongoc.h>
#include <libpq-fe.h>

#include <map>
#include <set>
#include "exception.hpp"
#include "dbtypes.hpp"

EXCEPTION_CLASS(connection_exception, exception)

namespace adns
{
    namespace db
    {
        /**
         * wrapper for low level database connection objects. Connections are set to autocommit by default.
         */
        class connection final
        {
        public:

            friend class row;
            friend class mysql_row_helper;
            friend class sqlite_row_helper;
            friend class mongodb_row_helper;
            friend class postgres_row_helper;
            friend class schema_manager;

            /**
             * invalid connection
             */
            connection();

            /**
             * no copying
             */
            connection(const connection &other) = delete;

            /**
             * no assignment
             */
            connection &operator=(const connection &other) = delete;

            /**
             * connection
             */
            connection(
                    type_t            db_type,
                    const std::string &url, 
                    const std::string &username, 
                    const std::string &password, 
                    const std::string &database,
                    bool              use_transactions);

            /**
             * close and destroy
             */
            virtual ~connection();

            /**
             * commit the transaction
             */
            void commit();

            /**
             * rollback the transaction
             */
            void rollback();

            /**
             * close the connection
             */
            void close();

            /**
             * can this connection go stale? Stale connections will be discarded by the
             * connection pool.
             */
            bool can_go_stale() const;

            /**
             * begin transaction - sets autocommit to off
             */
            void begin_transaction();

            /**
             * the connection has been placed into the pool ready to be given out on request
             */
            void into_pool();

            /**
             * the connection has been taken out of the pool and given to a thread to use
             */
            void out_of_pool();

        private:

            // what type of database is this?
            type_t m_db_type;

            // mysql connection
            std::shared_ptr<sql::Connection> m_mysql_connection;

            // sqlite connection
            sqlite3 *m_sqlite_connection;

            // mongodb client    
            mongoc_client_t *m_mongodb_client;

            // mongodb database
            mongoc_database_t *m_mongodb_database;

            // mongodb session - ensures operations are done sequentially, and used for transactions.
            mongoc_client_session_t *m_mongodb_session;

            // postgres client
            PGconn *m_postgres_connection;

            // use transactions or not - if true, begin/commit/rollback do nothing. Some DBs support them
            // some don't (in different configurations)
            bool m_use_transactions;

            /**
             * if there's an open mongodb session, free it.
             */
            void close_mongodb_session();

            /**
             * get the mongodb session if there is one or throw an exception
             */
            mongoc_client_session_t *get_mongodb_session();
        };
    }
}
