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
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include "postgres_row_helper.hpp"
#include "row.hpp"

using namespace std;
using namespace adns;
using namespace db;
using namespace sql;

string postgres_row_helper::get_insert_placeholders_string(const row &r)
{
    string res;
    int i = 1;

    for (auto c : r.get_columns())
    {
        if (res != "")
        {
            res += ", ";
        }
        res += "$" + boost::lexical_cast<string>(i++);
    }

    return res;
}

string postgres_row_helper::get_update_placeholders_string(const row &r)
{
    string res;
    int i = 1;

    for (auto c : r.get_columns())
    {
        if (c.get_name() != r.get_primary_key().get_name())
        {
            if (res != "")
            {
                res += ", ";
            }
            res += c.get_name() + " = $" + boost::lexical_cast<string>(i);
        }
        i++;
    }

    return res;
}

void postgres_row_helper::set_placeholder_values(char *values[], const row &r)
{
    int i = 0;

    for (auto c : r.get_columns())
    {
        switch (c.get_type())
        {
            case column::bool_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_bool_column_value(c);

                        if (v.is_null())
                        {
                            values[i++] = nullptr;
                        }
                        else
                        {
                            values[i++] = strdup(v.value() ? "TRUE" : "FALSE");
                        }
                    }
                    else
                    {
                        values[i++] = strdup(r.get_bool_column_value(c) ? "TRUE" : "FALSE");
                    }
                    break;
            case column::int_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_int_column_value(c);

                        if (v.is_null())
                        {
                            values[i++] = nullptr;
                        }
                        else
                        {
                            values[i++] = strdup(boost::lexical_cast<string>(v.value()).c_str());
                        }
                    }
                    else
                    {
                        values[i++] = strdup(boost::lexical_cast<string>(r.get_int_column_value(c)).c_str());
                    }
                    break;
            case column::uint_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_uint_column_value(c);

                        if (v.is_null())
                        {
                            values[i++] = nullptr;
                        }
                        else
                        {
                            values[i++] = strdup(boost::lexical_cast<string>(v.value()).c_str());
                        }
                    }
                    else
                    {
                        values[i++] = strdup(boost::lexical_cast<string>(r.get_uint_column_value(c)).c_str());
                    }
                    break;
            case column::octet_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_octet_column_value(c);

                        if (v.is_null())
                        {
                            values[i++] = nullptr;
                        }
                        else
                        {
                            values[i++] = strdup(boost::lexical_cast<string>(v.value()).c_str());
                        }
                    }
                    else
                    {
                        values[i++] = strdup(boost::lexical_cast<string>(static_cast<uint>(r.get_octet_column_value(c))).c_str());
                    }
                    break;
            case column::uuid_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_uuid_column_value(c);

                        if (v.is_null())
                        {
                            values[i++] = nullptr;
                        }
                        else
                        {
                            values[i++] = strdup(to_string(v.value()).c_str());
                        }
                    }
                    else
                    {
                        values[i++] = strdup(to_string(r.get_uuid_column_value(c)).c_str());
                    }
                    break;
            case column::datetime_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_datetime_column_value(c);

                        if (v.is_null())
                        {
                            values[i++] = nullptr;
                        }
                        else
                        {
                            values[i++] = strdup(v.value().to_string().c_str());
                        }
                    }
                    else
                    {
                        values[i++] = strdup(r.get_datetime_column_value(c).to_string().c_str());
                    }
                    break;
            case column::string_e:
                    if (c.get_nullable())
                    {
                        auto v = r.get_nullable_string_column_value(c);

                        if (v.is_null())
                        {
                            values[i++] = nullptr;
                        }
                        else
                        {
                            values[i++] = strdup(v.value().c_str());
                        }
                    }
                    else
                    {
                        values[i++] = strdup(r.get_string_column_value(c).c_str());
                    }
                    break;
            default:
                THROW(row_exception, "invalid column type", c.get_type());
        }
    }
}

string postgres_row_helper::get_columns_string(const row &r)
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

void postgres_row_helper::check_foreign_keys(connection &conn, const row &r)
{
    for (auto fk : r.get_foreign_keys())
    {
        string sql = "select count(*) from " + fk.get_referencing_table() + " where " + fk.get_referencing_column().get_name() + " = $1";

        char *param_values[1];
        param_values[0] = strdup(to_string(r.get_uuid_column_value(fk.get_referenced_column())).c_str());

        auto res = PQexecParams(conn.m_postgres_connection,
                           sql.c_str(),
                           1,       /* one param */
                           NULL,    /* let the backend deduce param type */
                           param_values,
                           NULL,    /* don't need param lengths since text */
                           NULL,    /* default to all text params */
                           0);      /* ask for string results */

        auto status = PQresultStatus(res);

        free(param_values[0]);

        if (status != PGRES_TUPLES_OK)
        {
            clear_and_throw_exception(conn, res);
        }

        if (PQgetisnull(res, 0, 0))
        {
            PQclear(res);
            THROW(row_exception, "null value found for row count when checking foreign key");
        }

        char *v = PQgetvalue(res, 0, 0);

        if (v == nullptr)
        {
            PQclear(res);
            THROW(row_exception, "null value found for row count when checking foreign key");
        }

        if (atoi(v) > 0)
        {
            PQclear(res);
            THROW(row_references_exist_exception, 
                        "foreign key references to " + fk.get_referenced_table() + " found",
                        fk.get_referencing_table() + "(" + fk.get_referencing_column().get_name() + ")");
        }

        PQclear(res);
    }
}

void postgres_row_helper::insert_row(connection &conn, row &r)
{
    string sql = "insert into " + r.get_table_name() + " (" + get_columns_string(r) + ") values (" + get_insert_placeholders_string(r) + ")";

    char *param_values[r.get_columns().size()];

    set_placeholder_values(param_values, r);

    auto res = PQexecParams(conn.m_postgres_connection,
                       sql.c_str(),
                       r.get_columns().size(), 
                       NULL,
                       param_values,
                       NULL,
                       NULL,
                       0);

    auto status = PQresultStatus(res);

    free_placeholder_values(param_values, r.get_columns().size());

    if (status != PGRES_COMMAND_OK)
    {
        clear_and_throw_exception(conn, res);
    }
}

void postgres_row_helper::free_placeholder_values(char *values[], int size)
{
    for (int i = 0; i < size; i++)
    {
        free(values[i]);
        values[i] = nullptr;
    }
}

void postgres_row_helper::update_row(connection &conn, row &r)
{
    string sql = 
            "update " + r.get_table_name() + 
            " set " + get_update_placeholders_string(r) + 
            " where " + r.get_primary_key().get_name() + " = $" + boost::lexical_cast<string>(r.get_primary_key().get_position() + 1);

    char *param_values[r.get_columns().size()];

    set_placeholder_values(param_values, r);

    auto res = PQexecParams(conn.m_postgres_connection,
                       sql.c_str(),
                       r.get_columns().size(),
                       NULL,
                       param_values,
                       NULL,
                       NULL,
                       0);

    auto status = PQresultStatus(res);

    free_placeholder_values(param_values, r.get_columns().size());

    if (status != PGRES_COMMAND_OK)
    {
        clear_and_throw_exception(conn, res);
    }
}

void postgres_row_helper::delete_row(connection &conn, row &r)
{
    string sql = "delete from " + r.get_table_name() + " where " + r.get_primary_key().get_name() + " = $1";

    char *param_values[1];
    param_values[0] = strdup(to_string(r.get_uuid_column_value(r.get_primary_key())).c_str());

    auto res = PQexecParams(conn.m_postgres_connection,
                       sql.c_str(),
                       1,       /* one param */
                       NULL,    /* let the backend deduce param type */
                       param_values,
                       NULL,    /* don't need param lengths since text */
                       NULL,    /* default to all text params */
                       0);      /* ask for string results */

    auto status = PQresultStatus(res);

    free(param_values[0]);

    if (status != PGRES_COMMAND_OK)
    {
        clear_and_throw_exception(conn, res);
    }
}

vector<row *> postgres_row_helper::fetch_rows(connection &conn, const column &c, uuid key, const row &r)
{
    string sql = "select " + get_columns_string(r) + " from " + r.get_table_name() + " where " + c.get_name() + " = $1";

    char *param_values[1];
    param_values[0] = strdup(to_string(key).c_str());

    auto res = PQexecParams(conn.m_postgres_connection,
                       sql.c_str(),
                       1,       /* one param */
                       NULL,    /* let the backend deduce param type */
                       param_values,
                       NULL,    /* don't need param lengths since text */
                       NULL,    /* default to all text params */
                       0);      /* ask for string results */

    free(param_values[0]);

    return fetch_rows(conn, res, r);
}

vector<row *> postgres_row_helper::fetch_rows(connection &conn, const row &r)
{
    string sql = "select " + get_columns_string(r) + " from " + r.get_table_name();
    auto res = PQexec(conn.m_postgres_connection, sql.c_str());
    return fetch_rows(conn, res, r);
}

row *postgres_row_helper::fetch_row(connection &conn, const column &c, uuid unique_key, const row &r)
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

void postgres_row_helper::clear_and_throw_exception(connection &conn, PGresult *res)
{
    auto err = error_string(conn.m_postgres_connection);
    auto ef = PQresultErrorField(res, PG_DIAG_SQLSTATE);
    if (ef != nullptr && strcmp(ef, "42P01") == 0)
    {
        PQclear(res);
        THROW(row_table_doesnt_exist_exception, "attempt to select from non-existant table", err);
    }
    else
    {
        PQclear(res);
        THROW(row_exception, "postgres error", err);
    }
}

vector<row *> postgres_row_helper::fetch_rows(connection &conn, PGresult *res, const row &r)
{
    vector<row *> rows;

    auto status = PQresultStatus(res);

    if (status == PGRES_COMMAND_OK)
    {
        return rows;
    }
    else if (status != PGRES_TUPLES_OK)
    {
        clear_and_throw_exception(conn, res);
    }

    for (int i = 0; i < PQntuples(res); i++)
    {
        auto row = r.create_instance();

        for (auto c : r.get_columns())
        {
            bool is_null = PQgetisnull(res, i, c.get_position() - 1);

            if (is_null)
            {
                if (c.get_nullable())
                {
                    continue;
                }
                else
                {
                    THROW(row_exception, "null value found for non-nullable column", c.get_name());
                }
            }

            char *v = PQgetvalue(res, i, c.get_position() - 1);

            switch (c.get_type())
            {
                case column::bool_e:
                        if (c.get_nullable())
                        {
                            row->set_column_value(c, nullable<bool>(strcmp(v, "t") == 0));
                        }
                        else
                        {
                            row->set_column_value(c, strcmp(v, "t") == 0);
                        }
                        break;
                case column::int_e:
                        if (c.get_nullable())
                        {
                            row->set_column_value(c, nullable<int>(atoi(v)));
                        }
                        else
                        {
                            row->set_column_value(c, atoi(v));
                        }
                        break;
                case column::uint_e:
                        if (c.get_nullable())
                        {
                            row->set_column_value(c, nullable<uint>(atoi(v)));
                        }
                        else
                        {
                            row->set_column_value(c, (uint)atoi(v));
                        }
                        break;
                case column::octet_e:
                        if (c.get_nullable())
                        {
                            row->set_column_value(c, nullable<octet>(atoi(v)));
                        }
                        else
                        {
                            row->set_column_value(c, (octet)atoi(v));
                        }
                        break;
                case column::uuid_e:
                        if (c.get_nullable())
                        {
                            row->set_column_value(c, nullable<uuid>(boost::lexical_cast<uuid>(v)));
                        }
                        else
                        {
                            row->set_column_value(c, boost::lexical_cast<uuid>(v));
                        }
                        break;
                case column::datetime_e:
                        if (c.get_nullable())
                        {
                            row->set_column_value(c, nullable<datetime>(datetime(v)));
                        }
                        else
                        {
                            row->set_column_value(c, datetime(v));
                        }
                        break;
                case column::string_e:
                        if (c.get_nullable())
                        {
                            row->set_column_value(c, nullable<string>(v));
                        }
                        else
                        {
                            row->set_column_value(c, string(v));
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

void postgres_row_helper::delete_rows(connection &conn, const column &c, uuid key, const row &r)
{
    string sql = "delete from " + r.get_table_name() + " where " + c.get_name() + " = $1";

    check_foreign_keys(conn, r);

    char *param_values[1];
    param_values[0] = strdup(to_string(key).c_str());

    auto res = PQexecParams(conn.m_postgres_connection,
                       sql.c_str(),
                       1,       /* one param */
                       NULL,    /* let the backend deduce param type */
                       param_values,
                       NULL,    /* don't need param lengths since text */
                       NULL,    /* default to all text params */
                       0);      /* ask for string results */

    auto status = PQresultStatus(res);

    free(param_values[0]);

    if (status != PGRES_COMMAND_OK)
    {
        clear_and_throw_exception(conn, res);
    }
}

void postgres_row_helper::execute(connection &conn, const string &sql)
{
    if (conn.m_postgres_connection)
    {
        auto res = PQexec(conn.m_postgres_connection, sql.c_str());
        if (PQresultStatus(res) == PGRES_COMMAND_OK)
        {
            PQclear(res);
        }
        else
        {
            auto err = error_string(conn.m_postgres_connection);
            PQclear(res);
            THROW(row_exception, "postgres command failed", err);
        }
    }
    else
    {
        THROW(row_exception, "postgres connection is null");
    }
}

void postgres_row_helper::begin_transaction(connection &conn)
{
    execute(conn, "BEGIN TRANSACTION");
}

void postgres_row_helper::commit_transaction(connection &conn)
{
    execute(conn, "COMMIT TRANSACTION");
}

void postgres_row_helper::rollback_transaction(connection &conn)
{
    execute(conn, "ROLLBACK TRANSACTION");
}

string postgres_row_helper::error_string(PGconn *conn)
{
    auto err = PQerrorMessage(conn);
    
    if (err)
    {
        char buf[1024];
        strncpy(buf, err, 1024);
        buf[1023] = 0;
        string res(buf);
        replace(res.begin(), res.end(), '\n', ' ');
        replace(res.begin(), res.end(), '\r', ' ');
        return res;
    }
    else
    {
        return "<null>";
    }
}
