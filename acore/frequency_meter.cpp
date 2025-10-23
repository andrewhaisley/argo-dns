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
 
#include "frequency_meter.hpp"

using namespace std;
using namespace adns;

frequency_meter::frequency_meter(int period_seconds)
{
    m_period_seconds = period_seconds;
    m_event_count = new int[period_seconds];
    m_frequency = 0;
    m_last_event_time = time(NULL);
    
    for (int i = 0; i < m_period_seconds; i++)
    {
        m_event_count[i] = 0;   
    }
}

frequency_meter::~frequency_meter()
{
    delete [] m_event_count;
}

int frequency_meter::event(int n)
{
    lock_guard<mutex> guard(m_lock);

    time_t now = time(NULL);

    if (m_last_event_time != now)
    {
        m_frequency -= m_event_count[now % m_period_seconds];
        m_event_count[now % m_period_seconds] = 0;
    }

    while (m_last_event_time < (now - m_period_seconds))
    {
        m_frequency -= m_event_count[m_last_event_time % m_period_seconds];
        m_event_count[m_last_event_time % m_period_seconds] = 0;
        m_last_event_time++;
    }

    m_last_event_time = now;
    m_event_count[m_last_event_time % m_period_seconds] += n;
    m_frequency += n;

    return m_frequency;
}

int frequency_meter::frequency()
{
    return event(0);
}
