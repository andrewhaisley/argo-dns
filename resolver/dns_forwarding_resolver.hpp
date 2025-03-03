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

#include "types.hpp"
#include "handler_pool.hpp"
#include "server_config.hpp"
#include "dns_message_envelope.hpp"
#include "dns_recursive_slot_manager.hpp"
#include "dns_forwarding_slot.hpp"

namespace adns
{
    class dns_forwarding_resolver final
    {
    public:

        /**
         * constructor
         */
        dns_forwarding_resolver(const server_config &sc);

        /**
         * destructor
         */
        virtual ~dns_forwarding_resolver();

        /**
         * asynchronously resolve a request
         */
        void enqueue(dns_message_envelope *m);

        /**
         * wait for a single response
         */
        dns_message_envelope *dequeue();

        /**
         * join threads 
         */
        void join();

    private:

        // pool of forwarding resolver slots
        handler_pool<dns_message_envelope *, dns_forwarding_slot, dns_forwarding_slot::params_t> *m_slot_pool;

        dns_recursive_slot_manager m_slot_manager;

    };
}
