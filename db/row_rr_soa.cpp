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

#include "row_rr_soa.hpp"

using namespace adns;
using namespace db;
using namespace std;

mutex row_rr_soa::o_lock;
vector<column> row_rr_soa::o_columns;
atomic<bool> row_rr_soa::o_initialised(false);
column row_rr_soa::o_primary_key;
vector<foreign_key> row_rr_soa::o_foreign_keys;

static row_rr_soa instance;

static shared_ptr<row_rr_soa> convert_row_type(row *r)
{
    return shared_ptr<row_rr_soa>(static_cast<row_rr_soa*>(r));
}

static vector<shared_ptr<row_rr_soa>> convert_row_type(vector<row *> vr)
{
    vector<shared_ptr<row_rr_soa>> res(vr.size());
    for (size_t i = 0; i < vr.size(); i++)
    {
        res[i] = shared_ptr<row_rr_soa>(static_cast<row_rr_soa*>(vr[i]));
    }
    return res;
}

    
row_rr_soa::row_rr_soa()
{
    m_expire = 0;
    m_minimum = 0;
    m_refresh = 0;
    m_retry = 0;
    m_serial = 0;
    m_ttl = 0;
}

row_rr_soa::~row_rr_soa()
{
}

vector<shared_ptr<row_rr_soa>> row_rr_soa::get_rows(connection &conn)
{
    return convert_row_type(instance.fetch_rows(conn));
}

string row_rr_soa::get_table_name() const
{
    return "rr_soa";
}

table::table_t row_rr_soa::get_table_id() const
{
    return table::rr_soa_e;
}

row *row_rr_soa::create_instance() const
{
    return new row_rr_soa();
}

void row_rr_soa::set_column_value(const column &c, int value)
{
    switch (c.get_position())
    {
    case 1:
        m_expire = value;
        break;
    case 5:
        m_refresh = value;
        break;
    case 6:
        m_retry = value;
        break;
    default:
        THROW(row_exception, "no columns of type int found at position", c.get_position());
    }
}

void row_rr_soa::set_column_value(const column &c, bool value)
{
    (void)value;
    THROW(row_exception, "no columns of type bool found", c.get_position());
}

void row_rr_soa::set_column_value(const column &c, uint value)
{
    switch (c.get_position())
    {
    case 2:
        m_minimum = value;
        break;
    case 9:
        m_serial = value;
        break;
    case 10:
        m_ttl = value;
        break;
    default:
        THROW(row_exception, "no columns of type uint found at position", c.get_position());
    }
}

void row_rr_soa::set_column_value(const column &c, octet value)
{
    (void)value;
    THROW(row_exception, "no columns of type octet found", c.get_position());
}

void row_rr_soa::set_column_value(const column &c, uuid value)
{
    switch (c.get_position())
    {
    case 8:
        m_rr_soa_id = value;
        break;
    case 11:
        m_zone_id = value;
        break;
    default:
        THROW(row_exception, "no columns of type uuid found at position", c.get_position());
    }
}

void row_rr_soa::set_column_value(const column &c, const string &value)
{
    switch (c.get_position())
    {
    case 3:
        m_mname = value;
        break;
    case 4:
        m_name = value;
        break;
    case 7:
        m_rname = value;
        break;
    default:
        THROW(row_exception, "no columns of type string found at position", c.get_position());
    }
}

void row_rr_soa::set_column_value(const column &c, const datetime &value)
{
    (void)value;
    THROW(row_exception, "no columns of type datetime found", c.get_position());
}

void row_rr_soa::set_column_value(const column &c, const buffer &value)
{
    (void)value;
    THROW(row_exception, "no columns of type buffer found", c.get_position());
}

void row_rr_soa::set_column_value(const column &c, nullable<int> value)
{
    (void)value;
    THROW(row_exception, "no columns of type int found", c.get_position());
}

void row_rr_soa::set_column_value(const column &c, nullable<bool> value)
{
    (void)value;
    THROW(row_exception, "no columns of type bool found", c.get_position());
}

void row_rr_soa::set_column_value(const column &c, nullable<uint> value)
{
    (void)value;
    THROW(row_exception, "no columns of type uint found", c.get_position());
}

void row_rr_soa::set_column_value(const column &c, nullable<octet> value)
{
    (void)value;
    THROW(row_exception, "no columns of type octet found", c.get_position());
}

void row_rr_soa::set_column_value(const column &c, nullable<uuid> value)
{
    (void)value;
    THROW(row_exception, "no columns of type uuid found", c.get_position());
}

void row_rr_soa::set_column_value(const column &c, const nullable<string> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type string found", c.get_position());
}

void row_rr_soa::set_column_value(const column &c, const nullable<datetime> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type datetime found", c.get_position());
}

void row_rr_soa::set_column_value(const column &c, const nullable<buffer> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type buffer found", c.get_position());
}

int row_rr_soa::get_int_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 1:
        return m_expire;
    case 5:
        return m_refresh;
    case 6:
        return m_retry;
    default:
        THROW(row_exception, "no columns of type int found at position", c.get_name(), c.get_position());
    }
}

bool row_rr_soa::get_bool_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type bool found", c.get_name(), c.get_position());
}

uint row_rr_soa::get_uint_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 2:
        return m_minimum;
    case 9:
        return m_serial;
    case 10:
        return m_ttl;
    default:
        THROW(row_exception, "no columns of type uint found at position", c.get_name(), c.get_position());
    }
}

octet row_rr_soa::get_octet_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type octet found", c.get_name(), c.get_position());
}

uuid row_rr_soa::get_uuid_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 8:
        return m_rr_soa_id;
    case 11:
        return m_zone_id;
    default:
        THROW(row_exception, "no columns of type uuid found at position", c.get_name(), c.get_position());
    }
}

const string &row_rr_soa::get_string_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 3:
        return m_mname;
    case 4:
        return m_name;
    case 7:
        return m_rname;
    default:
        THROW(row_exception, "no columns of type string found at position", c.get_name(), c.get_position());
    }
}

const datetime &row_rr_soa::get_datetime_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type datetime found", c.get_name(), c.get_position());
}

const buffer &row_rr_soa::get_buffer_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type buffer found", c.get_name(), c.get_position());
}

nullable<int> row_rr_soa::get_nullable_int_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type int found", c.get_name(), c.get_position());
}

nullable<bool> row_rr_soa::get_nullable_bool_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type bool found", c.get_name(), c.get_position());
}

nullable<uint> row_rr_soa::get_nullable_uint_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uint found", c.get_name(), c.get_position());
}

nullable<octet> row_rr_soa::get_nullable_octet_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type octet found", c.get_name(), c.get_position());
}

nullable<uuid> row_rr_soa::get_nullable_uuid_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uuid found", c.get_name(), c.get_position());
}

const nullable<string> &row_rr_soa::get_nullable_string_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type string found", c.get_name(), c.get_position());
}

const nullable<datetime> &row_rr_soa::get_nullable_datetime_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type datetime found", c.get_name(), c.get_position());
}

const nullable<buffer> &row_rr_soa::get_nullable_buffer_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type buffer found", c.get_name(), c.get_position());
}

const vector<column> &row_rr_soa::get_columns() const
{
    init_static();
    return o_columns;
}

const vector<foreign_key> &row_rr_soa::get_foreign_keys() const
{
    init_static();
    return o_foreign_keys;
}

const column &row_rr_soa::get_primary_key() const
{
    init_static();
    return o_primary_key;
}

uuid row_rr_soa::get_primary_key_value() const
{
    return get_uuid_column_value(get_primary_key());
}

void row_rr_soa::init_static()
{
    if (!o_initialised)
    {
        lock_guard<mutex> guard(o_lock);
        if (!o_initialised)
        {
            o_columns.push_back(column(column::int_e, "expire", 1, false));
            o_columns.push_back(column(column::uint_e, "minimum", 2, false));
            o_columns.push_back(column(column::string_e, "mname", 3, false));
            o_columns.push_back(column(column::string_e, "name", 4, false));
            o_columns.push_back(column(column::int_e, "refresh", 5, false));
            o_columns.push_back(column(column::int_e, "retry", 6, false));
            o_columns.push_back(column(column::string_e, "rname", 7, false));
            o_columns.push_back(column(column::uuid_e, "rr_soa_id", 8, false));
            o_primary_key = column(column::uuid_e, "rr_soa_id", 8, false);
            o_columns.push_back(column(column::uint_e, "serial", 9, false));
            o_columns.push_back(column(column::uint_e, "ttl", 10, false));
            o_columns.push_back(column(column::uuid_e, "zone_id", 11, false));
            
            o_initialised = true;
        }
    }
}

shared_ptr<row_rr_soa> row_rr_soa::get_by_rr_soa_id(connection &c, uuid rr_soa_id)
{
    init_static();
    return convert_row_type(instance.fetch_row(c, o_columns[7], rr_soa_id));
}

vector<shared_ptr<row_rr_soa>> row_rr_soa::get_by_zone_id(connection &c, uuid zone_id)
{
    init_static();
    return convert_row_type(instance.fetch_rows(c, o_columns[10], zone_id));
}



void row_rr_soa::delete_by_rr_soa_id(connection &c, uuid rr_soa_id)
{
    init_static();
    instance.delete_rows(c, o_columns[7], rr_soa_id);
}

void row_rr_soa::delete_by_zone_id(connection &c, uuid zone_id)
{
    init_static();
    instance.delete_rows(c, o_columns[10], zone_id);
}



int row_rr_soa::get_expire() const
{
    return m_expire;
}

uint row_rr_soa::get_minimum() const
{
    return m_minimum;
}

string row_rr_soa::get_mname() const
{
    return m_mname;
}

string row_rr_soa::get_name() const
{
    return m_name;
}

int row_rr_soa::get_refresh() const
{
    return m_refresh;
}

int row_rr_soa::get_retry() const
{
    return m_retry;
}

string row_rr_soa::get_rname() const
{
    return m_rname;
}

uuid row_rr_soa::get_rr_soa_id() const
{
    return m_rr_soa_id;
}

uint row_rr_soa::get_serial() const
{
    return m_serial;
}

uint row_rr_soa::get_ttl() const
{
    return m_ttl;
}

uuid row_rr_soa::get_zone_id() const
{
    return m_zone_id;
}



void row_rr_soa::set_expire(int v)
{
    m_expire = v;
}

void row_rr_soa::set_minimum(uint v)
{
    m_minimum = v;
}

void row_rr_soa::set_mname(string v)
{
    m_mname = v;
}

void row_rr_soa::set_name(string v)
{
    m_name = v;
}

void row_rr_soa::set_refresh(int v)
{
    m_refresh = v;
}

void row_rr_soa::set_retry(int v)
{
    m_retry = v;
}

void row_rr_soa::set_rname(string v)
{
    m_rname = v;
}

void row_rr_soa::set_rr_soa_id(uuid v)
{
    m_rr_soa_id = v;
}

void row_rr_soa::set_serial(uint v)
{
    m_serial = v;
}

void row_rr_soa::set_ttl(uint v)
{
    m_ttl = v;
}

void row_rr_soa::set_zone_id(uuid v)
{
    m_zone_id = v;
}


