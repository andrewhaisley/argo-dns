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
#include <chrono>

#include <thread>
#include <mutex>

#include "types.hpp"
#include "connection_pool.hpp"

using namespace std;
using namespace adns;
using namespace db;

mutex connection_pool::o_lock;
list<connection_pool::connection_t> connection_pool::o_pool;

string connection_pool::o_url;
string connection_pool::o_username;
string connection_pool::o_password;
string connection_pool::o_database;
bool connection_pool::o_use_transactions;

type_t connection_pool::o_db_type;


void connection_pool::init(type_t db_type, const string &url, const string &username, const string &password, const string &database, bool use_transactions)
{
    o_db_type = db_type;
    o_url = url;
    o_username = username;
    o_password = password;
    o_database = database;
    o_use_transactions = use_transactions;
}

shared_ptr<connection_guard> connection_pool::get_connection()
{
    lock_guard<mutex> guard(o_lock);

    chrono::duration<double> timeout(600);

    while (true)
    {
        if (o_pool.empty())
        {
            return make_shared<connection_guard>(make_shared<connection>(o_db_type, o_url, o_username, o_password, o_database, o_use_transactions));
        }
        else
        {
            connection_t c = o_pool.front();
            o_pool.pop_front();

            chrono::time_point<chrono::steady_clock> time_now = chrono::steady_clock::now();

            if (!c.conn->can_go_stale() || ((time_now - c.last_used) < timeout))
            {
                c.conn->out_of_pool();
                return make_shared<connection_guard>(c.conn);
            }
            else
            {
                try     
                {
                    c.conn->close();
                }
                catch (...)
                {
                    LOG(warning) << "the database threw some junk when closing a connection, ignored";
                }
            }
        }
    }
}

void connection_pool::put_connection(shared_ptr<connection> &conn)
{
    connection_t c;

    c.last_used = chrono::steady_clock::now();
    c.conn = conn;
    conn->into_pool();

    lock_guard<mutex> guard(o_lock);
    o_pool.push_back(c);
}
