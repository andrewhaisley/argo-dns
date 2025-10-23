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
