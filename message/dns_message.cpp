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
#include "dns_message.hpp"
#include "util.hpp"

using namespace adns;
using namespace std;
using namespace util;

dns_message::~dns_message()
{
}

dns_message::dns_message()
{
    m_id = 0;
    m_type = query_e;
    m_payload_size = STANDARD_MESSAGE_SIZE;
    m_op_code = op_query_e;
    m_is_authoritative = false;
    m_is_truncated = false;
    m_is_recursion_desired = false;
    m_is_recursion_available = false;
    m_response_code = no_error_e;
    m_an_count = 0;
    m_ns_count = 0;
    m_ar_count = 0;
    m_is_edns = false;
    m_edns_version = 0;
    m_is_dnssec_ok = false;
    m_min_ttl = -1;
}

void dns_message::dump() const
{
    LOG(debug) << "message contents...";
    LOG(debug) << "message id:" << m_id;

    switch (m_type)
    {
        case query_e:
            LOG(debug) << "type:query"; break;
        case response_e:
            LOG(debug) << "type:response"; break;
        default:
            LOG(debug) << "type:invalid";
    }

    switch (m_op_code)
    {
        case op_query_e:
            LOG(debug) << "op-code:query"; break;
        case op_inverse_query_e:
            LOG(debug) << "op-code:inverse query"; break;
        case op_status_e:
            LOG(debug) << "op-code:status"; break;
        case op_notify_e:
            LOG(debug) << "op-code:notify"; break;
        case op_update_e:
            LOG(debug) << "op-code:update"; break;
        default:
            LOG(debug) << "op-code:invalid";
    }

    LOG(debug) << "is authoritative:" << bool_to_string(m_is_authoritative);
    LOG(debug) << "is truncated:" << bool_to_string(m_is_truncated);
    LOG(debug) << "is recursion desired:" << bool_to_string(m_is_recursion_desired);
    LOG(debug) << "is recursion available:" << bool_to_string(m_is_recursion_available);

    switch (m_response_code)
    {
        case no_error_e:
            LOG(debug) << "response-code:no error"; break;
        case format_error_e:
            LOG(debug) << "response-code:format error"; break;
        case server_failure_e:
            LOG(debug) << "response-code:server failure"; break;
        case name_error_e:
            LOG(debug) << "response-code:name error"; break;
        case not_implemented_e:
            LOG(debug) << "response-code:not implemented"; break;
        case refused_e:
            LOG(debug) << "response-code:refused"; break;
        default:
            LOG(debug) << "response-code:invalid " << static_cast<int>(m_response_code);
    }

    if (!m_question)
    {
        LOG(debug) << "no question found";
    }
    else
    {
        LOG(debug) << "QUESTION IS " << *m_question;
    }

    LOG(debug) << "an count:" << m_an_count;
    for (auto iter : m_answers)
    {
        LOG(debug) << *iter;
    }

    LOG(debug) << "ns count:" << m_ns_count;
    for (auto iter : m_nameservers)
    {
        LOG(debug) << *iter;
    }

    LOG(debug) << "ar count:" << m_ar_count;
    for (auto iter : m_additional_records)
    {
        LOG(debug) << *iter;
    }

    LOG(debug) << "is EDNS:" << bool_to_string(m_is_edns);
    if (m_is_edns)
    {
        LOG(debug) << "EDNS version:" << static_cast<int>(m_edns_version);
        LOG(debug) << "EDNS payload:" << m_payload_size;
        LOG(debug) << "DNSSEC OK:" << bool_to_string(m_is_dnssec_ok);
    }
}

unsigned short int dns_message::get_id() const
{
    return m_id;
}

dns_message::type_t dns_message::get_type() const
{
    return m_type;
}

dns_message::op_code_t dns_message::get_op_code() const
{
    return m_op_code;
}

bool dns_message::get_is_authoritative() const
{
    return m_is_authoritative;
}

bool dns_message::get_is_truncated() const
{
    return m_is_truncated;
}

bool dns_message::get_is_recursion_desired() const
{
    return m_is_recursion_desired;
}

bool dns_message::get_is_recursion_available() const
{
    return m_is_recursion_available;
}

bool dns_message::get_is_edns() const
{
    return m_is_edns;
}

bool dns_message::get_is_dnssec_ok() const
{
    return m_is_dnssec_ok;
}

octet dns_message::get_edns_version() const
{
    return m_edns_version;
}

dns_message::response_code_t dns_message::get_response_code() const
{
    return m_response_code;
}

unsigned short dns_message::get_an_count() const
{
    return m_an_count;
}

unsigned short dns_message::get_ns_count() const
{
    return m_ns_count;
}

unsigned short dns_message::get_ar_count() const
{
    return m_ar_count;
}

void dns_message::set_id(unsigned short int id)
{
    m_id = id;
}

void dns_message::set_type(type_t t)
{
    m_type = t;
}

void dns_message::set_op_code(op_code_t oc)
{
    m_op_code = oc;
}

void dns_message::set_response_code(response_code_t rc)
{
    m_response_code = rc;
}

void dns_message::set_question(const shared_ptr<dns_question> &q)
{
    m_question = q;
}

void dns_message::set_is_authoritative(bool b)
{
    m_is_authoritative = b;
}

void dns_message::set_is_truncated(bool b)
{
    m_is_truncated = b;
}

void dns_message::set_is_recursion_desired(bool b)
{
    m_is_recursion_desired = b;
}

void dns_message::set_is_recursion_available(bool b)
{
    m_is_recursion_available = b;
}

const shared_ptr<dns_question> &dns_message::get_question() const
{
    return m_question;
}

unsigned short dns_message::get_edns_payload_size() const
{
    return m_payload_size;
}

void dns_message::set_edns(bool b, unsigned short payload_size)
{
    m_is_edns = b;
    m_edns_version = EDNS_VERSION;
    m_payload_size = payload_size;
}

const list<shared_ptr<dns_rr>> &dns_message::get_answers() const
{
    return m_answers;
}

const list<shared_ptr<dns_rr>> &dns_message::get_nameservers() const
{
    return m_nameservers;
}

const list<shared_ptr<dns_rr>> &dns_message::get_additional_records() const
{
    return m_additional_records;
}

size_t dns_message::get_num_answers(dns_rr::type_t t) const
{
    return get_num_records(t, m_answers);
}

size_t dns_message::get_num_nameservers(dns_rr::type_t t) const
{
    return get_num_records(t, m_nameservers);
}

size_t dns_message::get_num_additional_records(dns_rr::type_t t) const
{
    return get_num_records(t, m_additional_records);
}

bool dns_message::answer_exists(dns_rr::type_t t) const
{
    return record_exists(t, m_answers);
}

bool dns_message::nameserver_exists(dns_rr::type_t t) const
{
    return record_exists(t, m_nameservers);
}

bool dns_message::additional_record_exists(dns_rr::type_t t) const
{
    return record_exists(t, m_additional_records);
}

shared_ptr<dns_rr> dns_message::get_first_answer(dns_rr::type_t t) const
{
    for (auto rr : m_answers)
    {
        if (rr->get_type() == t)
        {
            return rr;
        }
    }
    THROW(message_exception, "no answer record of the given type found", t);
}

size_t dns_message::get_num_records(dns_rr::type_t t, const std::list<std::shared_ptr<dns_rr>> &rrs) const
{
    size_t res = 0;
    for (auto rr : rrs)
    {
        if (rr->get_type() == t)
        {
            res++;
        }
    }
    return res;
}

bool dns_message::record_exists(dns_rr::type_t t, const std::list<std::shared_ptr<dns_rr>> &rrs) const
{
    for (auto rr : rrs)
    {
        if (rr->get_type() == t)
        {
            return true;
        }
    }
    return false;
}

list<shared_ptr<dns_rr>> dns_message::get_answers(dns_rr::type_t t) const
{
    list<shared_ptr<dns_rr>> res;

    for (auto rr : m_answers)
    {
        if (rr->get_type() == t)
        {
            res.push_back(rr);
        }
    }
    return res;
}

size_t dns_message::get_num_answers() const
{
    return m_answers.size();
}

size_t dns_message::get_num_nameservers() const
{
    return m_nameservers.size();
}

size_t dns_message::get_num_additional_records() const
{
    return m_additional_records.size();
}

int dns_message::get_min_ttl() const
{
    return m_min_ttl;
}
