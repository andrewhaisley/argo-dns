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
#include <mongoc.h>

#include "unparser.hpp"
#include "parser.hpp"
#include "mongodb_row_helper.hpp"
#include "row.hpp"

using namespace std;
using namespace boost;
using namespace adns;
using namespace db;
using namespace sql;

row *mongodb_row_helper::fetch_row(connection &conn, const column &c, uuid unique_key, const row &r)
{
    auto rows = fetch_rows(conn, c, unique_key, r);

    if (rows.size() == 0)
    {
        THROW(row_not_found_exception, "row from " + r.get_table_name() + " with unique key column " + c.get_name() + " not found");
    }
    else if (rows.size() == 1)
    {
        return rows[0];
    }
    else
    {
        THROW(row_not_unique_exception, "more than one row from " + r.get_table_name() + " with unique key column " + c.get_name() + " found");
    }
}

vector<row *> mongodb_row_helper::fetch_rows(connection &conn, const column &c, uuid key, const row &r)
{
    bson_error_t error;
    vector<row *> res;

    if (mongoc_database_has_collection(conn.m_mongodb_database, r.get_table_name().c_str(), &error))
    {
        auto coll = mongoc_database_get_collection(conn.m_mongodb_database, r.get_table_name().c_str());

        if (!coll)
        {
            THROW(mongodb_exception, "mongoc_database_get_collection returned nullptr", r.get_table_name());
        }

        auto query = BCON_NEW(c.get_name().c_str(), BCON_UTF8(to_string(key).c_str()));
        const bson_t *doc;

        auto cursor = mongoc_collection_find_with_opts(coll, query, nullptr, nullptr);

        while (mongoc_cursor_next(cursor, &doc)) 
        {
            auto row = r.create_instance();
            auto str = bson_as_relaxed_extended_json(doc, nullptr);
            auto j = parser::parse(str);
            row->from_json(*j);
            row->m_document_id = static_cast<string>((*j)["_id"]["$oid"]);
            bson_free(str);
            res.push_back(row);
        }

        bson_destroy(query);
        mongoc_collection_destroy(coll);

        if (mongoc_cursor_error(cursor, &error)) 
        {
            mongoc_cursor_destroy(cursor);
            THROW(mongodb_exception, "mongo cursor error", error.message);
        }

        mongoc_cursor_destroy(cursor);
    }
    else
    {
        THROW(row_table_doesnt_exist_exception, "attempt to read from non-existant collection", r.get_table_name());
    }

    return res;
}

vector<row *> mongodb_row_helper::fetch_rows(connection &conn, const row &r)
{
    bson_error_t error;
    vector<row *> res;

    if (mongoc_database_has_collection(conn.m_mongodb_database, r.get_table_name().c_str(), &error))
    {
        auto coll = mongoc_database_get_collection(conn.m_mongodb_database, r.get_table_name().c_str());

        if (!coll)
        {
            THROW(mongodb_exception, "mongoc_database_get_collection returned nullptr", error.message);
        }

        auto query = BCON_NEW(nullptr);
        const bson_t *doc;

        auto cursor = mongoc_collection_find_with_opts(coll, query, nullptr, nullptr);

        while (mongoc_cursor_next(cursor, &doc)) 
        {
            auto row = r.create_instance();
            auto str = bson_as_relaxed_extended_json(doc, nullptr);
            auto j = parser::parse(str);
            row->m_document_id = static_cast<string>((*j)["_id"]["$oid"]);
            row->from_json(*j);
            bson_free(str);
            res.push_back(row);
        }

        bson_destroy(query);
        mongoc_collection_destroy(coll);

        if (mongoc_cursor_error(cursor, &error)) 
        {
            mongoc_cursor_destroy(cursor);
            THROW(mongodb_exception, "mongo cursor error", error.message);
        }

        mongoc_cursor_destroy(cursor);
    }
    else
    {
        THROW(row_table_doesnt_exist_exception, "attempt to read from non-existant collection", r.get_table_name());
    }

    return res;
}

void mongodb_row_helper::insert_row(connection &conn, row &r)
{
    bson_error_t error;

    auto coll = mongoc_database_get_collection(conn.m_mongodb_database, r.get_table_name().c_str());

    if (!coll)
    {
        THROW(mongodb_exception, "mongoc_database_get_collection returned nullptr", r.get_table_name());
    }

    string j;
    j << r.to_json();

    auto b = bson_new_from_json((const uint8_t*)(j.c_str()), -1, &error);

    if (!b)
    {
        THROW(mongodb_exception, "bson_new_from_json returned nullptr", error.message);
    }

    auto res = mongoc_collection_insert_one(coll, b, nullptr, nullptr, &error);

    bson_destroy(b);
    mongoc_collection_destroy(coll);

    if (!res)
    {
        THROW(mongodb_exception, "mongoc_collection_insert_one failed", error.message);
    }
}

void mongodb_row_helper::create_collection(connection &conn, const std::string &name)
{
    bson_error_t error;

    auto coll = mongoc_database_create_collection(conn.m_mongodb_database, name.c_str(), nullptr, &error);

    if (!coll)
    {
        THROW(mongodb_exception, "mongoc_database_create_collection returned nullptr", error.message);
    }

    mongoc_collection_destroy(coll);
}

void mongodb_row_helper::update_row(connection &conn, row &r)
{
    bson_error_t error;
    bson_t *selector;
    bson_t *update;

    auto coll = mongoc_database_get_collection(conn.m_mongodb_database, r.get_table_name().c_str());

    if (!coll)
    {
        THROW(mongodb_exception, "mongoc_database_get_collection returned nullptr", r.get_table_name());
    }

    if (r.m_document_id != "")
    {
        bson_oid_t oid;
        bson_oid_init_from_string(&oid, r.m_document_id.c_str());
        selector = BCON_NEW("_id", BCON_OID(&oid));
    }
    else
    {
        selector = BCON_NEW(r.get_primary_key().get_name().c_str(), BCON_UTF8(to_string(r.get_primary_key_value()).c_str()));
    }

    string j;

    json fields(json::object_e);
    fields["$set"] = r.to_json();
    j << fields;

    cout << j << endl;

    update = bson_new_from_json((const uint8_t*)(j.c_str()), -1, &error);

    auto res = mongoc_collection_update_one(coll, selector, update, nullptr, nullptr, &error);

    bson_free(selector);
    bson_free(update);

    if (!res)
    {
        THROW(mongodb_exception, "mongoc_collection_update_one failed", error.message);
    }
}

void mongodb_row_helper::delete_row(connection &conn, row &r)
{
    bson_error_t error;
    bson_t *selector;

    auto coll = mongoc_database_get_collection(conn.m_mongodb_database, r.get_table_name().c_str());

    if (!coll)
    {
        THROW(mongodb_exception, "mongoc_database_get_collection returned nullptr", r.get_table_name());
    }

    if (r.m_document_id != "")
    {
        bson_oid_t oid;
        bson_oid_init_from_string(&oid, r.m_document_id.c_str());
        selector = BCON_NEW("_id", BCON_OID(&oid));
    }
    else
    {
        selector = BCON_NEW(r.get_primary_key().get_name().c_str(), BCON_UTF8(to_string(r.get_primary_key_value()).c_str()));
    }

    auto res = mongoc_collection_delete_one(coll, selector, nullptr, nullptr, &error);

    bson_free(selector);

    if (!res)
    {
        THROW(mongodb_exception, "mongoc_collection_delete_one failed", error.message);
    }
}

void mongodb_row_helper::delete_rows(connection &conn, const column &c, uuid key, const row &r)
{
    bson_error_t error;
    bson_t *selector;

    auto coll = mongoc_database_get_collection(conn.m_mongodb_database, r.get_table_name().c_str());

    if (!coll)
    {
        THROW(mongodb_exception, "mongoc_database_get_collection returned nullptr", r.get_table_name());
    }

    selector = BCON_NEW(c.get_name().c_str(), BCON_UTF8(to_string(key).c_str()));

    auto res = mongoc_collection_delete_many(coll, selector, nullptr, nullptr, &error);

    bson_free(selector);

    if (!res)
    {
        THROW(mongodb_exception, "mongoc_collection_delete_many failed", error.message);
    }
}

void mongodb_row_helper::begin_transaction(connection &conn)
{
    bson_error_t error;

    if (!mongoc_client_session_start_transaction(conn.get_mongodb_session(), nullptr, &error))
    {
        THROW(mongodb_exception, "mongoc_client_session_start_transaction returned false", error.message);
    }
}

void mongodb_row_helper::commit_transaction(connection &conn)
{
    bson_error_t error;

    if (!mongoc_client_session_commit_transaction(conn.get_mongodb_session(), nullptr, &error))
    {
        THROW(mongodb_exception, "mongoc_client_session_commit_transaction returned false", error.message);
    }
}

void mongodb_row_helper::rollback_transaction(connection &conn)
{
    bson_error_t error;

    if (!mongoc_client_session_abort_transaction(conn.get_mongodb_session(), &error))
    {
        THROW(mongodb_exception, "mongoc_client_session_abort_transaction returned false", error.message);
    }
}

void mongodb_row_helper::create_index(connection &conn, const string &json_params)
{
    bson_error_t error;
    bson_t keys;
    bson_t reply;

    auto j = parser::parse(json_params);

    string table = (*j)["table"];
    string column = (*j)["column"];
    bool unique = (*j)["unique"];

    mongoc_index_opt_t io;
    mongoc_index_opt_init(&io);

    io.unique = unique;

    bson_init (&keys);

    BSON_APPEND_INT32(&keys, column.c_str(), 1);
    char *index_name = mongoc_collection_keys_to_index_string(&keys);
    auto create_indexes = BCON_NEW("createIndexes",
                              BCON_UTF8(table.c_str()),
                              "indexes",
                              "[",
                                  "{",
                                        "key", BCON_DOCUMENT(&keys),
                                        "name", BCON_UTF8(index_name),
                                        "unique", BCON_BOOL(unique),
                                  "}",
                              "]");

    bool res = mongoc_database_write_command_with_opts(conn.m_mongodb_database, create_indexes, nullptr, &reply, &error);

    bson_free(index_name);
    bson_destroy(&reply);
    bson_destroy(create_indexes);

    if (!res)
    {
        THROW(mongodb_exception, "mongoc_database_write_command_with_opts returned false", error.message);
    }
}
