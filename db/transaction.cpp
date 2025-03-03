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
