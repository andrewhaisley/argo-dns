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
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>

#include "row.hpp"
#include "unparser.hpp"
#include "mysql_row_helper.hpp"
#include "sqlite_row_helper.hpp"
#include "mongodb_row_helper.hpp"
#include "postgres_row_helper.hpp"
#include "util.hpp"

using namespace std;
using namespace sql;
using namespace adns;
using namespace db;

row::row()
{
}

row::~row()
{
}

void row::set_new_primary_key_value()
{
    set_column_value(get_primary_key(), boost::uuids::random_generator()());
}

void row::insert_row(connection &conn)
{
    switch (conn.m_db_type)
    {
        case mysql_e:
            mysql_row_helper::insert_row(conn, *this);
            break;
        case sqlite_e:
            sqlite_row_helper::insert_row(conn, *this);
            break;
        case mongodb_e:
            mongodb_row_helper::insert_row(conn, *this);
            break;
        case postgres_e:
            postgres_row_helper::insert_row(conn, *this);
            break;
        default:
            THROW(row_exception, "DB type not implemented", conn.m_db_type);
    }
}

void row::update_row(connection &conn)
{
    switch (conn.m_db_type)
    {
        case mysql_e:
            mysql_row_helper::update_row(conn, *this);
            break;
        case sqlite_e:
            sqlite_row_helper::update_row(conn, *this);
            break;
        case mongodb_e:
            mongodb_row_helper::update_row(conn, *this);
            break;
        case postgres_e:
            postgres_row_helper::update_row(conn, *this);
            break;
        default:
            THROW(row_exception, "DB type not implemented", conn.m_db_type);
    }
}

void row::delete_row(connection &conn)
{
    switch (conn.m_db_type)
    {
        case mysql_e:
            mysql_row_helper::delete_row(conn, *this);
            break;
        case sqlite_e:
            sqlite_row_helper::delete_row(conn, *this);
            break;
        case mongodb_e:
            mongodb_row_helper::delete_row(conn, *this);
            break;
        case postgres_e:
            postgres_row_helper::delete_row(conn, *this);
            break;
        default:
            THROW(row_exception, "DB type not implemented", conn.m_db_type);
    }
}

row *row::fetch_row(connection &conn, const column &c, uuid unique_key)
{
    switch (conn.m_db_type)
    {
        case mysql_e:
            return mysql_row_helper::fetch_row(conn, c, unique_key, *this);
        case sqlite_e:
            return sqlite_row_helper::fetch_row(conn, c, unique_key, *this);
        case mongodb_e:
            return mongodb_row_helper::fetch_row(conn, c, unique_key, *this);
        case postgres_e:
            return postgres_row_helper::fetch_row(conn, c, unique_key, *this);
        default:
            THROW(row_exception, "DB type not implemented", conn.m_db_type);
    }
}

vector<row *> row::fetch_rows(connection &conn, const column &c, uuid key)
{
    switch (conn.m_db_type)
    {
        case mysql_e:
            return mysql_row_helper::fetch_rows(conn, c, key, *this);
        case sqlite_e:
            return sqlite_row_helper::fetch_rows(conn, c, key, *this);
        case mongodb_e:
            return mongodb_row_helper::fetch_rows(conn, c, key, *this);
        case postgres_e:
            return postgres_row_helper::fetch_rows(conn, c, key, *this);
        default:
            THROW(row_exception, "DB type not implemented", conn.m_db_type);
    }
}

vector<row *> row::fetch_rows(connection &conn)
{
    switch (conn.m_db_type)
    {
        case mysql_e:
            return mysql_row_helper::fetch_rows(conn, *this);
        case sqlite_e:
            return sqlite_row_helper::fetch_rows(conn, *this);
        case mongodb_e:
            return mongodb_row_helper::fetch_rows(conn, *this);
        case postgres_e:
            return postgres_row_helper::fetch_rows(conn, *this);
        default:
            THROW(row_exception, "DB type not implemented", conn.m_db_type);
    }
}

void row::delete_rows(connection &conn, const column &c, uuid key)
{
    switch (conn.m_db_type)
    {
        case mysql_e:
            return mysql_row_helper::delete_rows(conn, c, key, *this);
        case sqlite_e:
            return sqlite_row_helper::delete_rows(conn, c, key, *this);
        case mongodb_e:
            return mongodb_row_helper::delete_rows(conn, c, key, *this);
        case postgres_e:
            return postgres_row_helper::delete_rows(conn, c, key, *this);
        default:
            THROW(row_exception, "DB type not implemented", conn.m_db_type);
    }
}

void row::dump() const
{
    for (auto c : get_columns())
    {
        if (c.get_nullable())
        {
            switch (c.get_type())
            {
            case column::bool_e:
                LOG(debug) << get_nullable_bool_column_value(c);
                break;
            case column::int_e:
                LOG(debug) << get_nullable_int_column_value(c);
                break;
            case column::uint_e:
                LOG(debug) << get_nullable_uint_column_value(c);
                break;
            case column::uuid_e:
                LOG(debug) << get_nullable_uuid_column_value(c);
                break;
            case column::datetime_e:
                LOG(debug) << get_nullable_datetime_column_value(c);
                break;
            case column::string_e:
                LOG(debug) << get_nullable_string_column_value(c);
                break;
            default:
                LOG(debug) << c.get_name() << " <invalid nullable type>";
            }
        }
        else
        {
            switch (c.get_type())
            {
            case column::bool_e:
                LOG(debug) << c.get_name() << " " << get_bool_column_value(c);
                break;
            case column::int_e:
                LOG(debug) << c.get_name() << " " << get_int_column_value(c);
                break;
            case column::uint_e:
                LOG(debug) << c.get_name() << " " << get_uint_column_value(c);
                break;
            case column::uuid_e:
                LOG(debug) << c.get_name() << " " << get_uuid_column_value(c);
                break;
            case column::datetime_e:
                LOG(debug) << c.get_name() << " " << get_datetime_column_value(c);
                break;
            case column::string_e:
                LOG(debug) << c.get_name() << " " << get_string_column_value(c);
                break;
            default:
                LOG(debug) << c.get_name() << " <invalid type>";
            }
        }
    }
}

void row::json_serialize() const
{
    for (auto c : get_columns())
    {
        if (c.get_nullable())
        {
            switch (c.get_type())
            {
            case column::bool_e:
                set_json(c.get_name(), get_nullable_bool_column_value(c));
                break;
            case column::int_e:
                set_json(c.get_name(), get_nullable_int_column_value(c));
                break;
            case column::uint_e:
                {
                    auto v = get_nullable_uint_column_value(c);
                    if (v.is_null())
                    {
                        set_json(c.get_name(), json());
                    }
                    else
                    {
                        set_json(c.get_name(), int(get_nullable_uint_column_value(c).value()));
                    }
                }
                break;
            case column::datetime_e:
                {
                    auto v = get_nullable_datetime_column_value(c);
                    if (v.is_null())
                    {
                        set_json(c.get_name(), json());
                    }
                    else
                    {
                        set_json(c.get_name(), v.value().to_string());
                    }
                }
                break;
            case column::uuid_e:
                {
                    auto v = get_nullable_uuid_column_value(c);
                    if (v.is_null())
                    {
                        set_json(c.get_name(), json());
                    }
                    else
                    {
                        set_json(c.get_name(), to_string(v.value()));
                    }
                }
                break;
            case column::string_e:
                set_json(c.get_name(), get_nullable_string_column_value(c));
                break;
            default:
                THROW(row_exception, "invalid column type", c.get_type());
            }
        }
        else
        {
            switch (c.get_type())
            {
            case column::bool_e:
                set_json(c.get_name(), get_bool_column_value(c));
                break;
            case column::int_e:
                set_json(c.get_name(), get_int_column_value(c));
                break;
            case column::uint_e:
                set_json(c.get_name(), int(get_uint_column_value(c)));
                break;
            case column::datetime_e:
                set_json(c.get_name(), get_datetime_column_value(c).to_string());
                break;
            case column::uuid_e:
                set_json(c.get_name(), to_string(get_uuid_column_value(c)));
                break;
            case column::string_e:
                set_json(c.get_name(), get_string_column_value(c));
                break;
            default:
                LOG(debug) << c.get_name() << " <invalid type>";
            }
        }
    }
}

void row::json_unserialize()
{
    for (auto c : get_columns())
    {
        switch (c.get_type())
        {
        case column::bool_e:
            set_column_value(c, static_cast<bool>((*m_json_object)[c.get_name()]));
            break;
        case column::int_e:
            set_column_value(c, static_cast<int>((*m_json_object)[c.get_name()]));
            break;
        case column::uint_e:
            set_column_value(c, static_cast<uint>(static_cast<int>((*m_json_object)[c.get_name()])));
            break;
        case column::datetime_e:
            set_column_value(c, datetime(static_cast<string>((*m_json_object)[c.get_name()])));
            break;
        case column::uuid_e:
            set_column_value(c, boost::lexical_cast<uuid>(static_cast<string>((*m_json_object)[c.get_name()])));
            break;
        case column::string_e:
            set_column_value(c, static_cast<string>((*m_json_object)[c.get_name()]));
            break;
        default:
            LOG(debug) << c.get_name() << " <invalid type>";
        }
    }
}
