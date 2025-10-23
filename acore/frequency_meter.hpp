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
