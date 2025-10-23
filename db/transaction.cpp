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
 
#include "transaction.hpp"

using namespace std;
using namespace adns;
using namespace db;

transaction::transaction(connection &c):
            m_connection(c),
            m_committed(false),
            m_rolled_back(false)
{
    m_connection.begin_transaction();
}

transaction::~transaction()
{
    if (!m_committed && !m_rolled_back)
    {
        rollback();
    }
}

void transaction::commit()
{
    if (m_committed)
    {
        THROW(transaction_exception, "attempt to commit a transaction more than once");
    }
    else if (m_rolled_back)
    {
        THROW(transaction_exception, "attempt to commit a previously rolled back transaction");
    }
    else
    {
        m_connection.commit();
        m_committed = true;
    }
}

void transaction::rollback()
{
    if (m_committed)
    {
        THROW(transaction_exception, "attempt to rollback a previously committed transaction");
    }
    else if (m_rolled_back)
    {
        THROW(transaction_exception, "attempt to rollback a transaction more than once");
    }
    else
    {
        m_connection.rollback();
        m_rolled_back = true;
    }
}
