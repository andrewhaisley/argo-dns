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

#include <string>
#include <vector>
#include <map>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/datatype.h>

#include "mysql_connection.h"

#include "types.hpp"
#include "nullable.hpp"
#include "column.hpp"
#include "foreign_key.hpp"
#include "datetime.hpp"
#include "exception.hpp"
#include "buffer.hpp"
#include "connection.hpp"
#include "json_serializable.hpp"
#include "table.hpp"

EXCEPTION_CLASS(row_exception, exception)
EXCEPTION_CLASS(row_not_found_exception, row_exception)
EXCEPTION_CLASS(row_not_unique_exception, row_exception)
EXCEPTION_CLASS(row_references_exist_exception, row_exception)
EXCEPTION_CLASS(row_foreign_key_exception, row_exception)
EXCEPTION_CLASS(row_table_doesnt_exist_exception, row_exception)

namespace adns
{
    namespace db
    {
        class row : public json_serializable
        {
        public:
        
            friend class mysql_row_helper;
            friend class sqlite_row_helper;
            friend class mongodb_row_helper;
            
            /**
             * null row
             */
            row();

            /**
             * destroy the row - does nothing to the database
             */
            virtual ~row();

            /**
             * insert the row into the database
             */
            void insert_row(connection &conn);

            /**
             * update the row in the database
             */
            void update_row(connection &conn);

            /**
             * delete the row from the database
             */
            void delete_row(connection &conn);

            /**
             * no copy constructor, use shared_ptrs to pass rows around
             */
            row(const row &other) = delete;

            /**
             * no assignment, use shared_ptrs to pass rows around
             */
            row &operator=(const row &other) = delete;

            /**
             * dump out for debug
             */
            void dump() const;

            /**
             * must be implemented by all derived classes
             */
            virtual std::string get_table_name() const = 0;

            /**
             * must be implemented by all derived classes
             */
            virtual table::table_t get_table_id() const = 0;

            /**
             * get the primary key value
             */
            virtual uuid get_primary_key_value() const = 0;

            /**
             * set the primary key to a random UUID
             */
            virtual void set_new_primary_key_value();

            /**
             * must be implemented by all derived classes
             */
            virtual row *create_instance() const = 0;

            virtual void set_column_value(const column &c, int value) = 0;
            virtual void set_column_value(const column &c, bool value) = 0;
            virtual void set_column_value(const column &c, octet value) = 0;
            virtual void set_column_value(const column &c, uint value) = 0;
            virtual void set_column_value(const column &c, uuid value) = 0;
            virtual void set_column_value(const column &c, const std::string &value) = 0;
            virtual void set_column_value(const column &c, const datetime &value) = 0;
            virtual void set_column_value(const column &c, const buffer &value) = 0;

            virtual void set_column_value(const column &c, nullable<int> value) = 0;
            virtual void set_column_value(const column &c, nullable<bool> value) = 0;
            virtual void set_column_value(const column &c, nullable<octet> value) = 0;
            virtual void set_column_value(const column &c, nullable<uint> value) = 0;
            virtual void set_column_value(const column &c, nullable<uuid> value) = 0;
            virtual void set_column_value(const column &c, const nullable<std::string> &value) = 0;
            virtual void set_column_value(const column &c, const nullable<datetime> &value) = 0;
            virtual void set_column_value(const column &c, const nullable<buffer> &value) = 0;

            virtual int  get_int_column_value(const column &c) const = 0;
            virtual bool get_bool_column_value(const column &c) const = 0;
            virtual uint get_uint_column_value(const column &c) const = 0;
            virtual octet get_octet_column_value(const column &c) const = 0;
            virtual uuid get_uuid_column_value(const column &c) const = 0;

            virtual const std::string  &get_string_column_value(const column &c) const = 0;
            virtual const datetime     &get_datetime_column_value(const column &c) const = 0;
            virtual const buffer       &get_buffer_column_value(const column &c) const = 0;

            virtual nullable<int>   get_nullable_int_column_value(const column &c) const = 0;
            virtual nullable<bool>  get_nullable_bool_column_value(const column &c) const = 0;
            virtual nullable<uint>  get_nullable_uint_column_value(const column &c) const = 0;
            virtual nullable<octet> get_nullable_octet_column_value(const column &c) const = 0;
            virtual nullable<uuid>  get_nullable_uuid_column_value(const column &c) const = 0;

            virtual const nullable<std::string> &get_nullable_string_column_value(const column &c) const = 0;
            virtual const nullable<datetime>    &get_nullable_datetime_column_value(const column &c) const = 0;
            virtual const nullable<buffer>      &get_nullable_buffer_column_value(const column &c) const = 0;

            /**
             * get a list of column definitions
             */
            virtual const std::vector<column> &get_columns() const = 0;

            /**
             * get the primary key column
             */
            virtual const column &get_primary_key() const = 0;

            /**
             * get a list of foreign keys definitions for the table
             */
            virtual const std::vector<foreign_key> &get_foreign_keys() const = 0;

            /**
             * read a row from the db based on the supplied unique key value
             */
            row *fetch_row(connection &conn, const column &key_col, uuid key_val);

            /**
             * read rows from the db based on the supplied non-unique key value
             */
            std::vector<row *> fetch_rows(connection &conn, const column &key_col, uuid key_val);

            /**
             * read all rows from the db
             */
            std::vector<row *> fetch_rows(connection &conn);

            /**
             * delete rows from the db based on the supplied key value
             */
            void delete_rows(connection &conn, const column &key_col, uuid key_val);

            /**
             * turn into JSON
             */
            virtual void json_serialize() const;

            /**
             * get from JSON
             */
            virtual void json_unserialize();

            // document/row ID for databases that need it
            std::string m_document_id;
        };
    }
}
