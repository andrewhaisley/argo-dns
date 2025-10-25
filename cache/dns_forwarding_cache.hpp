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

#include <unordered_map>
#include <chrono>
#include <thread>

#include "types.hpp"
#include "dns_question.hpp"
#include "dns_message.hpp"

namespace adns
{
    /**
     * a cache of resolver answers kept for a defined time
     */
    class dns_forwarding_cache
    {
    public:
        /**
         * create a new cache.
         * @param max_age_seconds     - maximum time a cache entry is valid
         * @param max_entries         - maximum number of cache entries
         * @param garbage_collect_pct - when the maximum number of entries is reached, remove 
         *                            - all entries greater than max time old and then the oldest
         *                            - in order until this % is met
         */
        dns_forwarding_cache(
                    int max_age_seconds,
                    int max_entries,
                    int garbage_collect_pct);

        virtual ~dns_forwarding_cache();

        /**
         * add a new entry, garbage collecting as needed
         */
        void add(const dns_question &q, const std::shared_ptr<dns_message> &a);

        /**
         * find an existing entry, return nullptr if none matches. 
         */
        std::shared_ptr<dns_message> get(const dns_question &q);
        
        /**
         * run unit tests
         */
        static void test();

    private:

        int m_max_age_seconds;
        int m_max_entries;
        int m_garbage_collect_pct;

        std::mutex m_lock;

        std::shared_ptr<std::unordered_map<
                dns_question, 
                std::pair<
                    std::chrono::time_point<std::chrono::steady_clock>, 
                    std::shared_ptr<dns_message>
                >>> m_answers;

        /**
         * make space in the cache. This proceeds as follows:
         *   until number of entries <= max_extries * garbage_collect_pct / 100:
         *     remove any entry older than m_max_entries
         *     remove the oldest entries
         */
        void garbage_collect();

    };
}
