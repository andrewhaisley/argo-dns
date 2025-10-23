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

    LOG(debug) << "forwarding slot address to use " << m->get_forwarding_address() << ":" << m->get_forwarding_port();

    client_config config;

    // TODO: read this from the DB instead of hard coding it all
    config.use_ip4 = true;
    config.use_ip6 = false;
    config.use_udp = true;
    config.use_tcp = false;
    config.num_parallel_udp = 1;
    config.total_timeout_ms = 1000;
    config.udp_timeout_ms = 1000;
    config.wait_udp_response_ms = 1000;
    config.connect_tcp_timeout_ms = 0;
    config.read_tcp_timeout_ms = 0;
    config.write_tcp_timeout_ms = 0;

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
        LOG(warning) << "exception contacting forwarder";
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

