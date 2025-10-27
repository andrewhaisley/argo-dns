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

#include <vector>
#include <algorithm>
#include <format>

#include "dns_forwarding_cache.hpp"

using namespace std;
using namespace chrono;
using namespace adns;

dns_forwarding_cache::dns_forwarding_cache(int max_age_seconds, int max_entries, int garbage_collect_pct) :
                            m_max_age_seconds(max_age_seconds),
                            m_max_entries(max_entries),
                            m_garbage_collect_pct(garbage_collect_pct)
{
    m_answers = make_shared<
                std::unordered_map<
                    dns_question, 
                    std::pair<
                        std::chrono::time_point<std::chrono::steady_clock>, 
                        std::shared_ptr<dns_message>
                    >>>();
}

dns_forwarding_cache::~dns_forwarding_cache()
{
}

void dns_forwarding_cache::add(const dns_question &q, const shared_ptr<dns_message> &a)
{
    lock_guard<mutex> guard(m_lock);

    a->set_min_ttl();

    (*m_answers)[q] = pair<time_point<steady_clock>, shared_ptr<dns_message>>(steady_clock::now(), a);

    if (m_answers->size() >= static_cast<size_t>(m_max_entries))
    {
        garbage_collect();
    }
}

void dns_forwarding_cache::garbage_collect()
{
    size_t target = (m_max_entries * m_garbage_collect_pct) / 100;

    if (m_answers->size() < target)
    {
        return;
    }

    vector<chrono::time_point<chrono::steady_clock>> times;

    transform(m_answers->begin(), m_answers->end(), back_inserter(times), [](const auto& pair) { return pair.second.first; });

    sort(times.begin(), times.end());

    auto now = steady_clock::now();

    while (duration_cast<chrono::seconds>(now - times[target]).count() > m_max_age_seconds)
    {
        target--;
    }

    if (target <= 0)
    {
        m_answers->clear();
    }
    else
    {
        auto oldest = times[times.size() - target];

        auto new_answers = make_shared<
                std::unordered_map<
                    dns_question, 
                    std::pair<
                        std::chrono::time_point<std::chrono::steady_clock>, 
                        std::shared_ptr<dns_message>
                    >>>();

        for (auto i : *m_answers)
        {
            if (i.second.first > oldest)
            {
                (*new_answers)[i.first] = i.second;
            }
        }
    
        m_answers = new_answers;
    }
}

shared_ptr<dns_message> dns_forwarding_cache::get(const dns_question &q, int &age_seconds)
{
    lock_guard<mutex> guard(m_lock);

    age_seconds = 0;

    auto i = m_answers->find(q);

    if (i == m_answers->end())
    {
        return nullptr;
    }
    else
    {

        age_seconds = duration_cast<chrono::seconds>(steady_clock::now() - i->second.first).count();
        
        if (age_seconds > m_max_age_seconds)
        {
            m_answers->erase(q);
            return nullptr;
        }
        else
        {
            return i->second.second;
        }
    }
}

void dns_forwarding_cache::test()
{
    dns_forwarding_cache c(100, 100, 90);

    for (int i = 0; i < 101; i++)
    {
        char n[100];
        sprintf(n, "name%d.com", i);

        dns_name name(n);
        dns_question q(name);

        c.add(q, make_shared<dns_message>());
        usleep(100);
    }

    // check that everything older that 9 is gone and everything younger than 11 is still there - bit of leaway for 
    // timing variability
    for (int i = 0; i < 9; i++)
    {
        char n[100];
        sprintf(n, "name%d.com", i);

        dns_name name(n);
        dns_question q(name);

        if (c.m_answers->find(q) != c.m_answers->end()) 
        {
            LOG(debug) << "test failed - found " << n;
            return;
        }
    }

    for (int i = 12; i < 100; i++)
    {
        char n[100];
        sprintf(n, "name%d.com", i);

        dns_name name(n);
        dns_question q(name);

        if (c.m_answers->find(q) == c.m_answers->end()) 
        {
            LOG(debug) << "test failed - didn't find " << n;
            return;
        }
    }

    LOG(debug) << "dns_forwarding_cache test passed";
}
