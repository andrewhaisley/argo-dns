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
 * autogenerated by dbgen.py at 03:09PM on March 22, 2025, do not hand edit
 */

#include <boost/uuid/uuid_generators.hpp>

#include "row_rr_cname.hpp"

using namespace adns;
using namespace db;
using namespace std;

mutex row_rr_cname::o_lock;
vector<column> row_rr_cname::o_columns;
atomic<bool> row_rr_cname::o_initialised(false);
column row_rr_cname::o_primary_key;
vector<foreign_key> row_rr_cname::o_foreign_keys;

static row_rr_cname instance;

static shared_ptr<row_rr_cname> convert_row_type(row *r)
{
    return shared_ptr<row_rr_cname>(static_cast<row_rr_cname*>(r));
}

static vector<shared_ptr<row_rr_cname>> convert_row_type(vector<row *> vr)
{
    vector<shared_ptr<row_rr_cname>> res(vr.size());
    for (size_t i = 0; i < vr.size(); i++)
    {
        res[i] = shared_ptr<row_rr_cname>(static_cast<row_rr_cname*>(vr[i]));
    }
    return res;
}

    
row_rr_cname::row_rr_cname()
{
    m_ttl = 0;
}

row_rr_cname::~row_rr_cname()
{
}

vector<shared_ptr<row_rr_cname>> row_rr_cname::get_rows(connection &conn)
{
    return convert_row_type(instance.fetch_rows(conn));
}

string row_rr_cname::get_table_name() const
{
    return "rr_cname";
}

table::table_t row_rr_cname::get_table_id() const
{
    return table::rr_cname_e;
}

row *row_rr_cname::create_instance() const
{
    return new row_rr_cname();
}

void row_rr_cname::set_column_value(const column &c, int value)
{
    (void)value;
    THROW(row_exception, "no columns of type int found", c.get_position());
}

void row_rr_cname::set_column_value(const column &c, bool value)
{
    (void)value;
    THROW(row_exception, "no columns of type bool found", c.get_position());
}

void row_rr_cname::set_column_value(const column &c, uint value)
{
    switch (c.get_position())
    {
    case 4:
        m_ttl = value;
        break;
    default:
        THROW(row_exception, "no columns of type uint found at position", c.get_position());
    }
}

void row_rr_cname::set_column_value(const column &c, octet value)
{
    (void)value;
    THROW(row_exception, "no columns of type octet found", c.get_position());
}

void row_rr_cname::set_column_value(const column &c, uuid value)
{
    switch (c.get_position())
    {
    case 3:
        m_rr_cname_id = value;
        break;
    case 5:
        m_zone_id = value;
        break;
    default:
        THROW(row_exception, "no columns of type uuid found at position", c.get_position());
    }
}

void row_rr_cname::set_column_value(const column &c, const string &value)
{
    switch (c.get_position())
    {
    case 1:
        m_cname = value;
        break;
    case 2:
        m_name = value;
        break;
    default:
        THROW(row_exception, "no columns of type string found at position", c.get_position());
    }
}

void row_rr_cname::set_column_value(const column &c, const datetime &value)
{
    (void)value;
    THROW(row_exception, "no columns of type datetime found", c.get_position());
}

void row_rr_cname::set_column_value(const column &c, const buffer &value)
{
    (void)value;
    THROW(row_exception, "no columns of type buffer found", c.get_position());
}

void row_rr_cname::set_column_value(const column &c, nullable<int> value)
{
    (void)value;
    THROW(row_exception, "no columns of type int found", c.get_position());
}

void row_rr_cname::set_column_value(const column &c, nullable<bool> value)
{
    (void)value;
    THROW(row_exception, "no columns of type bool found", c.get_position());
}

void row_rr_cname::set_column_value(const column &c, nullable<uint> value)
{
    (void)value;
    THROW(row_exception, "no columns of type uint found", c.get_position());
}

void row_rr_cname::set_column_value(const column &c, nullable<octet> value)
{
    (void)value;
    THROW(row_exception, "no columns of type octet found", c.get_position());
}

void row_rr_cname::set_column_value(const column &c, nullable<uuid> value)
{
    (void)value;
    THROW(row_exception, "no columns of type uuid found", c.get_position());
}

void row_rr_cname::set_column_value(const column &c, const nullable<string> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type string found", c.get_position());
}

void row_rr_cname::set_column_value(const column &c, const nullable<datetime> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type datetime found", c.get_position());
}

void row_rr_cname::set_column_value(const column &c, const nullable<buffer> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type buffer found", c.get_position());
}

int row_rr_cname::get_int_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type int found", c.get_name(), c.get_position());
}

bool row_rr_cname::get_bool_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type bool found", c.get_name(), c.get_position());
}

uint row_rr_cname::get_uint_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 4:
        return m_ttl;
    default:
        THROW(row_exception, "no columns of type uint found at position", c.get_name(), c.get_position());
    }
}

octet row_rr_cname::get_octet_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type octet found", c.get_name(), c.get_position());
}

uuid row_rr_cname::get_uuid_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 3:
        return m_rr_cname_id;
    case 5:
        return m_zone_id;
    default:
        THROW(row_exception, "no columns of type uuid found at position", c.get_name(), c.get_position());
    }
}

const string &row_rr_cname::get_string_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 1:
        return m_cname;
    case 2:
        return m_name;
    default:
        THROW(row_exception, "no columns of type string found at position", c.get_name(), c.get_position());
    }
}

const datetime &row_rr_cname::get_datetime_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type datetime found", c.get_name(), c.get_position());
}

const buffer &row_rr_cname::get_buffer_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type buffer found", c.get_name(), c.get_position());
}

nullable<int> row_rr_cname::get_nullable_int_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type int found", c.get_name(), c.get_position());
}

nullable<bool> row_rr_cname::get_nullable_bool_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type bool found", c.get_name(), c.get_position());
}

nullable<uint> row_rr_cname::get_nullable_uint_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uint found", c.get_name(), c.get_position());
}

nullable<octet> row_rr_cname::get_nullable_octet_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type octet found", c.get_name(), c.get_position());
}

nullable<uuid> row_rr_cname::get_nullable_uuid_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uuid found", c.get_name(), c.get_position());
}

const nullable<string> &row_rr_cname::get_nullable_string_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type string found", c.get_name(), c.get_position());
}

const nullable<datetime> &row_rr_cname::get_nullable_datetime_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type datetime found", c.get_name(), c.get_position());
}

const nullable<buffer> &row_rr_cname::get_nullable_buffer_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type buffer found", c.get_name(), c.get_position());
}

const vector<column> &row_rr_cname::get_columns() const
{
    init_static();
    return o_columns;
}

const vector<foreign_key> &row_rr_cname::get_foreign_keys() const
{
    init_static();
    return o_foreign_keys;
}

const column &row_rr_cname::get_primary_key() const
{
    init_static();
    return o_primary_key;
}

uuid row_rr_cname::get_primary_key_value() const
{
    return get_uuid_column_value(get_primary_key());
}

void row_rr_cname::init_static()
{
    if (!o_initialised)
    {
        lock_guard<mutex> guard(o_lock);
        if (!o_initialised)
        {
            o_columns.push_back(column(column::string_e, "cname", 1, false));
            o_columns.push_back(column(column::string_e, "name", 2, false));
            o_columns.push_back(column(column::uuid_e, "rr_cname_id", 3, false));
            o_primary_key = column(column::uuid_e, "rr_cname_id", 3, false);
            o_columns.push_back(column(column::uint_e, "ttl", 4, false));
            o_columns.push_back(column(column::uuid_e, "zone_id", 5, false));
            
            o_initialised = true;
        }
    }
}

shared_ptr<row_rr_cname> row_rr_cname::get_by_rr_cname_id(connection &c, uuid rr_cname_id)
{
    init_static();
    return convert_row_type(instance.fetch_row(c, o_columns[2], rr_cname_id));
}

vector<shared_ptr<row_rr_cname>> row_rr_cname::get_by_zone_id(connection &c, uuid zone_id)
{
    init_static();
    return convert_row_type(instance.fetch_rows(c, o_columns[4], zone_id));
}



void row_rr_cname::delete_by_rr_cname_id(connection &c, uuid rr_cname_id)
{
    init_static();
    instance.delete_rows(c, o_columns[2], rr_cname_id);
}

void row_rr_cname::delete_by_zone_id(connection &c, uuid zone_id)
{
    init_static();
    instance.delete_rows(c, o_columns[4], zone_id);
}



string row_rr_cname::get_cname() const
{
    return m_cname;
}

string row_rr_cname::get_name() const
{
    return m_name;
}

uuid row_rr_cname::get_rr_cname_id() const
{
    return m_rr_cname_id;
}

uint row_rr_cname::get_ttl() const
{
    return m_ttl;
}

uuid row_rr_cname::get_zone_id() const
{
    return m_zone_id;
}



void row_rr_cname::set_cname(string v)
{
    m_cname = v;
}

void row_rr_cname::set_name(string v)
{
    m_name = v;
}

void row_rr_cname::set_rr_cname_id(uuid v)
{
    m_rr_cname_id = v;
}

void row_rr_cname::set_ttl(uint v)
{
    m_ttl = v;
}

void row_rr_cname::set_zone_id(uuid v)
{
    m_zone_id = v;
}


