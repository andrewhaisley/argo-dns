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
 
#include "dns_recursive_slot_manager.hpp"

using namespace std;
using namespace adns;

dns_recursive_slot_manager::dns_recursive_slot_manager()
{
}

dns_recursive_slot_manager::~dns_recursive_slot_manager()
{
}

bool dns_recursive_slot_manager::already_resolving(dns_message_envelope *m)
{
    lock_guard<mutex> guard(m_mutex);
    auto w = m_waiters.find(*(m->get_request()->get_question()));
    if (w == m_waiters.end())
    {
        m_waiters[*(m->get_request()->get_question())].push_back(m);
        return false;
    }
    else
    {
        w->second.push_back(m);
        return true;
    }
}

list<dns_message_envelope *> dns_recursive_slot_manager::clear_waiters(const dns_question &q) 
{
    lock_guard<mutex> guard(m_mutex);
    auto w = m_waiters.find(q);
    if (w == m_waiters.end())
    {
        return list<dns_message_envelope *>();
    }
    else
    {
        auto res = w->second;
        m_waiters.erase(w);
        return res;
    }
}
