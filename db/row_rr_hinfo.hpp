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

/**
 * autogenerated by dbgen.py at 05:05PM on January 30, 2024, do not hand edit
 */

#include <mutex>
#include <atomic>

#include "foreign_key.hpp"
#include "row.hpp"

namespace adns
{
    namespace db
    {
        class row_rr_hinfo final : public row
        {
        public:
    
            /**
             * invalid instance (until each member variable is set)
             */
            row_rr_hinfo();

            /**
             * destructor
             */
            virtual ~row_rr_hinfo();

            /**
             * get all rows from the table
             */
            static std::vector<std::shared_ptr<row_rr_hinfo>> get_rows(connection &conn);

            static std::shared_ptr<row_rr_hinfo> get_by_rr_hinfo_id(connection &c, uuid rr_hinfo_id);
            static std::vector<std::shared_ptr<row_rr_hinfo>> get_by_zone_id(connection &c, uuid zone_id);

            static void delete_by_rr_hinfo_id(connection &c, uuid rr_hinfo_id);
            static void delete_by_zone_id(connection &c, uuid zone_id);

            std::string get_cpu() const;
            std::string get_name() const;
            std::string get_os() const;
            uuid get_rr_hinfo_id() const;
            uint get_ttl() const;
            uuid get_zone_id() const;

            void set_cpu(std::string v);
            void set_name(std::string v);
            void set_os(std::string v);
            void set_rr_hinfo_id(uuid v);
            void set_ttl(uint v);
            void set_zone_id(uuid v);


            /**
             * name of the table in the DB
             */
            virtual std::string get_table_name() const;

            /**
             * id of the table in the DB
             */
            virtual table::table_t get_table_id() const;

            /**
             * get the primary key value
             */
            virtual uuid get_primary_key_value() const;

        protected:

            /**
             * create a blank instance ready for use
             */
            virtual row *create_instance() const;

            /**
             * dynamically set a column value
             */
            virtual void set_column_value(const column &c, int value);
            virtual void set_column_value(const column &c, bool value);
            virtual void set_column_value(const column &c, uint value);
            virtual void set_column_value(const column &c, octet value);
            virtual void set_column_value(const column &c, uuid value);
            virtual void set_column_value(const column &c, const std::string &value);
            virtual void set_column_value(const column &c, const datetime &value);
            virtual void set_column_value(const column &c, const buffer &value);

            virtual void set_column_value(const column &c, nullable<int> value);
            virtual void set_column_value(const column &c, nullable<bool> value);
            virtual void set_column_value(const column &c, nullable<uint> value);
            virtual void set_column_value(const column &c, nullable<octet> value);
            virtual void set_column_value(const column &c, nullable<uuid> value);
            virtual void set_column_value(const column &c, const nullable<std::string> &value);
            virtual void set_column_value(const column &c, const nullable<datetime> &value);
            virtual void set_column_value(const column &c, const nullable<buffer> &value);

            /**
             * dynamically get a column value
             */
            virtual int          get_int_column_value(const column &c) const;
            virtual bool         get_bool_column_value(const column &c) const;
            virtual octet        get_octet_column_value(const column &c) const;
            virtual uint         get_uint_column_value(const column &c) const;
            virtual uuid         get_uuid_column_value(const column &c) const;

            virtual const std::string  &get_string_column_value(const column &c) const;
            virtual const datetime     &get_datetime_column_value(const column &c) const;
            virtual const buffer       &get_buffer_column_value(const column &c) const;

            virtual nullable<int>          get_nullable_int_column_value(const column &c) const;
            virtual nullable<bool>         get_nullable_bool_column_value(const column &c) const;
            virtual nullable<uint>         get_nullable_uint_column_value(const column &c) const;
            virtual nullable<octet>        get_nullable_octet_column_value(const column &c) const;
            virtual nullable<uuid>         get_nullable_uuid_column_value(const column &c) const;

            const virtual nullable<std::string>  &get_nullable_string_column_value(const column &c) const;
            const virtual nullable<datetime>     &get_nullable_datetime_column_value(const column &c) const;
            const virtual nullable<buffer>       &get_nullable_buffer_column_value(const column &c) const;

            /**
             * get a list of column definitions
             */
            virtual const std::vector<column> &get_columns() const;

            /**
             * get a list of foreign key definitions
             */
            virtual const std::vector<foreign_key> &get_foreign_keys() const;

            /**
             * get the primary key column
             */
            virtual const column &get_primary_key() const;

        private:

            std::string m_cpu;
            std::string m_name;
            std::string m_os;
            uuid m_rr_hinfo_id;
            uint m_ttl;
            uuid m_zone_id;


            static std::atomic<bool> o_initialised;
            static std::mutex o_lock;
            static std::vector<column> o_columns;
            static std::vector<foreign_key> o_foreign_keys;
            static column o_primary_key;

            /**
             * initialise the static column list if needed
             */
            static void init_static();
        
        };
    }
}
