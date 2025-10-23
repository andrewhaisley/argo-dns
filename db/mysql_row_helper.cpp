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
 
#include <boost/lexical_cast.hpp>
#include "mysql_row_helper.hpp"
#include "row.hpp"

using namespace std;
using namespace adns;
using namespace db;
using namespace sql;

string mysql_row_helper::get_insert_placeholders_string(const row &r)
{
    string res;

    for (auto c : r.get_columns())
    {
        if (res != "")
        {
            res += ", ";
        }
        res += "?";
    }

    return res;
}

string mysql_row_helper::get_update_placeholders_string(const row &r)
{
    string res;

    for (auto c : r.get_columns())
    {
        if (c.get_name() != r.get_primary_key().get_name())
        {
            if (res != "")
            {
                res += ", ";
            }
            res += c.get_name() + " = ?";
        }
    }

    return res;
}

void mysql_row_helper::set_placeholder_values(PreparedStatement *stmt, bool include_key, const row &r)
{
    int i = 1;

    for (auto c : r.get_columns())
    {
        if (!include_key && (c.get_name() == r.get_primary_key().get_name()))
        {
            continue;
        }

        switch (c.get_type())
        {
            case column::bool_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_bool_column_value(c);

                        if (v.is_null())
                        {
                            stmt->setNull(i++, DataType::TINYINT);
                        }
                        else
                        {
                            stmt->setBoolean(i++, v.value());
                        }
                    }
                    else
                    {
                        stmt->setBoolean(i++, r.get_bool_column_value(c));
                    }
                    break;
            case column::int_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_int_column_value(c);

                        if (v.is_null())
                        {
                            stmt->setNull(i++, DataType::INTEGER);
                        }
                        else
                        {
                            stmt->setInt(i++, v.value());
                        }
                    }
                    else
                    {
                        stmt->setInt(i++, r.get_int_column_value(c));
                    }
                    break;
            case column::octet_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_octet_column_value(c);

                        if (v.is_null())
                        {
                            stmt->setNull(i++, DataType::INTEGER);
                        }
                        else
                        {
                            stmt->setUInt(i++, v.value());
                        }
                    }
                    else
                    {
                        stmt->setUInt(i++, r.get_octet_column_value(c));
                    }
                    break;
            case column::uint_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_uint_column_value(c);

                        if (v.is_null())
                        {
                            stmt->setNull(i++, DataType::INTEGER);
                        }
                        else
                        {
                            stmt->setUInt(i++, v.value());
                        }
                    }
                    else
                    {
                        stmt->setUInt(i++, r.get_uint_column_value(c));
                    }
                    break;
            case column::uuid_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_uuid_column_value(c);

                        if (v.is_null())
                        {
                            stmt->setNull(i++, DataType::VARCHAR);
                        }
                        else
                        {
                            stmt->setString(i++, to_string(v.value()));
                        }
                    }
                    else
                    {
                        stmt->setString(i++, to_string(r.get_uuid_column_value(c)));
                    }
                    break;
            case column::datetime_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_datetime_column_value(c);

                        if (v.is_null())
                        {
                            stmt->setNull(i++, DataType::DATE);
                        }
                        else
                        {
                            stmt->setDateTime(i++, v.value().to_string());
                        }
                    }
                    else
                    {
                        stmt->setDateTime(i++, r.get_datetime_column_value(c).to_string());
                    }
                    break;
            case column::string_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_string_column_value(c);

                        if (v.is_null())
                        {
                            stmt->setNull(i++, DataType::VARCHAR);
                        }
                        else
                        {
                            stmt->setString(i++, v.value());
                        }
                    }
                    else
                    {
                        stmt->setString(i++, r.get_string_column_value(c));
                    }
                    break;
            default:
                THROW(row_exception, "invalid column type", c.get_type());
        }
    }
}

string mysql_row_helper::get_columns_string(const row &r)
{
    string res;

    for (auto c : r.get_columns())
    {
        if (res != "")
        {
            res += ", ";
        }
        res += c.get_name();
    }

    return res;
}

void mysql_row_helper::check_foreign_keys(connection &conn, const row &r)
{
    for (auto fk : r.get_foreign_keys())
    {
        string sql = "select count(*) from " + fk.get_referencing_table() + " where " + fk.get_referencing_column().get_name() + " = ?";

        shared_ptr<PreparedStatement> stmt(conn.m_mysql_connection->prepareStatement(sql));
        stmt->setString(1, to_string(r.get_uuid_column_value(fk.get_referenced_column())));

        unique_ptr<ResultSet> results(stmt->executeQuery());

        if (results->next())
        {
            if (results->getUInt(1) > 0)
            {
                THROW(row_references_exist_exception, 
                            "foreign key references to " + fk.get_referenced_table() + " found",
                            fk.get_referencing_table() + "(" + fk.get_referencing_column().get_name() + ")");
            }
        }
        else
        {
            // no rows yet, so the check passes
        }
    }
}

void mysql_row_helper::insert_row(connection &conn, row &r)
{
    string sql = "insert into " + r.get_table_name() + " (" + get_columns_string(r) + ") values (" + get_insert_placeholders_string(r) + ")";
    unique_ptr<PreparedStatement> stmt(conn.m_mysql_connection->prepareStatement(sql));
    set_placeholder_values(stmt.get(), true, r);

    try
    {
        unique_ptr<ResultSet> results(stmt->executeQuery());
    }
    catch (SQLException &e)
    {
        if (e.getErrorCode() == 1452)
        {
            THROW(row_foreign_key_exception, "attempt to insert row with foreign key referece to non-existent row", e.getSQLState());
        }
        else
        {
            throw;
        }
    }
}

void mysql_row_helper::update_row(connection &conn, row &r)
{
    string sql = "update " + r.get_table_name() + " set " + get_update_placeholders_string(r) + " where " + r.get_primary_key().get_name() + " = ?";
    unique_ptr<PreparedStatement> stmt(conn.m_mysql_connection->prepareStatement(sql));
    set_placeholder_values(stmt.get(), false, r);
    stmt->setString(r.get_columns().size(), to_string(r.get_uuid_column_value(r.get_primary_key())));
    unique_ptr<ResultSet> results(stmt->executeQuery());
}

void mysql_row_helper::delete_row(connection &conn, row &r)
{
    check_foreign_keys(conn, r);

    string sql = "delete from " + r.get_table_name() + " where " + r.get_primary_key().get_name() + " = ?";
    unique_ptr<PreparedStatement> stmt(conn.m_mysql_connection->prepareStatement(sql));
    stmt->setString(1, to_string(r.get_uuid_column_value(r.get_primary_key())));
    unique_ptr<ResultSet> results(stmt->executeQuery());
}

vector<row *> mysql_row_helper::fetch_rows(connection &conn, const column &c, uuid key, const row &r)
{
    string sql = "select " + get_columns_string(r) + " from " + r.get_table_name() + " where " + c.get_name() + " = ?";

    shared_ptr<PreparedStatement> stmt(conn.m_mysql_connection->prepareStatement(sql));
    stmt->setString(1, to_string(key));

    return fetch_rows(stmt, r);
}

vector<row *> mysql_row_helper::fetch_rows(connection &conn, const row &r)
{
    try
    {
        string sql = "select " + get_columns_string(r) + " from " + r.get_table_name();
        shared_ptr<PreparedStatement> stmt(conn.m_mysql_connection->prepareStatement(sql));
        return fetch_rows(stmt, r);
    }
    catch (SQLException &e)
    {
        if (e.getErrorCode() == 1146)
        {
            THROW(row_table_doesnt_exist_exception, "attempt to select from non-existant table", e.getSQLState());
        }
        else
        {
            throw;
        }
    }
}

row *mysql_row_helper::fetch_row(connection &conn, const column &c, uuid unique_key, const row &r)
{
    auto rows = fetch_rows(conn, c, unique_key, r);
    if (rows.size() == 0)
    {
        THROW(row_not_found_exception, "row from " + r.get_table_name() + " with unique key column " + c.get_name() + " not found", to_string(unique_key));
    }
    else if (rows.size() == 1)
    {
        return rows[0];
    }
    else
    {
        THROW(row_not_unique_exception, "more than one row from " + r.get_table_name() + " with unique key column " + c.get_name() + " found", to_string(unique_key));
    }
}

vector<row *> mysql_row_helper::fetch_rows(shared_ptr<PreparedStatement> stmt, const row &r)
{
    vector<row *> rows;
    unique_ptr<ResultSet> results(stmt->executeQuery());

    while (results->next())
    {
        auto row = r.create_instance();

        for (auto c : r.get_columns())
        {
            switch (c.get_type())
            {
                case column::bool_e:
                        if (c.get_nullable())
                        {
                            if (!results->isNull(c.get_position()))
                            {
                                row->set_column_value(c, nullable<bool>(results->getBoolean(c.get_position())));
                            }
                        }
                        else
                        {
                            row->set_column_value(c, results->getBoolean(c.get_position()));
                        }
                        break;
                case column::octet_e:
                        if (c.get_nullable())
                        {
                            if (!results->isNull(c.get_position()))
                            {
                                row->set_column_value(c, nullable<octet>(results->getUInt(c.get_position())));
                            }
                        }
                        else
                        {
                            row->set_column_value(c, static_cast<octet>(results->getUInt(c.get_position())));
                        }
                        break;
                case column::int_e:
                        if (c.get_nullable())
                        {
                            if (!results->isNull(c.get_position()))
                            {
                                row->set_column_value(c, nullable<int>(results->getInt(c.get_position())));
                            }
                        }
                        else
                        {
                            row->set_column_value(c, results->getInt(c.get_position()));
                        }
                        break;
                case column::uint_e:
                        if (c.get_nullable())
                        {
                            if (!results->isNull(c.get_position()))
                            {
                                row->set_column_value(c, nullable<uint>(results->getUInt(c.get_position())));
                            }
                        }
                        else
                        {
                            row->set_column_value(c, results->getUInt(c.get_position()));
                        }
                        break;
                case column::uuid_e:
                        if (c.get_nullable())
                        {
                            if (!results->isNull(c.get_position()))
                            {
                                row->set_column_value(c, nullable<uuid>(boost::lexical_cast<uuid>(results->getString(c.get_position()))));
                            }
                        }
                        else
                        {
                            row->set_column_value(c, boost::lexical_cast<uuid>(results->getString(c.get_position())));
                        }
                        break;
                case column::datetime_e:
                        if (c.get_nullable())
                        {
                            if (!results->isNull(c.get_position()))
                            {
                                row->set_column_value(c, nullable<datetime>(datetime(results->getString(c.get_position()))));
                            }
                        }
                        else
                        {
                            row->set_column_value(c, datetime(results->getString(c.get_position())));
                        }
                        break;
                case column::string_e:
                        if (c.get_nullable() && results->isNull(c.get_position()))
                        {
                            if (!results->isNull(c.get_position()))
                            {
                                row->set_column_value(c, nullable<string>(results->getString(c.get_position())));
                            }
                        }
                        else
                        {
                            row->set_column_value(c, results->getString(c.get_position()));
                        }
                        break;
                default:
                    THROW(row_exception, "invalid column type", c.get_type());
            }
        }

        rows.push_back(row);
    }

    return rows;
}

void mysql_row_helper::delete_rows(connection &conn, const column &c, uuid key, const row &r)
{
    string sql = "delete from " + r.get_table_name() + " where " + c.get_name() + " = ?";

    check_foreign_keys(conn, r);

    unique_ptr<PreparedStatement> stmt(conn.m_mysql_connection->prepareStatement(sql));
    stmt->setString(1, to_string(key));
    unique_ptr<ResultSet> res(stmt->executeQuery());
}

void mysql_row_helper::execute(connection &conn, const string &sql)
{
    unique_ptr<Statement> stmt(conn.m_mysql_connection->createStatement());
    stmt->execute(sql);
}
