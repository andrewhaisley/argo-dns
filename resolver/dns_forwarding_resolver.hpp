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
