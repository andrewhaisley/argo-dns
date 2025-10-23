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
#include <chrono>

#include "types.hpp"

#include "server_config.hpp"
#include "message_queue.hpp"
#include "dns_message_envelope.hpp"
#include "dns_recursive_slot_manager.hpp"

EXCEPTION_CLASS(forwarding_slot_exception, exception)

namespace adns
{
    class dns_forwarding_slot
    {
    public:

        struct params_t
        {
            params_t(const server_config &config, dns_recursive_slot_manager &rsm);

            server_config              m_config;
            dns_recursive_slot_manager &m_slot_manager;
        };

        /**
         * constructor - processes updates as part of a handler pool
         */
        dns_forwarding_slot(const params_t &params, message_queue<dns_message_envelope *> &queue);

        /** 
         * destructor
         */
        virtual ~dns_forwarding_slot();

        /**
         * wait for any running threads to join
         */
        void join();

        /**
         * forward a single query
         */
        void process(dns_message_envelope *m);

        /**
         * send a response to any waiters on the slot
         */
        virtual void send_response(dns_message_envelope *e);


    private:

        // parameters - e.g. the slot manager
        params_t m_params;

        // outgoing message queue 
        message_queue<dns_message_envelope *> &m_out_queue;

    };
}
