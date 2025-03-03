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
