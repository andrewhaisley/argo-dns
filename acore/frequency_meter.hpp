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
#pragma once 

#include <time.h>
#include <mutex>

#include "types.hpp"

namespace adns
{
    /**
     * A class for counting how often something happens. Resolution is 1 second.
     */
    class frequency_meter
    {
    public:

        /**
         * period_seconds - how many seconds back into the past should we look?
         */
        frequency_meter(int period_seconds);

        /**
         * destructor 
         */
        virtual ~frequency_meter();

        /**
         * mark the occurence of an event
         */
        int event(int n = 1);

        /**
         * How many times did the event happen during the period we track?
         */
        int frequency();

    private:

        /// how far back in the past we look
        int m_period_seconds;

        /// for each second in our total period how many times did the event happen
        int *m_event_count;

        /// time the last event occurred
        time_t m_last_event_time;

        /// frequency - may be out of date
        int m_frequency;

        /// lock
        std::mutex m_lock;
    };
}
