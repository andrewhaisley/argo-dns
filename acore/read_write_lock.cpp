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
#include "read_write_lock.hpp"

using namespace std;
using namespace adns;


read_write_lock::read_write_lock() : m_num_readers(0), m_num_writers(0), m_num_writers_waiting(0)
{
}

read_write_lock::~read_write_lock()
{
}

void read_write_lock::lock_read()
{
    unique_lock<mutex> guard(m_lock);

    while (m_num_writers > 0 || m_num_writers_waiting > 0)
    {
        m_no_writers.wait(guard);
    }

    m_num_readers++;
}

void read_write_lock::lock_write()
{
    unique_lock<mutex> guard(m_lock);

    m_num_writers_waiting++;

    while (m_num_writers > 0 || m_num_readers > 0)
    {
        m_no_readers_or_writers.wait(guard);
    }

    m_num_writers_waiting--;
    m_num_writers++;
}

void read_write_lock::unlock_read()
{
    unique_lock<mutex> guard(m_lock);

    m_num_readers--;

    if (m_num_readers == 0 && m_num_writers == 0)
    {
        m_no_readers_or_writers.notify_all();
    }
}

void read_write_lock::unlock_write()
{
    unique_lock<mutex> guard(m_lock);

    m_num_writers--;

    if (m_num_writers == 0)
    {
        m_no_writers.notify_all();
        m_no_readers_or_writers.notify_all();
    }
}

read_write_lock::write_guard::write_guard(read_write_lock &l) : m_lock(l)
{
    m_lock.lock_write();
}

read_write_lock::write_guard::~write_guard()
{
    m_lock.unlock_write();
}

read_write_lock::read_guard::read_guard(read_write_lock &l) : m_lock(l)
{
    m_lock.lock_read();
}

read_write_lock::read_guard::~read_guard()
{
    m_lock.unlock_read();
}
