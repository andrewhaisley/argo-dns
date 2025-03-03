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
#include <chrono>
#include <algorithm>

#include "config.hpp"
#include "monitor.hpp"
#include "dns_recursive_cache.hpp"
#include "row_root_server.hpp"
#include "connection_pool.hpp"
#include "dns_rr_NS.hpp"
#include "dns_rr_A.hpp"
#include "dns_rr_AAAA.hpp"
#include "run_state.hpp"
#include "dns_parallel_client.hpp"

using namespace std;
using namespace chrono;
using namespace adns;
using namespace db;

static shared_ptr<dns_recursive_cache> instance;

#define ONE_HOUR (60 * 60)
#define ONE_DAY  (60 * 60 * 24)
#define TWO_DAYS (60 * 60 * 48)

void dns_recursive_cache::init(const cache_config &c)
{
    static mutex lock;

    lock_guard<mutex> guard(lock);

    if (!instance)
    {
        instance.reset(new dns_recursive_cache(c));
    }
}

shared_ptr<dns_recursive_cache> dns_recursive_cache::get_instance()
{
    return instance;
}

dns_recursive_cache::dns_recursive_cache(const cache_config &c) : m_config(c), m_num_answer_rrs(0), m_num_referral_rrs(0)
{
    m_monitor_answers_id = monitor::add_thing("recursive cache", "singleton", "number of cached answer records", 0);
    m_monitor_referrals_id = monitor::add_thing("recursive cache", "singleton", "number of cached referral records", 0);
    m_monitor_last_garbage_collect_id = monitor::add_thing("recursive cache", "singleton", "time of last garbage collection", "<none yet>");

    monitor::add_thing("recursive cache", "singleton", "garbage collection interval (miliseconds)", c.cache_garbage_collect_ms);
    monitor::add_thing("recursive cache", "singleton", "max number of cached answer records", c.cache_max_answer_rrs);
    monitor::add_thing("recursive cache", "singleton", "max number of cached referral records", c.cache_max_referral_rrs);

    load_root_hints();
    m_garbage_collector_thread.reset(new thread(&dns_recursive_cache::garbage_collect, this));
    m_root_hint_updater_thread.reset(new thread(&dns_recursive_cache::update_root_hints, this));
}

void dns_recursive_cache::load_root_hints()
{
    {
        read_write_lock::write_guard guard1(m_answer_lock);
        m_answers.clear();
        m_num_answer_rrs = 0;

        read_write_lock::write_guard guard2(m_referral_lock);
        m_num_referral_rrs = 0;
        m_referrals.clear();
    }

    auto conn = connection_pool::get_connection();

    map<string, shared_ptr<dns_rr>> root_ns_records;

    list<shared_ptr<dns_rr>> root_glue;

    auto root = dns_name::root.clone();

    for (auto rh : row_root_server::get_rows(*conn))
    {
        auto nsr = make_shared<dns_rr_NS>();

        nsr->set_name(root);
        nsr->set_nsdname(make_shared<dns_name>(rh->get_name()));
        nsr->set_ttl(TWO_DAYS);
        nsr->set_class(dns_rr::C_IN_e);

        root_ns_records[rh->get_name()] = nsr;

        ip_address ip(rh->get_ip_address());

        if (ip.get_type() == ip_address::ip_v4_e)
        {
            auto ar = make_shared<dns_rr_A>();

            ar->set_ip4_addr(ip);
            ar->set_name(make_shared<dns_name>(rh->get_name()));
            ar->set_ttl(TWO_DAYS);
            ar->set_class(dns_rr::C_IN_e);

            root_glue.push_back(ar);
        }
        else
        {
            auto aaaar = make_shared<dns_rr_AAAA>();

            aaaar->set_ip6_addr(ip);
            aaaar->set_name(make_shared<dns_name>(rh->get_name()));
            aaaar->set_ttl(TWO_DAYS);
            aaaar->set_class(dns_rr::C_IN_e);
    
            root_glue.push_back(aaaar);
        }
    }

    list<shared_ptr<dns_rr>> nsr;

    for (auto nsp : root_ns_records)
    {
        nsr.push_back(nsp.second);
    }

    add_referral(dns_name::root, make_shared<dns_recursive_cache_answer>(
                                dns_recursive_cache_answer::referral_e, 
                                TWO_DAYS,
                                list<shared_ptr<dns_rr>>(),
                                nsr,
                                root_glue));
}

dns_recursive_cache::~dns_recursive_cache()
{
}

void dns_recursive_cache::add_answer(const shared_ptr<dns_question> &question, const shared_ptr<const dns_recursive_cache_answer> &answer)
{
    // don't cache something that has zero TTLs in it
    if (answer->get_min_ttl() == 0)
    {
        return;
    }

    // don't cache an answer that contains TKEY records
    if (answer->has_answer_record(dns_rr::T_TKEY_e))
    {
        return;
    }

    switch (answer->get_type())
    {
    case dns_recursive_cache_answer::data_e:
    case dns_recursive_cache_answer::no_data_e:
    case dns_recursive_cache_answer::nxdomain_e:
    case dns_recursive_cache_answer::fail_e:
        {
            read_write_lock::write_guard guard(m_answer_lock);

            m_answers[*question] = answer;
            m_answer_list.push_back(pair<dns_question, shared_ptr<const dns_recursive_cache_answer>>(*question, answer));
            m_num_answer_rrs += answer->get_num_records();
            monitor::set_value(m_monitor_answers_id, m_num_answer_rrs);
        }
        break;
    default:
        THROW(dns_recursive_cache_exception, "attempt to cache an answer of uncacheable type", answer->get_type());
    }
}

void dns_recursive_cache::add_referral(const dns_name &qname, const shared_ptr<const dns_recursive_cache_answer> &answer)
{
    read_write_lock::write_guard guard(m_referral_lock);

    if (answer->get_referral_name().is_root())
    {
        m_referrals[dns_name::root] = answer;
    }
    else
    {
        m_num_referral_rrs += answer->get_num_records();
        m_referral_list.push_back(pair<dns_name, shared_ptr<const dns_recursive_cache_answer>>(answer->get_referral_name(), answer));
        m_referrals[answer->get_referral_name()] = answer;
        monitor::set_value(m_monitor_referrals_id, m_num_referral_rrs);
    }
}

void dns_recursive_cache::garbage_collect()
{
    duration<int, milli> timeout(m_config.cache_garbage_collect_ms);

    time_point<steady_clock> start_time = steady_clock::now();

    while (true)
    {
        if (run_state::o_state == run_state::shutdown_e)
        {
            return;
        }

        time_point<steady_clock> time_now = steady_clock::now();

        if ((time_now - start_time) > timeout)
        {
            // garbage colllect
            garbage_collect_answers();
            garbage_collect_referrals();

            // reset the clock
            start_time = steady_clock::now();

            // update the monitor showing time of last GC - use the standard system clock for this
            monitor::set_value(m_monitor_last_garbage_collect_id, datetime::now().to_string());
        }
        else
        {
            sleep(1);
        }
    }
}

void dns_recursive_cache::join()
{
    if (m_garbage_collector_thread)
    {
        m_garbage_collector_thread->join();
        m_root_hint_updater_thread->join();
        m_garbage_collector_thread = nullptr;
        m_root_hint_updater_thread = nullptr;
    }
}

bool dns_recursive_cache::answer_expired(const dns_question &q)
{
    read_write_lock::read_guard guard(m_answer_lock);

    auto i = m_answers.find(q);
    if (i == m_answers.end())
    {
        return false;
    }
    else
    {
        return i->second->expired();
    }
}

shared_ptr<const dns_recursive_cache_answer> dns_recursive_cache::get_answer(const shared_ptr<dns_question> &question, bool referral_requested) 
{
    {
        read_write_lock::read_guard guard(m_answer_lock);

        auto i = m_answers.find(*question);

        if (i != m_answers.end())
        {
            auto a = i->second;

            if (!a->expired())
            {
                a->update_ttls();
                return a;
            }
        }
    }

    if (referral_requested)
    {
        return get_referral(question);
    }
    else
    {
        return make_shared<dns_recursive_cache_answer>(dns_recursive_cache_answer::fail_e, m_config.default_ttl);
    }
}

shared_ptr<const dns_recursive_cache_answer> dns_recursive_cache::get_referral(const shared_ptr<dns_question> &question) const
{
    read_write_lock::read_guard guard(m_referral_lock);

    auto name = question->get_qname().clone();

    while (true)
    {
        auto i = m_referrals.find(*name);
        if (i != m_referrals.end())
        {
            auto a = i->second;

            if (!a->expired())
            {
                return a;
            }
        }

        if (name->size() == 0)
        {
            break;
        }
        else
        {
            name->remove_first();
        }
    }
    
    THROW(dns_recursive_cache_exception, "no root hints loaded");
}

void dns_recursive_cache::dump() const
{
    LOG(debug) << "dumping recursive cache - answers";
    for (auto i : m_answers)
    {
        LOG(debug) << "name is '" << i.first << "'";
        i.second->dump();
    }

    LOG(debug) << "dumping recursive cache - referrals";
    for (auto i : m_referrals)
    {
        LOG(debug) << "name is '" << i.first << "'";
        i.second->dump();
    }
}

void dns_recursive_cache::update_root_hints_now(const set<ip_address> &ip_addresses)
{
    auto q = make_shared<dns_question>(dns_name("."), dns_question::QT_RR_e, dns_rr::T_NS_e);

    dns_message qm;
    qm.set_type(dns_message::query_e);
    qm.set_op_code(dns_message::op_query_e);
    qm.set_is_recursion_desired(false);
    qm.set_question(q);

    LOG(info) << "querying root servers";
    dns_parallel_client c(m_config.client);
    c.set_ip_addresses(ip_addresses);
    auto res = c.query(qm);

    if (res)
    {
        if (validate_root_response(res))
        {
            LOG(info) << "updating root hints in recursive cache";
            auto ans = make_shared<dns_recursive_cache_answer>(
                            dns_recursive_cache_answer::referral_e, 
                            TWO_DAYS,
                            list<shared_ptr<dns_rr>>(),
                            res->get_answers(), 
                            res->get_additional_records());
            // set the TTLs to 48 hours regardless of what we got back from the root
            // servers
            ans->set_ttls(TWO_DAYS);
            add_referral(dns_name::root, ans);
        }

        delete res;
    }
    else
    {
        LOG(warning) << "no responses from any root servers, hints not updated";
        // reset the TTLs to 48 hours, root servers don't change very often
        dns_recursive_cache::get_instance()->get_referral(q)->set_ttls(TWO_DAYS);
    }
}

bool dns_recursive_cache::validate_root_response(const dns_message *res)
{
    // are there any non-NS answers (that's a fail as we only asked for NS records)
    // are all of the NS records for root
    for (auto r : res->get_answers())
    {
        if (r->get_type() != dns_rr::T_NS_e)
        {
            LOG(warning) << "root response included non-NS record answer";
            return false;
        }
        if (!r->get_name()->is_root())
        {
            LOG(warning) << "root response included non-root NS record answer";
            return false;
        }
    }

    // are there are least 5 nameservers listed?
    if (res->get_answers().size() < 5)
    {
        LOG(warning) << "root response had fewer than 5 NS records";
        return false;
    }

    // do all of the NS records have glue?
    for (auto r : res->get_answers())
    {
        dns_rr_NS *nsr = dynamic_cast<dns_rr_NS *>(r.get());

        bool found_a = false;
        bool found_aaaa = false;

        for (auto g : res->get_additional_records())
        {
            if (g->get_type() == dns_rr::T_A_e)
            {
                if (*(g->get_name()) == *(nsr->get_nsdname()))
                {
                    found_a = true;
                }
            }
            else if (g->get_type() == dns_rr::T_AAAA_e)
            {
                if (*(g->get_name()) == *(nsr->get_nsdname()))
                {
                    found_aaaa = true;
                }
            }
        }

        if (!found_a || !found_aaaa)
        {
            LOG(warning) << "root response missing glue for " << *(nsr->get_nsdname());
            return false;
        }
    }
    
    return true;
}

void dns_recursive_cache::update_root_hints()
{
    auto q = make_shared<dns_question>(dns_name("."), dns_question::QT_RR_e, dns_rr::T_NS_e);

    bool first_pass = true;

    while (true)
    {
        sleep(2);

        if (run_state::o_state == run_state::shutdown_e)
        {
            return;
        }

        auto ca = dns_recursive_cache::get_instance()->get_referral(q);
        ca->update_ttls();

        if (first_pass || (ca->get_min_ttl() < ONE_DAY))
        {
            first_pass = false;
            set<ip_address> ip_addresses;
            get_glue_ips(ip_addresses, ca);
            update_root_hints_now(ip_addresses);

            // look at what we have in the cache now
            ca = dns_recursive_cache::get_instance()->get_referral(q);

            if (ca->get_min_ttl() < ONE_HOUR)
            {
                // something has gone badly wrong, reset the root server TTLs to 2 days
                LOG(warning) << "continually failed to update root hints, resetting TTLs to 48 hours";
                ca->set_ttls(TWO_DAYS);
            }
        }
    }
}

void dns_recursive_cache::get_glue_ips(
                        set<ip_address>                                    &ip_addresses,
                        const shared_ptr<const dns_recursive_cache_answer> &ca)
{
    ip_addresses.clear();

    for (auto ns : ca->get_nameserver_records())
    {
        dns_rr_NS *nsr = dynamic_cast<dns_rr_NS *>(ns.get());

        for (auto a : ca->get_additional_records())
        {
            if (config::use_ip4_root_hints() && ((*nsr->get_nsdname() == *a->get_name()) && (a->get_type() == dns_rr::T_A_e)))
            {
                dns_rr_A *ar = dynamic_cast<dns_rr_A *>(a.get());
                ip_addresses.insert(ar->get_ip4_addr());
            }

            if (config::use_ip6_root_hints() && ((*nsr->get_nsdname() == *a->get_name()) && (a->get_type() == dns_rr::T_AAAA_e)))
            {
                dns_rr_AAAA *ar = dynamic_cast<dns_rr_AAAA *>(a.get());
                ip_addresses.insert(ar->get_ip6_addr());
            }
        }
    }
}

void dns_recursive_cache::garbage_collect_answers()
{
    read_write_lock::write_guard guard2(m_answer_lock);

    auto i = m_answer_list.begin();

    // get rid of anything expired or anything in the list that is no longer in the cache
    // due to having being overwitten
    while (i != m_answer_list.end())
    {
        if (m_answers.find(i->first) == m_answers.end())
        {
            // no longer in the cache
            m_num_answer_rrs -= i->second->get_num_records();
            m_answer_list.erase(i++);
        }
        else
        {
            if (i->second->get_sequence() != m_answers[i->first]->get_sequence())
            {
                // in the cache but replaced with a new version
                m_num_answer_rrs -= i->second->get_num_records();
                m_answer_list.erase(i++);
            }
            else
            {
                if (i->second->expired())
                {
                    // in the cache and expires
                    m_num_answer_rrs -= i->second->get_num_records();
                    m_answers.erase(i->first);
                    m_answer_list.erase(i++);
                }
                else
                {
                    // in the cache, not expired, skip to next item
                    i++;
                }
            }
        }
    }

    i = m_answer_list.begin();

    // now get rid of the entries that have been in the cache the longest until we're down
    // to the required size
    while (i != m_answer_list.end() && (m_num_answer_rrs > m_config.cache_max_answer_rrs))
    {
        m_num_answer_rrs -= i->second->get_num_records();
        m_answers.erase(i->first);
        m_answer_list.erase(i++);
    }

    monitor::set_value(m_monitor_answers_id, m_num_answer_rrs);
}

void dns_recursive_cache::garbage_collect_referrals()
{
    read_write_lock::write_guard guard2(m_referral_lock);

    auto i = m_referral_list.begin();

    // get rid of anything expired or anything in the list that is no longer in the cache
    // due to having being overwitten
    while (i != m_referral_list.end())
    {
        if (m_referrals.find(i->first) == m_referrals.end())
        {
            // no longer in the cache at all
            //LOG(warning) << "found referral that's no in the cache";
            m_num_referral_rrs -= i->second->get_num_records();
            m_referral_list.erase(i++);
        }
        else
        {
            if (i->second->get_sequence() != m_referrals[i->first]->get_sequence())
            {
                // in the cache but replaced with a newer version
                //LOG(warning) << "found referral that's been replaced by newer version for " << i->first;
                //LOG(warning) << "list record is";
                //i->second->dump();
                //LOG(warning) << "cache record is";
                //m_referrals[i->first]->dump();
                m_num_referral_rrs -= i->second->get_num_records();
                m_referral_list.erase(i++);
            }
            else
            {
                if (i->second->expired())
                {
                    // in the cache but expired
                    //LOG(warning) << "found referral that's expired";
                    m_num_referral_rrs -= i->second->get_num_records();
                    m_referrals.erase(i->first);
                    m_referral_list.erase(i++);
                }
                else
                {
                    // still in the cache, not overwitten, not expired, so skip to the next
                    i++;
                }
            }
        }
    }

    i = m_referral_list.begin();

    // now get rid of the entries that have been in the cache the longest until we're down
    // to the required size

    while (i != m_referral_list.end() && (m_num_referral_rrs > m_config.cache_max_referral_rrs))
    {
        m_num_referral_rrs -= i->second->get_num_records();
        m_referrals.erase(i->first);
        m_referral_list.erase(i++);
    }

    monitor::set_value(m_monitor_referrals_id, m_num_referral_rrs);
}

uint dns_recursive_cache::get_default_ttl() const
{
    return m_config.default_ttl;
}

map<dns_name, uint> dns_recursive_cache::get_referral_name_record_counts()
{
    map<dns_name, uint> res;

    read_write_lock::read_guard guard2(m_referral_lock);

    for (auto i : m_referrals)
    {
        res[i.first] = i.second->get_num_records();
    }

    return res;
}
