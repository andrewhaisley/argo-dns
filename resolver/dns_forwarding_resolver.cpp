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
#include "types.hpp"
#include "dns_forwarding_resolver.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

dns_forwarding_resolver::dns_forwarding_resolver(const server_config &sc)
{
    dns_forwarding_slot::params_t p(sc, m_slot_manager);
    m_slot_pool = new handler_pool<dns_message_envelope *, dns_forwarding_slot, dns_forwarding_slot::params_t>(
            "forwarding slot pool",
            p,
            sc.dns.max_in_message_queue_length,
            sc.dns.max_out_message_queue_length,
            sc.dns.num_udp_recursive_slots);
}

dns_forwarding_resolver::~dns_forwarding_resolver()
{
    delete m_slot_pool;
}

void dns_forwarding_resolver::enqueue(dns_message_envelope *m)
{
    try
    {
        // for peformance, the already_resolving method will add the query to the
        // queue for a given slot if it's not there already. A false return says
        // we also need to queue it up for the handler pool.
        if (!m_slot_manager.already_resolving(m))
        {
            if (!m_slot_pool->enqueue(m))
            {
                delete m;
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

dns_message_envelope *dns_forwarding_resolver::dequeue()
{
    return m_slot_pool->dequeue();
}

void dns_forwarding_resolver::join()
{
    m_slot_pool->join();
}
