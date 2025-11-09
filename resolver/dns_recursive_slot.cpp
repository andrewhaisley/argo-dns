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
 
#include "dns_rr_NS.hpp"
#include "dns_rr_A.hpp"
#include "dns_rr_AAAA.hpp"
#include "dns_rr_CNAME.hpp"
#include "dns_rr_DNAME.hpp"
#include "dns_recursive_slot.hpp"
#include "dns_recursive_slot_manager.hpp"
#include "dns_recursive_resolver.hpp"

using namespace std;
using namespace chrono;
using namespace adns;
using namespace boost::log::trivial;

dns_recursive_slot::params_t::params_t(const server_config &config, dns_recursive_slot_manager &rsm) : 
                    m_config(config),
                    m_slot_manager(rsm)
{
}

dns_recursive_slot::dns_recursive_slot(const params_t &params, message_queue<dns_message_envelope *> &q) : 
                    m_params(params), 
                    m_out_queue(q),
                    m_client(params.m_config.dns.client),
                    m_cache(dns_recursive_cache::get_instance())
{
}

dns_recursive_slot::query_t::query_t(const dns_question &q, const ip_address &ip)
{
    m_question = q;
    m_ip = ip;
}

bool dns_recursive_slot::query_t::operator<(const query_t &other) const
{
    if (m_ip < other.m_ip)
    {
        return true;
    }
    else if (other.m_ip < m_ip)
    {
        return false;
    }
    else
    {
        return m_question < other.m_question;
    }
}

dns_recursive_slot::~dns_recursive_slot()
{
}

void dns_recursive_slot::join()
{
}

void dns_recursive_slot::send_response(dns_message_envelope *m)
{
    auto w = m_params.m_slot_manager.clear_waiters(*(m->get_request()->get_question()));

    // m is one of these, don't delete it until the end as we need the response it holds to still
    // be valid through this loop
    for (auto q : w)
    {
        if (q != m)
        {
            q->set_response(new dns_message(*m->get_response()));
            q->get_response()->set_id(q->get_request()->get_id());
            if (!m_out_queue.enqueue(q))
            {
                delete q;
            }
        }
    }

    m->get_response()->set_id(m->get_request()->get_id());
    if (!m_out_queue.enqueue(m))
    {
        delete m;
    }
}

bool dns_recursive_slot::has_glue_records(const shared_ptr<dns_rr> &ns, const shared_ptr<const dns_recursive_cache_answer> &cs)
{
    list<shared_ptr<dns_rr>> res;

    dns_rr_NS *nsr = dynamic_cast<dns_rr_NS *>(ns.get());

    for (auto a : cs->get_additional_records())
    {
        if (m_params.m_config.dns.client.use_ip4 && ((*nsr->get_nsdname() == *a->get_name()) && (a->get_type() == dns_rr::T_A_e)))
        {
            return true;
        }

        if (m_params.m_config.dns.client.use_ip6 && ((*nsr->get_nsdname() == *a->get_name()) && (a->get_type() == dns_rr::T_AAAA_e)))
        {
            return true;
        }
    }

    return false;
}

void dns_recursive_slot::get_ordered_nameserver_list(
                    list<shared_ptr<dns_rr>>                           &ordered_ns_with_glue, 
                    list<shared_ptr<dns_rr>>                           &ordered_ns_without_glue, 
                    const shared_ptr<const dns_recursive_cache_answer> &cs)
{
    ordered_ns_with_glue.clear();
    ordered_ns_without_glue.clear();

    for (auto ns : cs->get_nameserver_records())
    {
        if (has_glue_records(ns, cs))
        {
            ordered_ns_with_glue.push_back(ns);
        }
        else
        {
            ordered_ns_without_glue.push_back(ns);
        }
    }
}

void dns_recursive_slot::get_glue_ips(
                        set<ip_address>                                    &ip_addresses,
                        shared_ptr<dns_rr>                                 &ns, 
                        const shared_ptr<const dns_recursive_cache_answer> &cs)
{
    ip_addresses.clear();

    dns_rr_NS *nsr = dynamic_cast<dns_rr_NS *>(ns.get());

    for (auto a : cs->get_additional_records())
    {
        if (m_params.m_config.dns.client.use_ip4 && ((*nsr->get_nsdname() == *a->get_name()) && (a->get_type() == dns_rr::T_A_e)))
        {
            dns_rr_A *ar = dynamic_cast<dns_rr_A *>(a.get());
            ip_addresses.insert(ar->get_ip4_addr());
        }

        if (m_params.m_config.dns.client.use_ip6 && ((*nsr->get_nsdname() == *a->get_name()) && (a->get_type() == dns_rr::T_AAAA_e)))
        {
            dns_rr_AAAA *ar = dynamic_cast<dns_rr_AAAA *>(a.get());
            ip_addresses.insert(ar->get_ip6_addr());
        }
    }
}

void dns_recursive_slot::resolve_nameserver_ips(set<ip_address> &ip_addresses, const shared_ptr<dns_name> &n, uint depth)
{
    ip_addresses.clear();

    if (m_params.m_config.dns.client.use_ip4)
    {
        auto q = make_shared<dns_question>(*n, dns_question::QT_RR_e, dns_rr::T_A_e);

        dns_message *m = new dns_message();
        m->set_type(dns_message::query_e);
        m->set_op_code(dns_message::op_query_e);
        m->set_is_recursion_desired(false);
        m->set_question(q);

        unique_ptr<dns_message_envelope> rq(new dns_message_envelope());

        rq->set_request(m);

        auto ca = resolve(rq.get(), depth);

        if (ca->get_type() == dns_recursive_cache_answer::data_e)
        {
            for (auto a : ca->get_answer_records())
            {
                if ((a->get_type() == dns_rr::T_A_e) && (*n == *a->get_name()))
                {
                    dns_rr_A *ar = dynamic_cast<dns_rr_A *>(a.get());
                    ip_addresses.insert(ar->get_ip4_addr());
                }
            }
        }
    }

    if (m_params.m_config.dns.client.use_ip6)
    {
        auto q = make_shared<dns_question>(*n, dns_question::QT_RR_e, dns_rr::T_AAAA_e);

        dns_message *m = new dns_message();
        m->set_type(dns_message::query_e);
        m->set_op_code(dns_message::op_query_e);
        m->set_is_recursion_desired(false);
        m->set_question(q);

        unique_ptr<dns_message_envelope> rq(new dns_message_envelope());

        rq->set_request(m);

        auto ca = resolve(rq.get(), depth);

        if (ca->get_type() == dns_recursive_cache_answer::data_e)
        {
            for (auto a : ca->get_answer_records())
            {
                if ((a->get_type() == dns_rr::T_AAAA_e) && (*n == *a->get_name()))
                {
                    dns_rr_AAAA *ar = dynamic_cast<dns_rr_AAAA *>(a.get());
                    ip_addresses.insert(ar->get_ip6_addr());
                }
            }
        }
    }
}

shared_ptr<const dns_recursive_cache_answer> dns_recursive_slot::query_nameserver_cname(
                                                    dns_message_envelope                               *m,
                                                    const shared_ptr<const dns_recursive_cache_answer> &ans,
                                                    uint                                               depth)
{
    auto r = ans->get_first_answer_record(dns_rr::T_CNAME_e);
    dns_rr_CNAME *cnr = dynamic_cast<dns_rr_CNAME *>(r.get());

    // resolve the thing pointed at by the CNAME
    auto q = make_shared<dns_question>(
                    *cnr->get_cname(), 
                    m->get_request()->get_question()->get_qtype(), 
                    m->get_request()->get_question()->get_rr_type());

    dns_message *rm = new dns_message();
    rm->set_type(dns_message::query_e);
    rm->set_op_code(dns_message::op_query_e);
    rm->set_is_recursion_desired(false);
    rm->set_question(q);

    unique_ptr<dns_message_envelope> rq(new dns_message_envelope());

    rq->set_request(rm);

    auto ca = make_shared<dns_recursive_cache_answer>(*resolve(rq.get(), depth));

    switch (ca->get_type())
    {
    case dns_recursive_cache_answer::data_e:
    case dns_recursive_cache_answer::no_data_e:
    case dns_recursive_cache_answer::nxdomain_e:
    case dns_recursive_cache_answer::fail_e:
        ca->add_answer_records(ans->get_answer_records());
        m_cache->add_answer(m->get_request()->get_question(), ca);
        break;
    default:
        THROW(recursive_resolver_fail_exception, "CNAME resolution failed");
    }

    return ca;
}

shared_ptr<const dns_recursive_cache_answer> dns_recursive_slot::query_nameserver_dname(
                                                    dns_message_envelope                               *m,
                                                    const shared_ptr<const dns_recursive_cache_answer> &ans,
                                                    uint                                               depth)
{
    auto r = ans->get_first_answer_record(dns_rr::T_DNAME_e);
    dns_rr_DNAME *dnr = dynamic_cast<dns_rr_DNAME *>(r.get());

    // calculate the name of thing pointed at by the DNAME

    // QNAME:  a.example.com.      this is m->get_request()->get_qname()
    // OWNER:  example.com.        this is dns->get_name()
    // DNAME:  y.example.net.      this dnr->get_dname()
    // RESULT: a.y.example.net.    this is the query we need to construct

    dns_name qname = m->get_request()->get_question()->get_qname();
    auto owner = dnr->get_name();

    if (qname.is_subdomain_of(*owner))
    {
        qname.remove_last_n_labels(owner->size());
        qname.append(*(dnr->get_dname()));
    }
    else
    {
        // dnames must come from the domain being queried
        return nullptr;
    }

    // now resolve it
    auto q = make_shared<dns_question>(
                    qname, 
                    m->get_request()->get_question()->get_qtype(), 
                    m->get_request()->get_question()->get_rr_type());

    dns_message *rm = new dns_message();
    rm->set_type(dns_message::query_e);
    rm->set_op_code(dns_message::op_query_e);
    rm->set_is_recursion_desired(false);
    rm->set_question(q);

    unique_ptr<dns_message_envelope> rq(new dns_message_envelope());

    rq->set_request(rm);

    auto ca = make_shared<dns_recursive_cache_answer>(*resolve(rq.get(), depth));

    switch (ca->get_type())
    {
    case dns_recursive_cache_answer::data_e:
    case dns_recursive_cache_answer::no_data_e:
    case dns_recursive_cache_answer::nxdomain_e:
    case dns_recursive_cache_answer::fail_e:
        ca->add_answer_records(ans->get_answer_records());
        m_cache->add_answer(m->get_request()->get_question(), ca);
        break;
    default:
        THROW(recursive_resolver_fail_exception, "DNAME resolution failed");
    }

    return ca;
}

shared_ptr<const dns_recursive_cache_answer> dns_recursive_slot::query_nameserver_ips(
                                                    const shared_ptr<const dns_recursive_cache_answer> &referred_by,
                                                    dns_message_envelope  *m,
                                                    const set<ip_address> &ip_addresses,
                                                    uint                  depth)
{
    if (ip_addresses.size() == 0)
    {
        return shared_ptr<dns_recursive_cache_answer>(nullptr);
    }

    // query to be sent to the nameserver
    dns_message qm;
    qm.set_type(dns_message::query_e);
    qm.set_op_code(dns_message::op_query_e);
    qm.set_is_recursion_desired(false);
    qm.set_question(m->get_request()->get_question());

    duration<double> elapsed = duration_cast<duration<double>>(steady_clock::now() - m_start_time);

    check_timeout(m);

    m_client.set_ip_addresses(ip_addresses);

    unique_ptr<dns_message> res(m_client.query(qm));
    check_timeout(m);

    if (res)
    {
        auto at = classify_response(referred_by, m->get_request()->get_question(), res.get());

        auto ans = make_shared<dns_recursive_cache_answer>(
                            map_answer_type(at),
                            m_cache->get_default_ttl(),
                            res->get_answers(),
                            res->get_nameservers(),
                            res->get_additional_records());

        switch (at)
        {
        case additional_data_e:
            // merge the additional records into the answer records
            ans->add_answer_records(ans->get_additional_records());
            return ans;
            
        case data_e:
        case no_data_e:
        case nxdomain_e:
        case fail_e:
            m_cache->add_answer(m->get_request()->get_question(), ans);
            return ans;

        case referral_e:
            m_cache->add_referral(m->get_request()->get_question()->get_qname(), ans);
            return ans;

        case cname_e :
            return query_nameserver_cname(m, ans, depth);

        case dname_e :
            return query_nameserver_dname(m, ans, depth);

        default:
            THROW(recursive_resolver_fail_exception, "unknown answer type");
        }
    }
    else
    {
        return shared_ptr<dns_recursive_cache_answer>(nullptr);
    }
}

shared_ptr<const dns_recursive_cache_answer> dns_recursive_slot::query_nameserver(
                                                    dns_message_envelope                               *m,
                                                    const shared_ptr<const dns_recursive_cache_answer> &cs,
                                                    uint                                               depth)
{
    list<shared_ptr<dns_rr>> lns_with_glue;
    list<shared_ptr<dns_rr>> lns_without_glue;

    get_ordered_nameserver_list(lns_with_glue, lns_without_glue, cs);

    // start by getting all glue IPs for all nameservers in the glue list
    set<ip_address> all_ip_addresses;

    for (auto ns : lns_with_glue)
    {
        set<ip_address> ips;
        get_glue_ips(ips, ns, cs);
        all_ip_addresses.insert(ips.begin(), ips.end());
    }

    check_loop(*m->get_request()->get_question(), all_ip_addresses);

    if (all_ip_addresses.size() > 0)
    {
        // we have glue so try to talk in parallel to the available IPs
        try
        {
            auto res = query_nameserver_ips(cs, m, all_ip_addresses, depth);
            if (res)
            {
                return res;
            }
        }
        catch (exception &e)
        {
            e.log(warning, "query_nameserver_ips threw exception");
        }
        catch (...)
        {
        }
    }

    // Either there was no glue or none of the namservers responded, 
    // so now go nameserver by nameserver depth first but only for
    // those that had no glue (don't want to retry a dead one).
    for (auto ns : lns_without_glue)
    {
        dns_rr_NS *nsr = dynamic_cast<dns_rr_NS *>(ns.get());
        
        resolve_nameserver_ips(all_ip_addresses, nsr->get_nsdname(), depth);

        check_loop(*m->get_request()->get_question(), all_ip_addresses);

        if (all_ip_addresses.size() > 0)
        {
            auto res = query_nameserver_ips(cs, m, all_ip_addresses, depth);
            if (res)
            {
                return res;
            }
        }
    }

    // can't find any nameserver that will respond
    return make_shared<dns_recursive_cache_answer>(dns_recursive_cache_answer::fail_e, m_cache->get_default_ttl());
}

shared_ptr<const dns_recursive_cache_answer> dns_recursive_slot::resolve(dns_message_envelope *m, uint depth)
{
    if (++depth > m_params.m_config.dns.max_recursion_depth)
    {
        THROW(recursive_resolver_fail_exception, "maximum recursion depth exceeded");
    }

    auto ca = m_cache->get_answer(m->get_request()->get_question(), true);

    while (true)
    {
        switch (ca->get_type())
        {
        case dns_recursive_cache_answer::data_e:
        case dns_recursive_cache_answer::no_data_e:
        case dns_recursive_cache_answer::fail_e:
        case dns_recursive_cache_answer::nxdomain_e:
            return ca;

        case dns_recursive_cache_answer::referral_e:
            ca = query_nameserver(m, ca, depth);
            break;

        default:
            THROW(recursive_resolver_fail_exception, "got invalid cached answer type");
        }
    }
}

void dns_recursive_slot::check_timeout(const dns_message_envelope *m)
{
    duration<uint, milli> elapsed = duration_cast<duration<uint, milli>>(steady_clock::now() - m_start_time);

    if (elapsed.count() > m_params.m_config.dns.recursive_timeout_ms)
    {
        THROW(recursive_resolver_fail_exception, "timeout, query took more than the configured maximum time");
    }
}

void dns_recursive_slot::check_loop(const dns_question &q, set<ip_address> &ip_addresses)
{
    set<ip_address> repeat_addresses;

    if (ip_addresses.size() == 0)
    {
        return;
    }

    for (auto i : ip_addresses)
    {
        query_t query(q, i);

        if (m_query_trail.find(query) == m_query_trail.end())
        {
            m_query_trail.insert(query);
        }
        else
        {
            repeat_addresses.insert(i);
        }
    }

    for (auto i : repeat_addresses)
    {
        ip_addresses.erase(i);
    }
}

void dns_recursive_slot::process(dns_message_envelope *m)
{
    m_start_time = steady_clock::now();
    m_query_trail.clear();
    m_top_level_query = m;
    m_client.reset_sockets();

    try
    {
        auto qt = m->get_request()->get_question()->get_qtype();
        if (qt == dns_question::QT_IXFR_e || qt == dns_question::QT_AXFR_e)
        { 
            THROW(recursive_resolver_fail_exception, "AXFR and IXFR are not supported");
        } 
        else
        {
            auto a = resolve(m);

            switch (a->get_type())
            {
            case dns_recursive_cache_answer::data_e:
                respond_with_answer(m, a);
                break;

            case dns_recursive_cache_answer::no_data_e:
                respond_with_no_data(m, a);
                break;

            case dns_recursive_cache_answer::fail_e:
                respond_with_error(m, dns_message::server_failure_e);
                break;

            case dns_recursive_cache_answer::nxdomain_e:
                respond_with_nxdomain(m, a);
                break;

            // the resolution process should mean we never get these here
            default :
                THROW(recursive_resolver_fail_exception, "got unexpected answer type from resolve()");
                break;
            }
        }
        m_client.close_sockets();
    }
    catch (adns::exception &e)
    {
        m_client.close_sockets();
        respond_with_error(m, dns_message::server_failure_e);
    }
}

dns_message *dns_recursive_slot::construct_base_response(dns_message_envelope *e)
{
    dns_message *r = new dns_message;

    r->set_id(e->get_request()->get_id());
    r->set_type(dns_message::response_e);
    r->set_op_code(e->get_request()->get_op_code());
    r->set_response_code(dns_message::no_error_e);
    r->set_is_recursion_desired(e->get_request()->get_is_recursion_desired());
    r->set_is_recursion_available(true);
    r->set_question(e->get_request()->get_question());

    return r;
}

void dns_recursive_slot::respond_with_answer(
                    dns_message_envelope                              *e,
                    const shared_ptr<const dns_recursive_cache_answer> &answer)
{
    e->set_response(construct_base_response(e));
    e->get_response()->add_answers(answer->get_answer_records());
    send_response(e);
}

void dns_recursive_slot::respond_with_error(dns_message_envelope *e, dns_message::response_code_t rc)
{
    e->set_response(construct_base_response(e));
    e->get_response()->set_response_code(rc);
    send_response(e);
}

void dns_recursive_slot::respond_with_no_data(
                    dns_message_envelope                               *e, 
                    const shared_ptr<const dns_recursive_cache_answer> &answer)
{
    e->set_response(construct_base_response(e));

    auto soa = answer->get_soa_record();
    if (soa)
    {
        e->get_response()->add_nameserver(soa);
    }

    e->get_response()->add_answers(answer->get_answer_records(dns_rr::T_CNAME_e));
    e->get_response()->add_answers(answer->get_answer_records(dns_rr::T_DNAME_e));

    send_response(e);
}

void dns_recursive_slot::respond_with_nxdomain(
                    dns_message_envelope                               *e, 
                    const shared_ptr<const dns_recursive_cache_answer> &answer)
{
    e->set_response(construct_base_response(e));
    e->get_response()->set_response_code(dns_message::name_error_e);

    auto soa = answer->get_soa_record();
    if (soa)
    {
        e->get_response()->add_nameserver(soa);
    }

    e->get_response()->add_answers(answer->get_answer_records(dns_rr::T_CNAME_e));
    e->get_response()->add_answers(answer->get_answer_records(dns_rr::T_DNAME_e));

    send_response(e);
}

dns_recursive_slot::answer_type_t dns_recursive_slot::classify_response(
                        const shared_ptr<const dns_recursive_cache_answer> &referred_by,
                        const shared_ptr<dns_question> &q,
                        const dns_message              *r) const
{
    switch (r->get_response_code())
    {
    case dns_message::no_error_e :
        if (has_answers_matching_question(q, r))
        {
            return dns_recursive_slot::data_e;
        }
        else if (has_additional_records_matching_question(q, r))
        {
            return dns_recursive_slot::additional_data_e;
        }
        else if ((r->get_num_answers() == 0) &&
                r->nameserver_exists(dns_rr::T_NS_e) && 
                !r->nameserver_exists(dns_rr::T_SOA_e))
        {
            // need to check that all the nameservers are for the same subdomain of the question
            // e.g.
            //  question:      a.b.c.com
            //  nameservers:   x.a.b.c.com
            set<dns_name> ns_names;
            for (auto ns : r->get_nameservers())
            {
                if (ns->get_type() == dns_rr::T_NS_e)
                {
                    ns_names.insert(*(ns->get_name()));
                }
            }

            if (ns_names.size() == 1)
            {
                auto referral_name = *(ns_names.begin());

                if (referral_name.is_subdomain_of(referred_by->get_referral_name()))
                {
                    return dns_recursive_slot::referral_e;
                }
                else
                {
                    LOG(info) << "referred back up the tree, failing " << referred_by->get_referral_name() << " " << referral_name;
                    return dns_recursive_slot::fail_e;
                }
            }
            else
            {
                LOG(info) << "got referral response with mixed referrals";
                return dns_recursive_slot::fail_e;
            }
        }
        else if ((r->get_num_answers() == 0) &&
                r->nameserver_exists(dns_rr::T_SOA_e) &&
                !r->nameserver_exists(dns_rr::T_NS_e))
        {
            return dns_recursive_slot::no_data_e;
        }
        else if (r->answer_exists(dns_rr::T_CNAME_e))
        {
            return dns_recursive_slot::cname_e;
        }
        // all resolvers I've tested add a CNAME record for every DNAME
        // so this may never actually happen
        else if (r->answer_exists(dns_rr::T_DNAME_e))
        {
            return dns_recursive_slot::dname_e;
        }
        else if ((r->get_num_answers() == 0) &&
                 (r->get_num_nameservers() == 0) &&
                 (r->get_num_additional_records() == 0))
        {
            return dns_recursive_slot::no_data_e;
        }
        else
        {
            return dns_recursive_slot::fail_e;
        }
        
    case dns_message::name_error_e :
        if (r->answer_exists(dns_rr::T_CNAME_e))
        {
            return dns_recursive_slot::cname_e;
        }
        else if (r->answer_exists(dns_rr::T_DNAME_e))
        {
            return dns_recursive_slot::dname_e;
        }
        else
        {
            return dns_recursive_slot::nxdomain_e;
        }

    case dns_message::server_failure_e :
    case dns_message::format_error_e :
    case dns_message::not_implemented_e :
    case dns_message::refused_e :
    default:
        return dns_recursive_slot::fail_e;
    }
}

bool dns_recursive_slot::has_answers_matching_question(const shared_ptr<dns_question> &q, const dns_message *r) const
{
    if (q->get_qtype() == dns_question::QT_ALL_e) 
    {
        return r->get_num_answers() > 0;
    }

    for (auto rr : r->get_answers())
    {
        if (*(rr->get_name()) == q->get_qname() && rr->get_type() == q->get_rr_type())
        {
            return true;
        }

        if ((q->get_qtype() == dns_question::QT_RR_e) && 
            (q->get_rr_type() == dns_rr::T_CNAME_e) && 
            ((rr->get_type() == dns_rr::T_A_e) || (rr->get_type() == dns_rr::T_AAAA_e)))
        {
            return true;
        }
    }

    return false;
}

bool dns_recursive_slot::has_additional_records_matching_question(const shared_ptr<dns_question> &q, const dns_message *r) const
{
    // look in additional records just in case there's glue that matches
    for (auto rr : r->get_additional_records())
    {
        if (*(rr->get_name()) == q->get_qname() && rr->get_type() == q->get_rr_type())
        {
            return true;
        }
    }

    return false;
}

dns_recursive_cache_answer::type_t dns_recursive_slot::map_answer_type(answer_type_t t) const
{
    switch (t)
    {
    case dns_recursive_slot::cname_e :
    case dns_recursive_slot::dname_e :
    case dns_recursive_slot::additional_data_e:
    case dns_recursive_slot::data_e:
        return dns_recursive_cache_answer::data_e;

    case dns_recursive_slot::no_data_e:
        return dns_recursive_cache_answer::no_data_e;

    case dns_recursive_slot::nxdomain_e:
        return dns_recursive_cache_answer::nxdomain_e;

    case dns_recursive_slot::fail_e:
        return dns_recursive_cache_answer::fail_e;

    case dns_recursive_slot::referral_e:
        return dns_recursive_cache_answer::referral_e;

    default:
        THROW(recursive_slot_exception, "unknown answer type");
    }
}
