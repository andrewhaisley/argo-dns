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
