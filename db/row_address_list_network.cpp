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

#include "row_address_list_network.hpp"

using namespace adns;
using namespace db;
using namespace std;

mutex row_address_list_network::o_lock;
vector<column> row_address_list_network::o_columns;
atomic<bool> row_address_list_network::o_initialised(false);
column row_address_list_network::o_primary_key;
vector<foreign_key> row_address_list_network::o_foreign_keys;

static row_address_list_network instance;

static shared_ptr<row_address_list_network> convert_row_type(row *r)
{
    return shared_ptr<row_address_list_network>(static_cast<row_address_list_network*>(r));
}

static vector<shared_ptr<row_address_list_network>> convert_row_type(vector<row *> vr)
{
    vector<shared_ptr<row_address_list_network>> res(vr.size());
    for (size_t i = 0; i < vr.size(); i++)
    {
        res[i] = shared_ptr<row_address_list_network>(static_cast<row_address_list_network*>(vr[i]));
    }
    return res;
}

    
row_address_list_network::row_address_list_network()
{
    
}

row_address_list_network::~row_address_list_network()
{
}

vector<shared_ptr<row_address_list_network>> row_address_list_network::get_rows(connection &conn)
{
    return convert_row_type(instance.fetch_rows(conn));
}

string row_address_list_network::get_table_name() const
{
    return "address_list_network";
}

table::table_t row_address_list_network::get_table_id() const
{
    return table::address_list_network_e;
}

row *row_address_list_network::create_instance() const
{
    return new row_address_list_network();
}

void row_address_list_network::set_column_value(const column &c, int value)
{
    (void)value;
    THROW(row_exception, "no columns of type int found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, bool value)
{
    (void)value;
    THROW(row_exception, "no columns of type bool found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, uint value)
{
    (void)value;
    THROW(row_exception, "no columns of type uint found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, octet value)
{
    (void)value;
    THROW(row_exception, "no columns of type octet found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, uuid value)
{
    switch (c.get_position())
    {
    case 1:
        m_address_list_id = value;
        break;
    case 2:
        m_address_list_network_id = value;
        break;
    default:
        THROW(row_exception, "no columns of type uuid found at position", c.get_position());
    }
}

void row_address_list_network::set_column_value(const column &c, const string &value)
{
    switch (c.get_position())
    {
    case 3:
        m_cidr_address = value;
        break;
    default:
        THROW(row_exception, "no columns of type string found at position", c.get_position());
    }
}

void row_address_list_network::set_column_value(const column &c, const datetime &value)
{
    (void)value;
    THROW(row_exception, "no columns of type datetime found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, const buffer &value)
{
    (void)value;
    THROW(row_exception, "no columns of type buffer found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, nullable<int> value)
{
    (void)value;
    THROW(row_exception, "no columns of type int found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, nullable<bool> value)
{
    (void)value;
    THROW(row_exception, "no columns of type bool found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, nullable<uint> value)
{
    (void)value;
    THROW(row_exception, "no columns of type uint found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, nullable<octet> value)
{
    (void)value;
    THROW(row_exception, "no columns of type octet found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, nullable<uuid> value)
{
    (void)value;
    THROW(row_exception, "no columns of type uuid found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, const nullable<string> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type string found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, const nullable<datetime> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type datetime found", c.get_position());
}

void row_address_list_network::set_column_value(const column &c, const nullable<buffer> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type buffer found", c.get_position());
}

int row_address_list_network::get_int_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type int found", c.get_name(), c.get_position());
}

bool row_address_list_network::get_bool_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type bool found", c.get_name(), c.get_position());
}

uint row_address_list_network::get_uint_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uint found", c.get_name(), c.get_position());
}

octet row_address_list_network::get_octet_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type octet found", c.get_name(), c.get_position());
}

uuid row_address_list_network::get_uuid_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 1:
        return m_address_list_id;
    case 2:
        return m_address_list_network_id;
    default:
        THROW(row_exception, "no columns of type uuid found at position", c.get_name(), c.get_position());
    }
}

const string &row_address_list_network::get_string_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 3:
        return m_cidr_address;
    default:
        THROW(row_exception, "no columns of type string found at position", c.get_name(), c.get_position());
    }
}

const datetime &row_address_list_network::get_datetime_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type datetime found", c.get_name(), c.get_position());
}

const buffer &row_address_list_network::get_buffer_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type buffer found", c.get_name(), c.get_position());
}

nullable<int> row_address_list_network::get_nullable_int_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type int found", c.get_name(), c.get_position());
}

nullable<bool> row_address_list_network::get_nullable_bool_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type bool found", c.get_name(), c.get_position());
}

nullable<uint> row_address_list_network::get_nullable_uint_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uint found", c.get_name(), c.get_position());
}

nullable<octet> row_address_list_network::get_nullable_octet_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type octet found", c.get_name(), c.get_position());
}

nullable<uuid> row_address_list_network::get_nullable_uuid_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uuid found", c.get_name(), c.get_position());
}

const nullable<string> &row_address_list_network::get_nullable_string_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type string found", c.get_name(), c.get_position());
}

const nullable<datetime> &row_address_list_network::get_nullable_datetime_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type datetime found", c.get_name(), c.get_position());
}

const nullable<buffer> &row_address_list_network::get_nullable_buffer_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type buffer found", c.get_name(), c.get_position());
}

const vector<column> &row_address_list_network::get_columns() const
{
    init_static();
    return o_columns;
}

const vector<foreign_key> &row_address_list_network::get_foreign_keys() const
{
    init_static();
    return o_foreign_keys;
}

const column &row_address_list_network::get_primary_key() const
{
    init_static();
    return o_primary_key;
}

uuid row_address_list_network::get_primary_key_value() const
{
    return get_uuid_column_value(get_primary_key());
}

void row_address_list_network::init_static()
{
    if (!o_initialised)
    {
        lock_guard<mutex> guard(o_lock);
        if (!o_initialised)
        {
            o_columns.push_back(column(column::uuid_e, "address_list_id", 1, false));
            o_columns.push_back(column(column::uuid_e, "address_list_network_id", 2, false));
            o_primary_key = column(column::uuid_e, "address_list_network_id", 2, false);
            o_columns.push_back(column(column::string_e, "cidr_address", 3, false));
            
            o_initialised = true;
        }
    }
}

shared_ptr<row_address_list_network> row_address_list_network::get_by_address_list_network_id(connection &c, uuid address_list_network_id)
{
    init_static();
    return convert_row_type(instance.fetch_row(c, o_columns[1], address_list_network_id));
}

vector<shared_ptr<row_address_list_network>> row_address_list_network::get_by_address_list_id(connection &c, uuid address_list_id)
{
    init_static();
    return convert_row_type(instance.fetch_rows(c, o_columns[0], address_list_id));
}



void row_address_list_network::delete_by_address_list_network_id(connection &c, uuid address_list_network_id)
{
    init_static();
    instance.delete_rows(c, o_columns[1], address_list_network_id);
}

void row_address_list_network::delete_by_address_list_id(connection &c, uuid address_list_id)
{
    init_static();
    instance.delete_rows(c, o_columns[0], address_list_id);
}



uuid row_address_list_network::get_address_list_id() const
{
    return m_address_list_id;
}

uuid row_address_list_network::get_address_list_network_id() const
{
    return m_address_list_network_id;
}

string row_address_list_network::get_cidr_address() const
{
    return m_cidr_address;
}



void row_address_list_network::set_address_list_id(uuid v)
{
    m_address_list_id = v;
}

void row_address_list_network::set_address_list_network_id(uuid v)
{
    m_address_list_network_id = v;
}

void row_address_list_network::set_cidr_address(string v)
{
    m_cidr_address = v;
}


