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

#include "dns_forwarding_cache.hpp"

using namespace std;
using namespace chrono;
using namespace adns;

dns_forwarding_cache::dns_forwarding_cache(int max_age_seconds, int max_entries, int garbage_collect_pct) :
                            m_max_age_seconds(max_age_seconds),
                            m_max_entries(max_entries),
                            m_garbage_collect_pct(garbage_collect_pct)
{
}

dns_forwarding_cache::~dns_forwarding_cache()
{
}

void dns_forwarding_cache::add(const dns_question &q, const shared_ptr<dns_message> &a)
{
    lock_guard<mutex> guard(m_lock);

    m_answers[q] = pair<time_point<steady_clock>, shared_ptr<dns_message>>(steady_clock::now(), a);
}

shared_ptr<dns_message> dns_forwarding_cache::get(const dns_question &q)
{
    lock_guard<mutex> guard(m_lock);

    auto i = m_answers.find(q);

    if (i == m_answers.end())
    {
        return nullptr;
    }
    else
    {

        int seconds = duration_cast<chrono::seconds>(steady_clock::now() - i->second.first).count();
        
        if (seconds > m_max_age_seconds)
        {
            m_answers.erase(q);
            return nullptr;
        }
        else
        {
            return i->second.second;
        }
    }
}
