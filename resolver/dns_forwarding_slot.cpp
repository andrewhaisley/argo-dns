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
#include "dns_forwarding_slot.hpp"
#include "dns_parallel_client.hpp"
#include "client_config.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;



dns_forwarding_slot::params_t::params_t(const server_config &config, dns_recursive_slot_manager &rsm) : 
                m_config(config),
                m_slot_manager(rsm)
{
}

dns_forwarding_slot::dns_forwarding_slot(const params_t &params, message_queue<dns_message_envelope *> &queue) :
                m_params(params),
                m_out_queue(queue)
{
}

dns_forwarding_slot::~dns_forwarding_slot()
{
}

void dns_forwarding_slot::join()
{
}

void dns_forwarding_slot::process(dns_message_envelope *m)
{
    auto id = m->get_request()->get_id();

    client_config config;

    // override the port as that is set per zone.
    config.server_port = m->get_forwarding_port();

    dns_parallel_client c(config);
    c.set_ip_address(m->get_forwarding_address());

    try
    {
        dns_message qm;
        qm.set_type(dns_message::query_e);
        qm.set_op_code(dns_message::op_query_e);
        qm.set_is_recursion_desired(m->get_request()->get_is_recursion_desired());
        qm.set_question(m->get_request()->get_question());

        auto res = c.query(qm);

        if (res)
        {
            res->set_id(id);
            m->set_response(res);
            send_response(m);
        }
        else
        {
            LOG(warning) << "no response from forwarder";
        }
    }
    catch (adns::exception &e)
    {
        LOG(warning) << "no response from forwarder";
    }
}

void dns_forwarding_slot::send_response(dns_message_envelope *m)
{
    auto w = m_params.m_slot_manager.clear_waiters(*(m->get_request()->get_question()));

    if (w.size() == 0)
    {
        if (!m_out_queue.enqueue(m))
        {
            delete m;
        }
    }
    else
    {
        for (auto q : w)
        {
            dns_message_envelope *copy_envelope = new dns_message_envelope(*m);
            copy_envelope->get_response()->set_id(q->get_request()->get_id());
            if (!m_out_queue.enqueue(copy_envelope))
            {
                delete copy_envelope;
            }
        }

        for (auto q : w)
        {
            delete q;
        }
    }
}

