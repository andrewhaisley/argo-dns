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

#include <thread>
#include <mutex>

#include "types.hpp"
#include "server_config.hpp"
#include "socket_address.hpp"
#include "message_queue.hpp"
#include "handler_pool.hpp"
#include "exception.hpp"
#include "frequency_meter.hpp"

#include "dns_message.hpp"
#include "dns_message_envelope.hpp"
#include "dns_recursive_slot.hpp"
#include "dns_recursive_slot_manager.hpp"

EXCEPTION_CLASS(recursive_resolver_fail_exception, exception)

namespace adns
{
    /**
     * recursive DNS resolver. Sends asynchronous responses to requests.
     */
    class dns_recursive_resolver final
    {
    public:

        /**
         * default constructor
         */
        dns_recursive_resolver(const server_config &server_config, bool is_tcp);

        /**
         *  destructor
         */
        virtual ~dns_recursive_resolver();

        /**
         * join running threads
         */
        void join();

        /**
         * put a new question onto the request queue for resolution
         */
        void enqueue(dns_message_envelope *q);

        /**
         * get a response from a previous question 
         */
        dns_message_envelope *dequeue();

        /**
         * for things that need it, adds a mutliplexer channel - don't mix with
         * enqueue/dequeue or confusing things will happen.
         */
        void add_channel(
                std::shared_ptr<message_queue<dns_message_envelope *>> resolver_in, 
                std::shared_ptr<message_queue<dns_message_envelope *>> resolver_out);

    private:

        // pool of recursive resolver slots
        handler_pool<dns_message_envelope *, dns_recursive_slot, dns_recursive_slot::params_t> *m_slot_pool;

        // recursive slot manager - records which slot is resolving
        // what.
        dns_recursive_slot_manager m_slot_manager;

        // general config
        server_config m_config;

        // used to measure transactions per second averaged over 10 seconds
        std::shared_ptr<frequency_meter> m_meter;

        // thread to update the frequency monitor
        std::shared_ptr<std::thread> m_monitor_thread;

        // ID of the monitor for TPS
        uint m_monitor_tps_id;

        /**
         * monitor TPS 
         */
        void monitor_tps();

        /**
         * put a response on the queue for later de-queuing and return to the client
         */
        void send_response(dns_message_envelope *m);

        /**
         * main processing loop - one per thread
         */
        void process();
    };
}
