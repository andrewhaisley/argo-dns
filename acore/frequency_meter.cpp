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
