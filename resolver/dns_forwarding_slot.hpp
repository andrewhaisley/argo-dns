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
