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
