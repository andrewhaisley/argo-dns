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

#include "sqlite_row_helper.hpp"
#include "row.hpp"

using namespace std;
using namespace boost;
using namespace adns;
using namespace db;
using namespace sql;

class callback_data_t
{
public:

    callback_data_t(const row &p_row, vector<row *> &p_rows) : m_row(p_row), m_rows(p_rows) {}
    const row &m_row;
    vector<row *> &m_rows;
};

string sqlite_row_helper::get_insert_placeholders_string(const row &r)
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

string sqlite_row_helper::get_update_placeholders_string(const row &r)
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

void sqlite_row_helper::set_placeholder_values(sqlite3_stmt *stmt, bool include_key, const row &r)
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
                            int rc = sqlite3_bind_null(stmt, i);
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind null bool", rc);
                            }
                        }
                        else
                        {
                            int rc = sqlite3_bind_int(stmt, i, v.value() ? 1 : 0);
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind bool", rc);
                            }
                        }
                    }
                    else
                    {
                        int rc = sqlite3_bind_int(stmt, i, r.get_bool_column_value(c) ? 1 : 0);
                        if (rc != SQLITE_OK)
                        {
                            THROW(sqlite_exception, "sqlite error in bind bool", rc);
                        }
                    }
                    break;
            case column::int_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_int_column_value(c);

                        if (v.is_null())
                        {
                            int rc = sqlite3_bind_null(stmt, i);
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind null int", rc);
                            }
                        }
                        else
                        {
                            int rc = sqlite3_bind_int(stmt, i, v.value());
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind bool", rc);
                            }
                        }
                    }
                    else
                    {
                        int rc = sqlite3_bind_int(stmt, i, r.get_int_column_value(c));
                        if (rc != SQLITE_OK)
                        {
                            THROW(sqlite_exception, "sqlite error in bind bool", rc);
                        }
                    }
                    break;
            case column::uint_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_uint_column_value(c);

                        if (v.is_null())
                        {
                            int rc = sqlite3_bind_null(stmt, i);
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind null uint", rc);
                            }
                        }
                        else
                        {
                            int rc = sqlite3_bind_int64(stmt, i, v.value());
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind int64", rc);
                            }
                        }
                    }
                    else
                    {
                        int rc = sqlite3_bind_int64(stmt, i, r.get_uint_column_value(c));
                        if (rc != SQLITE_OK)
                        {
                            THROW(sqlite_exception, "sqlite error in bind int64", rc);
                        }
                    }
                    break;
            case column::octet_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_octet_column_value(c);

                        if (v.is_null())
                        {
                            int rc = sqlite3_bind_null(stmt, i);
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind null octet", rc);
                            }
                        }
                        else
                        {
                            int rc = sqlite3_bind_int64(stmt, i, v.value());
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind int64", rc);
                            }
                        }
                    }
                    else
                    {
                        int rc = sqlite3_bind_int64(stmt, i, r.get_octet_column_value(c));
                        if (rc != SQLITE_OK)
                        {
                            THROW(sqlite_exception, "sqlite error in bind int64", rc);
                        }
                    }
                    break;
            case column::datetime_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_datetime_column_value(c);

                        if (v.is_null())
                        {
                            int rc = sqlite3_bind_null(stmt, i);
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind null datetime", rc);
                            }
                        }
                        else
                        {
                            int rc = sqlite3_bind_text(stmt, i, v.value().to_string().c_str(), -1, SQLITE_TRANSIENT);
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind datetime", rc);
                            }
                        }
                    }
                    else
                    {
                        int rc = sqlite3_bind_text(stmt, i, r.get_datetime_column_value(c).to_string().c_str(), -1, SQLITE_TRANSIENT);
                        if (rc != SQLITE_OK)
                        {
                            THROW(sqlite_exception, "sqlite error in bind datetime", rc);
                        }
                    }
                    break;
            case column::string_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_string_column_value(c);

                        if (v.is_null())
                        {
                            int rc = sqlite3_bind_null(stmt, i);
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind null string", rc);
                            }
                        }
                        else
                        {
                            int rc = sqlite3_bind_text(stmt, i, v.value().c_str(), -1, SQLITE_TRANSIENT);
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind string", rc);
                            }
                        }
                    }
                    else
                    {
                        int rc = sqlite3_bind_text(stmt, i, r.get_string_column_value(c).c_str(), -1, SQLITE_TRANSIENT);
                        if (rc != SQLITE_OK)
                        {
                            THROW(sqlite_exception, "sqlite error in bind string", rc);
                        }
                    }
                    break;
            case column::uuid_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_uuid_column_value(c);

                        if (v.is_null())
                        {
                            int rc = sqlite3_bind_null(stmt, i);
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind null uuid", rc);
                            }
                        }
                        else
                        {
                            int rc = sqlite3_bind_text(stmt, i, to_string(v.value()).c_str(), -1, SQLITE_TRANSIENT);
                            if (rc != SQLITE_OK)
                            {
                                THROW(sqlite_exception, "sqlite error in bind uuid", rc);
                            }
                        }
                    }
                    else
                    {
                        int rc = sqlite3_bind_text(stmt, i, to_string(r.get_uuid_column_value(c)).c_str(), -1, SQLITE_TRANSIENT);
                        if (rc != SQLITE_OK)
                        {
                            THROW(sqlite_exception, "sqlite error in bind uuid", rc);
                        }
                    }
                    break;
            default:
                THROW(row_exception, "invalid column type", c.get_type());
        }
        i++;
    }
}

void sqlite_row_helper::insert_row(connection &conn, row &r)
{
    string sql = "insert into " + r.get_table_name() + " (" + get_columns_string(r) + ") values (" + get_insert_placeholders_string(r) + ")";

    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(conn.m_sqlite_connection, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        THROW(sqlite_exception, "sqlite prepare for insert failed", rc);
    }

    try
    {
        set_placeholder_values(stmt, true, r);

        int rc = sqlite3_step(stmt);

        if (rc != SQLITE_OK && rc != SQLITE_DONE)
        {
            sqlite3_finalize(stmt);
            THROW(sqlite_exception, "sqlite step for insert failed", rc);
        }
        
        sqlite3_finalize(stmt);
    }
    catch (...)
    {
        sqlite3_finalize(stmt);
        throw;
    }
}

void sqlite_row_helper::update_row(connection &conn, row &r)
{
    string sql = "update " + r.get_table_name() + " set " + get_update_placeholders_string(r) + " where " + r.get_primary_key().get_name() + " = ?";

    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(conn.m_sqlite_connection, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        THROW(sqlite_exception, "sqlite prepare for update failed", rc);
    }

    try
    {
        set_placeholder_values(stmt, true, r);

        int rc = sqlite3_step(stmt);

        if (rc != SQLITE_OK && rc != SQLITE_DONE)
        {
            sqlite3_finalize(stmt);
            THROW(sqlite_exception, "sqlite step for update failed", rc);
        }
        
        sqlite3_finalize(stmt);
    }
    catch (...)
    {
        sqlite3_finalize(stmt);
        throw;
    }
}

void sqlite_row_helper::delete_row(connection &conn, row &r)
{
    char *sqlite_err;

    string sql = "delete from " + r.get_table_name() + " where " + r.get_primary_key().get_name() + " = '" + lexical_cast<string>(r.get_uuid_column_value(r.get_primary_key())) + "'";

    auto rc = sqlite3_exec(conn.m_sqlite_connection, sql.c_str(), nullptr, nullptr, &sqlite_err);

    if (rc != SQLITE_OK)
    {
        string s(sqlite_err);
        sqlite3_free(sqlite_err);
        THROW(sqlite_exception, s, rc);
    }
}

void sqlite_row_helper::delete_rows(connection &conn, const column &c, uuid key, const row &r)
{
    char *sqlite_err;

    string sql = "delete from " + r.get_table_name() + " where " + c.get_name() + " = '" + lexical_cast<string>(key) + "'";

    auto rc = sqlite3_exec(conn.m_sqlite_connection, sql.c_str(), nullptr, nullptr, &sqlite_err);

    if (rc != SQLITE_OK)
    {
        string s(sqlite_err);
        sqlite3_free(sqlite_err);
        THROW(sqlite_exception, s, rc);
    }
}

int sqlite_row_helper::select_callback(void *rp, int count, char **data, char **columns)
{
    callback_data_t *cd = (callback_data_t *)(rp);

    int i = 0;

    row *r = cd->m_row.create_instance();

    for (auto c : cd->m_row.get_columns())
    {
        if (data[i] != nullptr)
        {
            if (c.get_nullable())
            {
                switch (c.get_type())
                {
                case column::bool_e:
                    r->set_column_value(c, nullable<bool>(strcmp(data[i], "1") == 0));
                    break;
                case column::int_e:
                    r->set_column_value(c, nullable<int>(lexical_cast<int>(data[i])));
                    break;
                case column::uint_e:
                    r->set_column_value(c, nullable<uint>(lexical_cast<uint>(data[i])));
                    break;
                case column::octet_e:
                    r->set_column_value(c, nullable<octet>(static_cast<octet>(lexical_cast<uint>(data[i]))));
                    break;
                case column::uuid_e:
                    r->set_column_value(c, nullable<uuid>(lexical_cast<uuid>(data[i])));
                    break;
                case column::datetime_e:
                    r->set_column_value(c, nullable<datetime>(datetime(data[i])));
                    break;
                case column::string_e:
                    r->set_column_value(c, nullable<string>(string(data[i])));
                    break;
                case column::invalid_e:
                default:
                    delete r;
                    THROW(row_exception, "invalid column type found in select_callback", c.get_type());
                }
            }
            else
            {
                switch (c.get_type())
                {
                case column::bool_e:
                    r->set_column_value(c, strcmp(data[i], "1") == 0);
                    break;
                case column::int_e:
                    r->set_column_value(c, lexical_cast<int>(data[i]));
                    break;
                case column::uint_e:
                    r->set_column_value(c, lexical_cast<uint>(data[i]));
                    break;
                case column::octet_e:
                    r->set_column_value(c, static_cast<octet>(lexical_cast<uint>(data[i])));
                    break;
                case column::uuid_e:
                    r->set_column_value(c, lexical_cast<uuid>(data[i]));
                    break;
                case column::datetime_e:
                    r->set_column_value(c, datetime(data[i]));
                    break;
                case column::string_e:
                    r->set_column_value(c, string(data[i]));
                    break;
                case column::invalid_e:
                default:
                    delete r;
                    THROW(row_exception, "invalid column type found in select_callback", c.get_type());
                }
            }
        }
        i++;
    }

    cd->m_rows.push_back(r);

    return 0;
}

vector<row *> sqlite_row_helper::fetch_rows(connection &conn, const string &sql, const row &r)
{
    char *sqlite_err = nullptr;

    vector<row *> res;
    callback_data_t cd(r, res);

    int rc = sqlite3_exec(conn.m_sqlite_connection, sql.c_str(), select_callback, &cd, &sqlite_err);

    if (rc == SQLITE_OK)
    {
        return res;
    }
    else
    {
        string s(sqlite_err);
        sqlite3_free(sqlite_err);
        if (s.find("no such table") == string::npos)
        {
            THROW(sqlite_exception, s, rc);
        }
        else
        {
            THROW(row_table_doesnt_exist_exception, "attempt to select from non-existant table", s);
        }
    }
}

row *sqlite_row_helper::fetch_row(connection &conn, const column &c, uuid unique_key, const row &r)
{
    string sql = "select " + get_columns_string(r) + " from " + r.get_table_name() + " where " + c.get_name() + " = '" + lexical_cast<string>(unique_key) + "'";

    auto rows = fetch_rows(conn, sql, r);

    if (rows.size() == 0)
    {
        THROW(row_not_found_exception, "row from " + r.get_table_name() + " with unique key column " + c.get_name() + " not found", lexical_cast<string>(unique_key));
    }
    else if (rows.size() == 1)
    {
        return rows[0];
    }
    else
    {
        THROW(row_not_unique_exception, "more than one row from " + r.get_table_name() + " with unique key column " + c.get_name() + " found", lexical_cast<string>(unique_key));
    }
}

vector<row *> sqlite_row_helper::fetch_rows(connection &conn, const column &c, uuid key, const row &r)
{
    string sql = "select " + get_columns_string(r) + " from " + r.get_table_name() + " where " + c.get_name() + " = '" + lexical_cast<string>(key) + "'";

    return fetch_rows(conn, sql, r);
}

vector<row *> sqlite_row_helper::fetch_rows(connection &conn, const row &r)
{
    string sql = "select " + get_columns_string(r) + " from " + r.get_table_name();

    return fetch_rows(conn, sql, r);
}

void sqlite_row_helper::execute(connection &conn, const string &sql)
{
    char *sqlite_err = nullptr;

    int rc = sqlite3_exec(conn.m_sqlite_connection, sql.c_str(), nullptr, 0, &sqlite_err);
   
    if (rc != SQLITE_OK)
    {
        string err(sqlite_err);
        sqlite3_free(sqlite_err);
        THROW(sqlite_exception, err);
    } 
}

string sqlite_row_helper::get_columns_string(const row &r)
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

void sqlite_row_helper::begin_transaction(connection &conn)
{
    sqlite_row_helper::execute(conn, "BEGIN TRANSACTION");
}

void sqlite_row_helper::commit_transaction(connection &conn)
{
    sqlite_row_helper::execute(conn, "COMMIT");
}

void sqlite_row_helper::rollback_transaction(connection &conn)
{
    sqlite_row_helper::execute(conn, "ROLLBACK");
}
