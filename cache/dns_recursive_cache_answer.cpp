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
#include <set>

#include "dns_rr_SOA.hpp"
#include "dns_recursive_cache_answer.hpp"

using namespace std;
using namespace adns;

atomic<size_t> dns_recursive_cache_answer::o_sequence = 0;

dns_recursive_cache_answer::dns_recursive_cache_answer(const dns_recursive_cache_answer &other) :
        m_sequence(o_sequence++),
        m_num_records(other.m_num_records),
        m_type(other.m_type),
        m_referral_name(other.m_referral_name),
        m_answer_rr_strings(other.m_answer_rr_strings),
        m_create_time(chrono::steady_clock::now()),
        m_min_ttl(other.m_min_ttl),
        m_answer_records(other.m_answer_records),
        m_ns_records(other.m_ns_records),
        m_additional_records(other.m_additional_records)
{
}

dns_recursive_cache_answer::dns_recursive_cache_answer(
                    type_t t, 
                    uint   default_ttl,
                    const list<shared_ptr<dns_rr>> &answer_records,
                    const list<shared_ptr<dns_rr>> &ns_records,
                    const list<shared_ptr<dns_rr>> &additional_records) :
                            m_sequence(o_sequence++),
                            m_num_records(0),
                            m_type(t),
                            m_create_time(chrono::steady_clock::now()),
                            m_min_ttl(chrono::duration<double>(default_ttl)),
                            m_answer_records(answer_records),
                            m_additional_records(additional_records)
{
    bool min_set = false;

    update_min_ttl(min_set, m_answer_records);
    update_min_ttl(min_set, m_ns_records);
    update_min_ttl(min_set, m_additional_records);

    // if there are no records at all in the answer, then we go with a default TTL
    // this is only really relevant in the case of NXDOMAIN or empty responses but, if the 
    // nameserver in question follows the rules, we'll at least have an SOA for those which
    // will have an appropriate timeout set.


    if (t == referral_e)
    {
        set<dns_name> referral_names;

        for (auto rr : ns_records)
        {
            if (rr->get_type() == dns_rr::T_NS_e)
            {
                if (rr->get_ttl() == 0)
                {
                    LOG(info) << "ignored 0 TTL NS record " << *rr;
                }
                else
                {
                    referral_names.insert(*rr->get_name());
                    m_ns_records.push_back(rr);
                }
            }
            else
            {
                LOG(info) << "ignored none-NS record " << *rr;
            }
        }

        if (referral_names.size() == 0)
        {
            THROW(dns_recursive_cache_answer_exception, "attempt to create referral that didn't contain any valid NS records");
        }
        else if (referral_names.size() > 1)
        {
            THROW(dns_recursive_cache_answer_exception, "attempt to create referral that contained records for more than one name");
        }
        else
        {
            m_referral_name = *(referral_names.begin());
        }
    }

    m_num_records = m_answer_records.size() + m_additional_records.size() + m_ns_records.size();

    for (auto i : m_answer_records)
    {
        m_answer_rr_strings.insert(i->to_string_no_ttl());
    }
}

void dns_recursive_cache_answer::update_min_ttl(bool &min_set, const list<shared_ptr<dns_rr>> &rrs) const
{
    for (auto rr : rrs)
    {
        if (!min_set)
        {
            m_min_ttl = chrono::duration<double>(rr->get_ttl());
            min_set = true;
        }

        uint ttl;

        if (rr->get_type() == dns_rr::T_SOA_e)
        {
            ttl = min(rr->get_ttl(), dynamic_cast<dns_rr_SOA *>(rr.get())->get_minimum());
        }
        else
        {
            ttl = rr->get_ttl();
        }

        if (chrono::duration<double>(ttl) < m_min_ttl)
        {
            m_min_ttl = chrono::duration<double>(rr->get_ttl());
        }
    }
}

dns_recursive_cache_answer::type_t dns_recursive_cache_answer::get_type() const
{
    return m_type;
}

list<shared_ptr<dns_rr>> dns_recursive_cache_answer::get_answer_records() const
{
    unique_lock<mutex> guard(m_lock);
    return m_answer_records;
}

void dns_recursive_cache_answer::add_answer_records(const list<shared_ptr<dns_rr>> &rrs)
{
    unique_lock<mutex> guard(m_lock);

    for (auto nrr : rrs)
    {
        auto s = nrr->to_string_no_ttl();

        if (m_answer_rr_strings.find(s) == m_answer_rr_strings.end())
        {
            m_answer_records.push_back(nrr);
            m_answer_rr_strings.insert(s);
            m_num_records++;
        }
    }
}

list<shared_ptr<dns_rr>> dns_recursive_cache_answer::get_answer_records(dns_rr::type_t t) const
{
    list<shared_ptr<dns_rr>> res;

    unique_lock<mutex> guard(m_lock);
    for (auto rr : m_answer_records)
    {
        if (rr->get_type() == t)
        {
            res.push_back(rr);
        }
    }

    return res;
}

shared_ptr<dns_rr> dns_recursive_cache_answer::get_first_answer_record(dns_rr::type_t t) const
{
    unique_lock<mutex> guard(m_lock);
    for (auto rr : m_answer_records)
    {
        if (rr->get_type() == t)
        {
            return rr;
        }
    }
    THROW(dns_recursive_cache_answer_exception, "no answer record of given type found", t);
}

bool dns_recursive_cache_answer::has_answer_record(dns_rr::type_t t) const
{
    unique_lock<mutex> guard(m_lock);
    for (auto rr : m_answer_records)
    {
        if (rr->get_type() == t)
        {
            return true;
        }
    }
    return false;
}

list<shared_ptr<dns_rr>> dns_recursive_cache_answer::get_nameserver_records() const
{
    unique_lock<mutex> guard(m_lock);
    return m_ns_records;
}

list<shared_ptr<dns_rr>> dns_recursive_cache_answer::get_additional_records() const
{
    unique_lock<mutex> guard(m_lock);
    return m_additional_records;
}

bool dns_recursive_cache_answer::expired() const
{
    unique_lock<mutex> guard(m_lock);

    chrono::time_point<chrono::steady_clock> time_now = chrono::steady_clock::now();

    return (time_now - m_create_time) > m_min_ttl;
}

void dns_recursive_cache_answer::update_ttls() const
{
    unique_lock<mutex> guard(m_lock);

    chrono::time_point<chrono::steady_clock> now = chrono::steady_clock::now();

    chrono::duration<double> diff = now - m_create_time;

    if (diff < chrono::duration<double>(1))
    {
        return;
    }

    if (diff > m_min_ttl)
    {
        diff = m_min_ttl;
    }

    m_min_ttl -= diff;
    m_create_time = now;

    int sdiff = -chrono::duration_cast<chrono::seconds>(diff).count();

    for (auto rr : m_answer_records)
    {
        rr->update_ttl(sdiff);
    }
    for (auto rr : m_ns_records)
    {
        rr->update_ttl(sdiff);
    }
    for (auto rr : m_additional_records)
    {
        rr->update_ttl(sdiff);
    }
}

shared_ptr<dns_rr> dns_recursive_cache_answer::get_soa_record() const
{
    unique_lock<mutex> guard(m_lock);
    for (auto rr : m_ns_records)
    {
        if (rr->get_type() == dns_rr::T_SOA_e)
        {
            return rr;
        }
    }
    return nullptr;
}

uint dns_recursive_cache_answer::get_min_ttl() const
{
    return chrono::duration_cast<chrono::seconds>(m_min_ttl).count();
}

list<shared_ptr<dns_rr>> dns_recursive_cache_answer::get_nameserver_NS_records() const
{
    list<shared_ptr<dns_rr>> res;

    for (auto rr : m_ns_records)
    {
        if (rr->get_type() == dns_rr::T_NS_e)
        {
            res.push_back(rr);
        }
    }

    return res;
}

std::string dns_recursive_cache_answer::get_type_as_string() const
{
    return get_type_as_string(m_type);
}

std::string dns_recursive_cache_answer::get_type_as_string(type_t t)
{
    switch (t)
    {
    case data_e:
        return "DATA";
    case no_data_e:
        return "NO DATA";
    case nxdomain_e:
        return "NXDOMAIN";
    case referral_e:
        return "REFERRAL";
    case fail_e:
        return "FAIL";
    }
    return "<INVALID>";
}

void dns_recursive_cache_answer::set_ttls(uint ttl) const
{
    unique_lock<mutex> guard(m_lock);

    for (auto rr : m_answer_records)
    {
        rr->set_ttl(ttl);
    }
    for (auto rr : m_ns_records)
    {
        rr->set_ttl(ttl);
    }
    for (auto rr : m_additional_records)
    {
        rr->set_ttl(ttl);
    }

    m_min_ttl = chrono::duration<double>(ttl);
}

const dns_name &dns_recursive_cache_answer::get_referral_name() const
{
    return m_referral_name;
}

size_t dns_recursive_cache_answer::get_sequence() const
{
    return m_sequence;
}

uint dns_recursive_cache_answer::get_num_records() const
{
    return m_num_records;
}

void dns_recursive_cache_answer::dump() const
{
    LOG(warning) << "recursive cache answer contents";

    LOG(warning) << "TYPE IS " << get_type_as_string();
    LOG(warning) << "ANSWER RECORDS";
    for (auto rr : m_answer_records)
    {
        LOG(warning) << *rr;
    }
    LOG(warning) << "NS RECORDS";
    for (auto rr : m_ns_records)
    {
        LOG(warning) << *rr;
    }
    LOG(warning) << "OTHER RECORDS";
    for (auto rr : m_additional_records)
    {
        LOG(warning) << *rr;
    }
}

