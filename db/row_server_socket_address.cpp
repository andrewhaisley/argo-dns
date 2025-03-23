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

#include "row_server_socket_address.hpp"

using namespace adns;
using namespace db;
using namespace std;

mutex row_server_socket_address::o_lock;
vector<column> row_server_socket_address::o_columns;
atomic<bool> row_server_socket_address::o_initialised(false);
column row_server_socket_address::o_primary_key;
vector<foreign_key> row_server_socket_address::o_foreign_keys;

static row_server_socket_address instance;

static shared_ptr<row_server_socket_address> convert_row_type(row *r)
{
    return shared_ptr<row_server_socket_address>(static_cast<row_server_socket_address*>(r));
}

static vector<shared_ptr<row_server_socket_address>> convert_row_type(vector<row *> vr)
{
    vector<shared_ptr<row_server_socket_address>> res(vr.size());
    for (size_t i = 0; i < vr.size(); i++)
    {
        res[i] = shared_ptr<row_server_socket_address>(static_cast<row_server_socket_address*>(vr[i]));
    }
    return res;
}

    
row_server_socket_address::row_server_socket_address()
{
    m_port = 0;
}

row_server_socket_address::~row_server_socket_address()
{
}

vector<shared_ptr<row_server_socket_address>> row_server_socket_address::get_rows(connection &conn)
{
    return convert_row_type(instance.fetch_rows(conn));
}

string row_server_socket_address::get_table_name() const
{
    return "server_socket_address";
}

table::table_t row_server_socket_address::get_table_id() const
{
    return table::server_socket_address_e;
}

row *row_server_socket_address::create_instance() const
{
    return new row_server_socket_address();
}

void row_server_socket_address::set_column_value(const column &c, int value)
{
    (void)value;
    THROW(row_exception, "no columns of type int found", c.get_position());
}

void row_server_socket_address::set_column_value(const column &c, bool value)
{
    (void)value;
    THROW(row_exception, "no columns of type bool found", c.get_position());
}

void row_server_socket_address::set_column_value(const column &c, uint value)
{
    switch (c.get_position())
    {
    case 2:
        m_port = value;
        break;
    default:
        THROW(row_exception, "no columns of type uint found at position", c.get_position());
    }
}

void row_server_socket_address::set_column_value(const column &c, octet value)
{
    (void)value;
    THROW(row_exception, "no columns of type octet found", c.get_position());
}

void row_server_socket_address::set_column_value(const column &c, uuid value)
{
    switch (c.get_position())
    {
    case 3:
        m_server_id = value;
        break;
    case 4:
        m_server_socket_address_id = value;
        break;
    default:
        THROW(row_exception, "no columns of type uuid found at position", c.get_position());
    }
}

void row_server_socket_address::set_column_value(const column &c, const string &value)
{
    switch (c.get_position())
    {
    case 1:
        m_ip_address = value;
        break;
    default:
        THROW(row_exception, "no columns of type string found at position", c.get_position());
    }
}

void row_server_socket_address::set_column_value(const column &c, const datetime &value)
{
    (void)value;
    THROW(row_exception, "no columns of type datetime found", c.get_position());
}

void row_server_socket_address::set_column_value(const column &c, const buffer &value)
{
    (void)value;
    THROW(row_exception, "no columns of type buffer found", c.get_position());
}

void row_server_socket_address::set_column_value(const column &c, nullable<int> value)
{
    (void)value;
    THROW(row_exception, "no columns of type int found", c.get_position());
}

void row_server_socket_address::set_column_value(const column &c, nullable<bool> value)
{
    (void)value;
    THROW(row_exception, "no columns of type bool found", c.get_position());
}

void row_server_socket_address::set_column_value(const column &c, nullable<uint> value)
{
    (void)value;
    THROW(row_exception, "no columns of type uint found", c.get_position());
}

void row_server_socket_address::set_column_value(const column &c, nullable<octet> value)
{
    (void)value;
    THROW(row_exception, "no columns of type octet found", c.get_position());
}

void row_server_socket_address::set_column_value(const column &c, nullable<uuid> value)
{
    (void)value;
    THROW(row_exception, "no columns of type uuid found", c.get_position());
}

void row_server_socket_address::set_column_value(const column &c, const nullable<string> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type string found", c.get_position());
}

void row_server_socket_address::set_column_value(const column &c, const nullable<datetime> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type datetime found", c.get_position());
}

void row_server_socket_address::set_column_value(const column &c, const nullable<buffer> &value)
{
    (void)value;
    THROW(row_exception, "no columns of type buffer found", c.get_position());
}

int row_server_socket_address::get_int_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type int found", c.get_name(), c.get_position());
}

bool row_server_socket_address::get_bool_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type bool found", c.get_name(), c.get_position());
}

uint row_server_socket_address::get_uint_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 2:
        return m_port;
    default:
        THROW(row_exception, "no columns of type uint found at position", c.get_name(), c.get_position());
    }
}

octet row_server_socket_address::get_octet_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type octet found", c.get_name(), c.get_position());
}

uuid row_server_socket_address::get_uuid_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 3:
        return m_server_id;
    case 4:
        return m_server_socket_address_id;
    default:
        THROW(row_exception, "no columns of type uuid found at position", c.get_name(), c.get_position());
    }
}

const string &row_server_socket_address::get_string_column_value(const column &c) const
{
    switch (c.get_position())
    {
    case 1:
        return m_ip_address;
    default:
        THROW(row_exception, "no columns of type string found at position", c.get_name(), c.get_position());
    }
}

const datetime &row_server_socket_address::get_datetime_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type datetime found", c.get_name(), c.get_position());
}

const buffer &row_server_socket_address::get_buffer_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type buffer found", c.get_name(), c.get_position());
}

nullable<int> row_server_socket_address::get_nullable_int_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type int found", c.get_name(), c.get_position());
}

nullable<bool> row_server_socket_address::get_nullable_bool_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type bool found", c.get_name(), c.get_position());
}

nullable<uint> row_server_socket_address::get_nullable_uint_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uint found", c.get_name(), c.get_position());
}

nullable<octet> row_server_socket_address::get_nullable_octet_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type octet found", c.get_name(), c.get_position());
}

nullable<uuid> row_server_socket_address::get_nullable_uuid_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type uuid found", c.get_name(), c.get_position());
}

const nullable<string> &row_server_socket_address::get_nullable_string_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type string found", c.get_name(), c.get_position());
}

const nullable<datetime> &row_server_socket_address::get_nullable_datetime_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type datetime found", c.get_name(), c.get_position());
}

const nullable<buffer> &row_server_socket_address::get_nullable_buffer_column_value(const column &c) const
{
    THROW(row_exception, "no columns of type buffer found", c.get_name(), c.get_position());
}

const vector<column> &row_server_socket_address::get_columns() const
{
    init_static();
    return o_columns;
}

const vector<foreign_key> &row_server_socket_address::get_foreign_keys() const
{
    init_static();
    return o_foreign_keys;
}

const column &row_server_socket_address::get_primary_key() const
{
    init_static();
    return o_primary_key;
}

uuid row_server_socket_address::get_primary_key_value() const
{
    return get_uuid_column_value(get_primary_key());
}

void row_server_socket_address::init_static()
{
    if (!o_initialised)
    {
        lock_guard<mutex> guard(o_lock);
        if (!o_initialised)
        {
            o_columns.push_back(column(column::string_e, "ip_address", 1, false));
            o_columns.push_back(column(column::uint_e, "port", 2, false));
            o_columns.push_back(column(column::uuid_e, "server_id", 3, false));
            o_columns.push_back(column(column::uuid_e, "server_socket_address_id", 4, false));
            o_primary_key = column(column::uuid_e, "server_socket_address_id", 4, false);
            
            o_initialised = true;
        }
    }
}

shared_ptr<row_server_socket_address> row_server_socket_address::get_by_server_socket_address_id(connection &c, uuid server_socket_address_id)
{
    init_static();
    return convert_row_type(instance.fetch_row(c, o_columns[3], server_socket_address_id));
}

vector<shared_ptr<row_server_socket_address>> row_server_socket_address::get_by_server_id(connection &c, uuid server_id)
{
    init_static();
    return convert_row_type(instance.fetch_rows(c, o_columns[2], server_id));
}



void row_server_socket_address::delete_by_server_socket_address_id(connection &c, uuid server_socket_address_id)
{
    init_static();
    instance.delete_rows(c, o_columns[3], server_socket_address_id);
}

void row_server_socket_address::delete_by_server_id(connection &c, uuid server_id)
{
    init_static();
    instance.delete_rows(c, o_columns[2], server_id);
}



string row_server_socket_address::get_ip_address() const
{
    return m_ip_address;
}

uint row_server_socket_address::get_port() const
{
    return m_port;
}

uuid row_server_socket_address::get_server_id() const
{
    return m_server_id;
}

uuid row_server_socket_address::get_server_socket_address_id() const
{
    return m_server_socket_address_id;
}



void row_server_socket_address::set_ip_address(string v)
{
    m_ip_address = v;
}

void row_server_socket_address::set_port(uint v)
{
    m_port = v;
}

void row_server_socket_address::set_server_id(uuid v)
{
    m_server_id = v;
}

void row_server_socket_address::set_server_socket_address_id(uuid v)
{
    m_server_socket_address_id = v;
}


