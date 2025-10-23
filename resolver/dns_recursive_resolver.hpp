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
