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
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <boost/lexical_cast.hpp>

#include "util.hpp"
#include "config.hpp"
#include "dns_handler.hpp"
#include "dns_zone.hpp"
#include "dns_zone_guard.hpp"
#include "run_state.hpp"
#include "dns_horizon.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

dns_handler::dns_handler(const params_t &params, message_queue<dns_message_envelope *> &q) :
            m_config(params.config), 
            m_auth_resolver(params.p_auth_resolver),
            m_recursive_resolver(params.p_recursive_resolver),
            m_forwarding_resolver(params.p_forwarding_resolver),
            m_out_queue(q),
            m_check_message_length(params.check_message_length),
            m_is_tcp(params.is_tcp)
{
    if (m_is_tcp)
    {
        m_to_resolver = make_shared<message_queue<dns_message_envelope *>>(
                                "to recursive resolver",
                                m_config.dns.max_in_message_queue_length);

        m_from_resolver = make_shared<message_queue<dns_message_envelope *>>(
                                "from recursive resolver",
                                m_config.dns.max_out_message_queue_length);

        m_recursive_resolver->add_channel(m_to_resolver, m_from_resolver);
    }
    m_resolver_thread = make_shared<thread>(&dns_handler::get_resolver_responses, this);
    m_forwarder_thread = make_shared<thread>(&dns_handler::get_forwarder_responses, this);
}

dns_handler::~dns_handler()
{
}

dns_message_envelope *dns_handler::dequeue_from_resolver()
{
    if (m_is_tcp)
    {
        return m_from_resolver->dequeue();
    }
    else
    {
        return m_recursive_resolver->dequeue();
    }
}

void dns_handler::enqueue_to_resolver(dns_message_envelope *m)
{
    if (m_is_tcp)
    {
        if (!m_to_resolver->enqueue(m))
        {
            delete m;
        }
    }
    else
    {
        m_recursive_resolver->enqueue(m);
    }
}

void dns_handler::enqueue_to_return(dns_message_envelope *m)
{
    if (!m_out_queue.enqueue(m))
    {
        delete m;
    }
}

void dns_handler::get_resolver_responses()
{
    while (true)
    {
        try
        {
            try
            {
                send_response(dequeue_from_resolver(), true);
            }
            catch (message_queue_timeout_exception &e)
            {
                if (run_state::o_state == run_state::shutdown_e)
                {
                    return;
                }
            }
        }
        catch (adns::exception &e)
        {
            e.log(error, "caught exception in get_resolver_responses()");
        }
    }
}

void dns_handler::get_forwarder_responses()
{
    while (true)
    {
        try
        {
            try
            {
                send_response(m_forwarding_resolver->dequeue(), true);
            }
            catch (message_queue_timeout_exception &e)
            {
                if (run_state::o_state == run_state::shutdown_e)
                {
                    return;
                }
            }
        }
        catch (adns::exception &e)
        {
            e.log(error, "caught exception in get_forwarder_responses()");
        }
    }
}

void dns_handler::respond_with_parse_error(dns_message_envelope *m, dns_message::response_code_t rc)
{
    dns_message r;

    r.set_id(m_parser.extract_id(m->get_raw()));
    r.set_type(dns_message::response_e);
    r.set_op_code(m_parser.extract_opcode(m->get_raw()));
    r.set_response_code(rc);

    m->set_raw(m_parser.to_wire(r));

    enqueue_to_return(m);
}

void dns_handler::respond_with_error(
                dns_message_envelope         *m, 
                bool                         allow_recursion,
                dns_message::response_code_t rc)
{
    dns_message r;

    auto request = m->get_request();

    r.set_id(request->get_id());
    r.set_type(dns_message::response_e);
    r.set_op_code(request->get_op_code());
    r.set_response_code(rc);
    r.set_is_recursion_desired(request->get_is_recursion_desired());
    r.set_is_recursion_available(allow_recursion);

    if (request->get_question())
    {
        r.set_question(request->get_question());
    }

    if (request->get_is_edns())
    {
        r.set_edns(true, config::edns_size());
    }

    m->set_raw(m_parser.to_wire(r));

    enqueue_to_return(m);
}

bool dns_handler::feature_not_supported(dns_message *request, string &notes)
{
    if (!request->get_question())
    {
        notes = "only requests with exactly one question are supported";
        return true;
    }

    // RFC 3425 obsoleted inverse queries. 
    // Status queries never seemed to be defined anywhere - no accepted RFC.
    // We don't support notifications.
    // Update will be implemented in due course.
    if (request->get_op_code() != dns_message::op_query_e)
    {
        notes = "unsupported op code type " + boost::lexical_cast<string>(request->get_op_code());
        return true;
    }

    // we only support IN class queries at the moment - might add some CH class ones for
    // status requests etc.
    if (request->get_question()->get_qclass() != dns_question::QC_IN_e)
    {
        notes = "only IN query classes are supported, got request for " + boost::lexical_cast<string>(request->get_question()->get_qclass());
        return true;
    }

    return false;
}

void dns_handler::process(dns_message_envelope *m)
{
    dns_horizon *h = dns_horizon::get(m->get_remote_address().get_ip_address());

    if (!h)
    {
        // black hole requests that don't match any horizons
        delete m;
        return;
    }

    try
    {
        dns_message *pm = nullptr;

        try
        {
            pm = m_parser.from_wire(m->get_raw());
            m->set_request(pm);
        }
        catch (...)
        {
            delete pm;
            throw;
        }
    }
    catch (message_parser_format_exception &e)
    {
        e.log(info, "exception whilst parsing request");
        respond_with_parse_error(m, dns_message::format_error_e);
        return;
    }
    catch (adns::exception &e)
    {
        e.log(error, "unexpected exception whilst parsing request");
        respond_with_parse_error(m, dns_message::server_failure_e);
        return;
    }

    string notes;

    if (feature_not_supported(m->get_request(), notes))
    {
        LOG(info) << notes;
        respond_with_error(m, h->allow_recursion(), dns_message::not_implemented_e);
        return;
    }

    if (m->get_request()->get_type() == dns_message::response_e)
    {
        LOG(warning) << "got a response on the main server port - potential cache poisoning attempt";
        delete m;
        return;
    }
    else
    {
        handle_request(m, h);
    }
}

void dns_handler::handle_request(dns_message_envelope *m, const dns_horizon *h)
{
    auto request = m->get_request();
    auto remote_address = m->get_remote_address();

    if (request->get_is_edns() && request->get_edns_version() > EDNS_VERSION)
    {
        respond_with_error(m, h->allow_recursion(), dns_message::badvers_e);
        return;
    }

    try
    {
        auto response = make_shared<dns_message>();

        dns_zone_guard guard(dns_zone_guard::read_e);

        auto z = dns_zone::find(h->get_horizon_id(), request->get_question()->get_qname());
        auto qt = request->get_question()->get_qtype();

        if (z)
        {
            if (z->get_is_forwarded())
            {
                if (qt == dns_question::QT_IXFR_e || qt == dns_question::QT_AXFR_e)
                {
                    respond_with_error(m, h->allow_recursion(), dns_message::refused_e);
                }
                else
                {
                    m->set_forwarding(z->get_forward_ip_address(), z->get_forward_port());
                    m_forwarding_resolver->enqueue(m);
                }
            }
            else
            {
                if (request->get_op_code() == dns_message::op_query_e)
                {
                    if (qt == dns_question::QT_IXFR_e || qt == dns_question::QT_AXFR_e)
                    {
                        respond_with_error(m, h->allow_recursion(), dns_message::refused_e);
                    }
                    else
                    {
                        m->set_response(m_auth_resolver->resolve(*h, request, z));
                        send_response(m, h->allow_recursion());
                    }
                }
                else
                {
                    respond_with_error(m, h->allow_recursion(), dns_message::not_implemented_e);
                }
            }
        }
        else if (h->allow_recursion() && request->get_is_recursion_desired())
        {
            if (qt == dns_question::QT_IXFR_e || qt == dns_question::QT_AXFR_e)
            {
                respond_with_error(m, h->allow_recursion(), dns_message::refused_e);
            }
            else
            {
                LOG(info) << "resolving " << *(request->get_question()) << " for " << m->get_remote_address();
                enqueue_to_resolver(m);
            }
        }
        else
        {
            respond_with_error(m, h->allow_recursion(), dns_message::refused_e);
        }
    }
    catch (auth_resolver_not_impl_exception &e)
    {
        e.log(error, "exception whilst answering question");
        respond_with_error(m, h->allow_recursion(), dns_message::not_implemented_e);
    }
    catch (auth_resolver_fail_exception &e)
    {
        e.log(error, "exception whilst answering authoritative query");
        respond_with_error(m, h->allow_recursion(), dns_message::server_failure_e);
    }
    catch (recursive_resolver_fail_exception &e)
    {
        e.log(error, "exception whilst answering recursive query");
        respond_with_error(m, h->allow_recursion(), dns_message::server_failure_e);
    }
    catch (adns::exception &e)
    {
        e.log(error, "unexpected exception whilst answering question");
        respond_with_error(m, h->allow_recursion(), dns_message::server_failure_e);
    }
}

void dns_handler::send_response(dns_message_envelope *m, bool allow_recursion)
{
    // a null response is valid - it means don't send anything back
    auto response = m->get_response();

    if (!response)
    {
        LOG(info) << "null response";
        return;
    }

    auto request = m->get_request();

    try
    {
        if (config::support_edns() && request->get_is_edns())
        {
            response->set_edns(true, config::edns_size());
        }
        else
        {
            response->set_edns(false, STANDARD_MESSAGE_SIZE);
        }

        response->set_is_recursion_desired(request->get_is_recursion_desired());
        response->set_is_recursion_available(allow_recursion);

        buffer raw = m_parser.to_wire(*response);

        bool edns_needed = raw.get_size() > STANDARD_MESSAGE_SIZE;

        // only care about the message length (unless it's > 64K) for UDP
        if (m_check_message_length)
        {
            if (edns_needed)
            {
                if ((raw.get_size() > config::edns_size()) || (raw.get_size() > request->get_edns_payload_size()))
                {
                    m_parser.set_truncated(raw, true);
                    raw.set_size(min(config::edns_size(), request->get_edns_payload_size()));
                    m->set_raw(raw);
                    enqueue_to_return(m);
                }
                else
                {
                    m->set_raw(raw);
                    enqueue_to_return(m);
                }
            }
            else
            {
                m->set_raw(raw);
                enqueue_to_return(m);
            }
        }
        else
        {
            m->set_raw(raw);
            enqueue_to_return(m);
        }
    }
    catch (message_serial_overrun_exception &e)
    {
        // if we've hit here, the message is longer than 64K so can't be retried even using
        // TCP. Log an error and send a server fail since there's not much more that can be 
        // done.
        LOG(error) << "response greater than 64K generated, giving up " << *(request->get_question());
        respond_with_error(m, allow_recursion, dns_message::server_failure_e);
    }
    catch (message_unparser_exception &e)
    {
        e.log(error, "exception unparsing response");
        respond_with_error(m, allow_recursion, dns_message::server_failure_e);
    }
}

void dns_handler::join()
{
    m_resolver_thread->join();
    m_forwarder_thread->join();
}
