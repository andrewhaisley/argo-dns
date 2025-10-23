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
