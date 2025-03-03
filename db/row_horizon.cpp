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

#include "row_horizon.hpp"

using namespace adns;
using namespace db;
using namespace std;

mutex row_horizon::o_lock;
vector<column> row_horizon::o_columns;
atomic<bool> row_horizon::o_initialised(false);
column row_horizon::o_primary_key;
vector<foreign_key> row_horizon::o_foreign_keys;

static row_horizon instance;

static shared_ptr<row_horizon> convert_row_type(row *r)
{
    return shared_ptr<row_horizon>(static_cast<row_horizon*>(r));
}

static vector<shared_ptr<row_horizon>> convert_row_type(vector<row *> vr)
{
    vector<shared_ptr<row_horizon>> res(vr.size());
    for (size_t i = 0; i < vr.size(); i++)
    {
        res[i] = shared_ptr<row_horizon>(static_cast<row_horizon*>(vr[i]));
    }
    return res;
}

    
row_horizon::row_horizon()
{
    m_allow_recursion = false;
    m_check_order = 0;
}

row_horizon::~row_horizon()
{
}

vector<shared_ptr<row_horizon>> row_horizon::get_rows(connection &conn)
{
    return convert_row_type(instance.fetch_rows(conn));
}

string row_horizon::get_table_name() const
{
    return "horizon";
}

table::table_t row_horizon::get_table_id() const
{
    return table::horizon_e;
}

row *row_horizon::create_instance() const
{
    return new row_horizon();
}

void row_horizon::set_column_value(const column &c, int value)
{
    (void)value;
    THROW(row_exception, "no columns of type int found", c.get_position());
}

void row_horizon::set_column_value(const column &c, bool value)
{
    switch (c.get_position())
    {
    case 2:
        m_allow_recursion = value;
        break;
    default:
        THROW(row_exception, "no columns of type bool found at position", c.get_position());
    }
}

void row_horizon::set_column_value(const column &c, uint value)
{
    switch (c.get_position())
    {
    case 4:
        m_check_order = value;
        break;
    default:
        THROW(row_exception, "no columns of type uint found at position", c.get_position());
    }
}

void row_horizon::set_column_value(const column &c, octet value)
{
    (void)value;
    THROW(row_exception, "no columns of type octet found", c.get_position());
}

void row_horizon::set_column_value(const column &c, uuid value)
{
    switch (c.get_position())
    {
    case 1:
        m_address_list_id = value;
        break;
    case 3:
        m_horizon_id = value;
        break;
    default:
        THROW(row_exception, "no columns of type uuid found at position", c.get_position());
    }
}

void row_horizon::set_column_value(const column &c, const string &value)
{
    (void)value;
    THROW(row_exception, "no columns of type string found", c.get_position());
}

void row_horizon::set_column_value(const column &c, const datetime &value)
{
    (void)value;
    THROW(row_exception, "no columns of type datetime found", c.get_position());
}

void row_horizon::set_column_value(const column &c, const buffer &value)
{
    (void)value;
    THROW(row_exception, "no columns of type buffer found", c.get_position());
}

void row_horizon::set_column_value(const column &c, nullable<int> value)
{
    (void)value;
    THROW(row_exception, "no columns of type int found", c.get_position());
}

void row_horizon::set_column_value(const column &c, nullable<bool> value)
{
    (void)value;
    THROW(row_exception, "no columns of type bool found", c.get_position());
}

void row_horizon::set_column_value(const column &c, nullable<uint> value)
{
    (void)value;
    THROW(row_exception, "no columns of type uint found", c.get_position());
}

void row_horizon::set_column_value(const column &c, nullable<octet> value)
{
    (void)value;
    THROW(row_exception, "no columns of type octet found", c.get_position());
}

void row_horizon::set_column_value(const column &c, nullable<uuid> value)
{
    (void)value;
    THROW(row_exception, "no columns of type uuid found", c.get_position());
}

void row_horizon::set_column_value(const column &c, const nullable<string> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type string found", c.get_position());
}

void row_horizon::set_column_value(const column &c, const nullable<datetime> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type datetime found", c.get_position());
}

void row_horizon::set_column_value(const column &c, const nullable<buffer> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type buffer found", c.get_position());
}

int row_horizon::get_int_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type int found", c.get_name(), c.get_position());
}

bool row_horizon::get_bool_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 2:
        return m_allow_recursion;
    default:
        THROW(row_exception, "no columns of type bool found at position", c.get_name(), c.get_position());
    }
}

uint row_horizon::get_uint_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 4:
        return m_check_order;
    default:
        THROW(row_exception, "no columns of type uint found at position", c.get_name(), c.get_position());
    }
}

octet row_horizon::get_octet_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type octet found", c.get_name(), c.get_position());
}

uuid row_horizon::get_uuid_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 1:
        return m_address_list_id;
    case 3:
        return m_horizon_id;
    default:
        THROW(row_exception, "no columns of type uuid found at position", c.get_name(), c.get_position());
    }
}

const string &row_horizon::get_string_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type string found", c.get_name(), c.get_position());
}

const datetime &row_horizon::get_datetime_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type datetime found", c.get_name(), c.get_position());
}

const buffer &row_horizon::get_buffer_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type buffer found", c.get_name(), c.get_position());
}

nullable<int> row_horizon::get_nullable_int_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type int found", c.get_name(), c.get_position());
}

nullable<bool> row_horizon::get_nullable_bool_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type bool found", c.get_name(), c.get_position());
}

nullable<uint> row_horizon::get_nullable_uint_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uint found", c.get_name(), c.get_position());
}

nullable<octet> row_horizon::get_nullable_octet_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type octet found", c.get_name(), c.get_position());
}

nullable<uuid> row_horizon::get_nullable_uuid_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uuid found", c.get_name(), c.get_position());
}

const nullable<string> &row_horizon::get_nullable_string_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type string found", c.get_name(), c.get_position());
}

const nullable<datetime> &row_horizon::get_nullable_datetime_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type datetime found", c.get_name(), c.get_position());
}

const nullable<buffer> &row_horizon::get_nullable_buffer_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type buffer found", c.get_name(), c.get_position());
}

const vector<column> &row_horizon::get_columns() const
{
    init_static();
    return o_columns;
}

const vector<foreign_key> &row_horizon::get_foreign_keys() const
{
    init_static();
    return o_foreign_keys;
}

const column &row_horizon::get_primary_key() const
{
    init_static();
    return o_primary_key;
}

uuid row_horizon::get_primary_key_value() const
{
    return get_uuid_column_value(get_primary_key());
}

void row_horizon::init_static()
{
    if (!o_initialised)
    {
        lock_guard<mutex> guard(o_lock);
        if (!o_initialised)
        {
            o_columns.push_back(column(column::uuid_e, "address_list_id", 1, false));
            o_columns.push_back(column(column::bool_e, "allow_recursion", 2, false));
            o_columns.push_back(column(column::uuid_e, "horizon_id", 3, false));
            o_primary_key = column(column::uuid_e, "horizon_id", 3, false);
            o_columns.push_back(column(column::uint_e, "check_order", 4, false));
            
            o_initialised = true;
        }
    }
}

shared_ptr<row_horizon> row_horizon::get_by_horizon_id(connection &c, uuid horizon_id)
{
    init_static();
    return convert_row_type(instance.fetch_row(c, o_columns[2], horizon_id));
}

vector<shared_ptr<row_horizon>> row_horizon::get_by_address_list_id(connection &c, uuid address_list_id)
{
    init_static();
    return convert_row_type(instance.fetch_rows(c, o_columns[0], address_list_id));
}



void row_horizon::delete_by_horizon_id(connection &c, uuid horizon_id)
{
    init_static();
    instance.delete_rows(c, o_columns[2], horizon_id);
}

void row_horizon::delete_by_address_list_id(connection &c, uuid address_list_id)
{
    init_static();
    instance.delete_rows(c, o_columns[0], address_list_id);
}



uuid row_horizon::get_address_list_id() const
{
    return m_address_list_id;
}

bool row_horizon::get_allow_recursion() const
{
    return m_allow_recursion;
}

uuid row_horizon::get_horizon_id() const
{
    return m_horizon_id;
}

uint row_horizon::get_check_order() const
{
    return m_check_order;
}



void row_horizon::set_address_list_id(uuid v)
{
    m_address_list_id = v;
}

void row_horizon::set_allow_recursion(bool v)
{
    m_allow_recursion = v;
}

void row_horizon::set_horizon_id(uuid v)
{
    m_horizon_id = v;
}

void row_horizon::set_check_order(uint v)
{
    m_check_order = v;
}


