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

#include <mutex>
#include <unordered_map>

#include "types.hpp"

#include "dns_message_envelope.hpp"

namespace adns
{
    /**
     * Keep track of whether a slot is currently handling a specific question.
     * This stops loads of parallel requests to the same external servers
     * with the same question under heavy load scenarios.
     */
    class dns_recursive_slot_manager
    {
    public:

        // constructor
        dns_recursive_slot_manager();

        // destructor
        virtual ~dns_recursive_slot_manager();

        // is a slot already working on a question?
        bool already_resolving(dns_message_envelope *m);

        /**
         * Clear the list of queries waiting on a specific question.
         * @return  the waiting list of queries
         */
        std::list<dns_message_envelope*> clear_waiters(const dns_question &q);

    private:
        std::unordered_map<dns_question, std::list<dns_message_envelope *>> m_waiters;
        std::mutex m_mutex;
    };
}
