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
