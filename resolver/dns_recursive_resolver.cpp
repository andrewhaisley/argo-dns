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
#include <boost/lexical_cast.hpp>

#include "run_state.hpp"
#include "ip_address.hpp"
#include "types.hpp"
#include "dns_recursive_resolver.hpp"
#include "socket_set.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

static shared_ptr<dns_recursive_resolver> instance;

dns_recursive_resolver::dns_recursive_resolver(const server_config &sc, bool is_tcp) : m_config(sc)
{
    dns_recursive_slot::params_t p(sc, m_slot_manager);
    m_slot_pool = new handler_pool<dns_message_envelope *, dns_recursive_slot, dns_recursive_slot::params_t>(
            "recursive slot pool",
            p,
            m_config.dns.max_in_message_queue_length,
            m_config.dns.max_out_message_queue_length,
            is_tcp ? m_config.dns.num_tcp_recursive_slots : m_config.dns.num_udp_recursive_slots);
    m_meter = make_shared<frequency_meter>(60);

    m_monitor_tps_id = monitor::add_thing("recursive resolver", boost::lexical_cast<string>(sc.server_id), "TPS", 0);

    // for convenience - makes debug a bit easier to see these in the monitor status
    monitor::add_thing("recursive resolver", boost::lexical_cast<string>(sc.server_id), "protocol", sc.protocol);
    monitor::add_thing("recursive resolver", boost::lexical_cast<string>(sc.server_id), "transport", sc.transport);

    m_monitor_thread.reset(new thread(&dns_recursive_resolver::monitor_tps, this));
}

dns_recursive_resolver::~dns_recursive_resolver()
{
    delete m_slot_pool;
}

void dns_recursive_resolver::monitor_tps()
{
    while (true)
    {
        if (run_state::o_state == run_state::shutdown_e)
        {
            return;
        }

        monitor::set_value(m_monitor_tps_id, m_meter->frequency() / 60);
        sleep(1);
    }
}

void dns_recursive_resolver::join()
{
    m_slot_pool->join();
    m_monitor_thread->join();
}

void dns_recursive_resolver::enqueue(dns_message_envelope *q)
{
    m_meter->event();

    try
    {
        // for peformance, the already_resolving method will add the query to the
        // queue for a given slot if it's not there already. A false return says
        // we also need to queue it up for the handler pool.
        if (!m_slot_manager.already_resolving(q))
        {
            if (!m_slot_pool->enqueue(q))
            {
                delete q;
            }
        }
    }
    catch (adns::exception &e)
    {
        // catch to avoid a core dump but this really should never happen short
        // of running out of memory so don't send a message back.
        e.log(error, "exception whilst resolving");
    }
}

dns_message_envelope *dns_recursive_resolver::dequeue()
{
    return m_slot_pool->dequeue();
}

void dns_recursive_resolver::add_channel(
        shared_ptr<message_queue<dns_message_envelope *>> resolver_in, 
        shared_ptr<message_queue<dns_message_envelope *>> resolver_out)
{
    m_slot_pool->get_multiplexer()->add_channel(resolver_in, resolver_out);
}
