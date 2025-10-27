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

size_t dns_message::get_num_records(dns_rr::type_t t, const list<shared_ptr<dns_rr>> &rrs) const
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

bool dns_message::record_exists(dns_rr::type_t t, const list<shared_ptr<dns_rr>> &rrs) const
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

void dns_message::update_ttls(list<shared_ptr<dns_rr>> &rrs, int diff)
{
    for (auto r : rrs)
    {
        int ttl = r->get_ttl();
        ttl = std::max(0, ttl + diff);
        r->set_ttl(ttl);
        m_min_ttl = std::min(m_min_ttl, ttl);
    }
}

void dns_message::update_ttl(int diff)
{
    update_ttls(m_answers, diff);
    update_ttls(m_nameservers, diff);
    update_ttls(m_additional_records, diff);
}

void dns_message::set_min_ttl()
{
    m_min_ttl = -1;
    set_min_ttl(m_answers);
    set_min_ttl(m_nameservers);
    set_min_ttl(m_additional_records);
}

void dns_message::set_min_ttl(list<shared_ptr<dns_rr>> &rrs)
{
    for (auto r : rrs)
    {
        if (m_min_ttl == -1)
        {
            m_min_ttl = r->get_ttl();
        }
        else
        {
            m_min_ttl = std::min(m_min_ttl, static_cast<int>(r->get_ttl()));
        }
    }
}

dns_message *dns_message::clone()
{
    auto res = new dns_message;

    res->m_id = m_id;
    res->m_payload_size = m_payload_size;
    res->m_type = m_type;
    res->m_op_code = m_op_code;
    res->m_is_authoritative = m_is_authoritative;
    res->m_is_truncated = m_is_truncated;
    res->m_is_recursion_desired = m_is_recursion_desired;
    res->m_is_recursion_available = m_is_recursion_available;
    res->m_is_edns = m_is_edns;
    res->m_is_dnssec_ok = m_is_dnssec_ok;
    res->m_edns_version = m_edns_version;
    res->m_response_code = m_response_code;
    res->m_an_count = m_an_count;
    res->m_ns_count = m_ns_count;
    res->m_ar_count = m_ar_count;
    res->m_min_ttl = m_min_ttl;

    res->m_question = m_question;

    for (auto r : m_answers)
    {
        res->m_answers.push_back(shared_ptr<dns_rr>(r->clone()));
    }

    for (auto r : m_nameservers)
    {
        res->m_nameservers.push_back(shared_ptr<dns_rr>(r->clone()));
    }

    for (auto r : m_additional_records)
    {
        res->m_additional_records.push_back(shared_ptr<dns_rr>(r->clone()));
    }

    return res;
}
