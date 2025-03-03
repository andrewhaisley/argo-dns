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
/**
 * autogenerated by dbgen.py at 05:05PM on January 30, 2024, do not hand edit
 */

#include <boost/uuid/uuid_generators.hpp>

#include "row_zone.hpp"

using namespace adns;
using namespace db;
using namespace std;

mutex row_zone::o_lock;
vector<column> row_zone::o_columns;
atomic<bool> row_zone::o_initialised(false);
column row_zone::o_primary_key;
vector<foreign_key> row_zone::o_foreign_keys;

static row_zone instance;

static shared_ptr<row_zone> convert_row_type(row *r)
{
    return shared_ptr<row_zone>(static_cast<row_zone*>(r));
}

static vector<shared_ptr<row_zone>> convert_row_type(vector<row *> vr)
{
    vector<shared_ptr<row_zone>> res(vr.size());
    for (size_t i = 0; i < vr.size(); i++)
    {
        res[i] = shared_ptr<row_zone>(static_cast<row_zone*>(vr[i]));
    }
    return res;
}

    
row_zone::row_zone()
{
    m_is_forwarded = false;
}

row_zone::~row_zone()
{
}

vector<shared_ptr<row_zone>> row_zone::get_rows(connection &conn)
{
    return convert_row_type(instance.fetch_rows(conn));
}

string row_zone::get_table_name() const
{
    return "zone";
}

table::table_t row_zone::get_table_id() const
{
    return table::zone_e;
}

row *row_zone::create_instance() const
{
    return new row_zone();
}

void row_zone::set_column_value(const column &c, int value)
{
    (void)value;
    THROW(row_exception, "no columns of type int found", c.get_position());
}

void row_zone::set_column_value(const column &c, bool value)
{
    switch (c.get_position())
    {
    case 4:
        m_is_forwarded = value;
        break;
    default:
        THROW(row_exception, "no columns of type bool found at position", c.get_position());
    }
}

void row_zone::set_column_value(const column &c, uint value)
{
    (void)value;
    THROW(row_exception, "no columns of type uint found", c.get_position());
}

void row_zone::set_column_value(const column &c, octet value)
{
    (void)value;
    THROW(row_exception, "no columns of type octet found", c.get_position());
}

void row_zone::set_column_value(const column &c, uuid value)
{
    switch (c.get_position())
    {
    case 3:
        m_horizon_id = value;
        break;
    case 6:
        m_zone_id = value;
        break;
    default:
        THROW(row_exception, "no columns of type uuid found at position", c.get_position());
    }
}

void row_zone::set_column_value(const column &c, const string &value)
{
    switch (c.get_position())
    {
    case 5:
        m_name = value;
        break;
    default:
        THROW(row_exception, "no columns of type string found at position", c.get_position());
    }
}

void row_zone::set_column_value(const column &c, const datetime &value)
{
    (void)value;
    THROW(row_exception, "no columns of type datetime found", c.get_position());
}

void row_zone::set_column_value(const column &c, const buffer &value)
{
    (void)value;
    THROW(row_exception, "no columns of type buffer found", c.get_position());
}

void row_zone::set_column_value(const column &c, nullable<int> value)
{
    (void)value;
    THROW(row_exception, "no columns of type int found", c.get_position());
}

void row_zone::set_column_value(const column &c, nullable<bool> value)
{
    (void)value;
    THROW(row_exception, "no columns of type bool found", c.get_position());
}

void row_zone::set_column_value(const column &c, nullable<uint> value)
{
    switch (c.get_position())
    {
    case 2:
        m_forward_port = value;
        break;
    default:
        THROW(row_exception, "no columns of type uint found at position", c.get_position());
    }
}

void row_zone::set_column_value(const column &c, nullable<octet> value)
{
    (void)value;
    THROW(row_exception, "no columns of type octet found", c.get_position());
}

void row_zone::set_column_value(const column &c, nullable<uuid> value)
{
    (void)value;
    THROW(row_exception, "no columns of type uuid found", c.get_position());
}

void row_zone::set_column_value(const column &c, const nullable<string> &value)
{
    switch (c.get_position())
    {
    case 1:
        m_forward_ip_address = value;
        break;
    default:
        THROW(row_exception, "no columns of type string found at position", c.get_position());
    }
}

void row_zone::set_column_value(const column &c, const nullable<datetime> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type datetime found", c.get_position());
}

void row_zone::set_column_value(const column &c, const nullable<buffer> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type buffer found", c.get_position());
}

int row_zone::get_int_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type int found", c.get_name(), c.get_position());
}

bool row_zone::get_bool_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 4:
        return m_is_forwarded;
    default:
        THROW(row_exception, "no columns of type bool found at position", c.get_name(), c.get_position());
    }
}

uint row_zone::get_uint_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uint found", c.get_name(), c.get_position());
}

octet row_zone::get_octet_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type octet found", c.get_name(), c.get_position());
}

uuid row_zone::get_uuid_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 3:
        return m_horizon_id;
    case 6:
        return m_zone_id;
    default:
        THROW(row_exception, "no columns of type uuid found at position", c.get_name(), c.get_position());
    }
}

const string &row_zone::get_string_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 5:
        return m_name;
    default:
        THROW(row_exception, "no columns of type string found at position", c.get_name(), c.get_position());
    }
}

const datetime &row_zone::get_datetime_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type datetime found", c.get_name(), c.get_position());
}

const buffer &row_zone::get_buffer_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type buffer found", c.get_name(), c.get_position());
}

nullable<int> row_zone::get_nullable_int_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type int found", c.get_name(), c.get_position());
}

nullable<bool> row_zone::get_nullable_bool_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type bool found", c.get_name(), c.get_position());
}

nullable<uint> row_zone::get_nullable_uint_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 2:
        return m_forward_port;
    default:
        THROW(row_exception, "no columns of type uint found at position", c.get_name(), c.get_position());
    }
}

nullable<octet> row_zone::get_nullable_octet_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type octet found", c.get_name(), c.get_position());
}

nullable<uuid> row_zone::get_nullable_uuid_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uuid found", c.get_name(), c.get_position());
}

const nullable<string> &row_zone::get_nullable_string_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 1:
        return m_forward_ip_address;
    default:
        THROW(row_exception, "no columns of type string found at position", c.get_name(), c.get_position());
    }
}

const nullable<datetime> &row_zone::get_nullable_datetime_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type datetime found", c.get_name(), c.get_position());
}

const nullable<buffer> &row_zone::get_nullable_buffer_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type buffer found", c.get_name(), c.get_position());
}

const vector<column> &row_zone::get_columns() const
{
    init_static();
    return o_columns;
}

const vector<foreign_key> &row_zone::get_foreign_keys() const
{
    init_static();
    return o_foreign_keys;
}

const column &row_zone::get_primary_key() const
{
    init_static();
    return o_primary_key;
}

uuid row_zone::get_primary_key_value() const
{
    return get_uuid_column_value(get_primary_key());
}

void row_zone::init_static()
{
    if (!o_initialised)
    {
        lock_guard<mutex> guard(o_lock);
        if (!o_initialised)
        {
            o_columns.push_back(column(column::string_e, "forward_ip_address", 1, true));
            o_columns.push_back(column(column::uint_e, "forward_port", 2, true));
            o_columns.push_back(column(column::uuid_e, "horizon_id", 3, false));
            o_columns.push_back(column(column::bool_e, "is_forwarded", 4, false));
            o_columns.push_back(column(column::string_e, "name", 5, false));
            o_columns.push_back(column(column::uuid_e, "zone_id", 6, false));
            o_primary_key = column(column::uuid_e, "zone_id", 6, false);
            
            o_initialised = true;
        }
    }
}

shared_ptr<row_zone> row_zone::get_by_zone_id(connection &c, uuid zone_id)
{
    init_static();
    return convert_row_type(instance.fetch_row(c, o_columns[5], zone_id));
}

vector<shared_ptr<row_zone>> row_zone::get_by_horizon_id(connection &c, uuid horizon_id)
{
    init_static();
    return convert_row_type(instance.fetch_rows(c, o_columns[2], horizon_id));
}



void row_zone::delete_by_zone_id(connection &c, uuid zone_id)
{
    init_static();
    instance.delete_rows(c, o_columns[5], zone_id);
}

void row_zone::delete_by_horizon_id(connection &c, uuid horizon_id)
{
    init_static();
    instance.delete_rows(c, o_columns[2], horizon_id);
}



nullable<string> row_zone::get_forward_ip_address() const
{
    return m_forward_ip_address;
}

nullable<uint> row_zone::get_forward_port() const
{
    return m_forward_port;
}

uuid row_zone::get_horizon_id() const
{
    return m_horizon_id;
}

bool row_zone::get_is_forwarded() const
{
    return m_is_forwarded;
}

string row_zone::get_name() const
{
    return m_name;
}

uuid row_zone::get_zone_id() const
{
    return m_zone_id;
}



void row_zone::set_forward_ip_address(nullable<string> v)
{
    m_forward_ip_address = v;
}

void row_zone::set_forward_port(nullable<uint> v)
{
    m_forward_port = v;
}

void row_zone::set_horizon_id(uuid v)
{
    m_horizon_id = v;
}

void row_zone::set_is_forwarded(bool v)
{
    m_is_forwarded = v;
}

void row_zone::set_name(string v)
{
    m_name = v;
}

void row_zone::set_zone_id(uuid v)
{
    m_zone_id = v;
}


