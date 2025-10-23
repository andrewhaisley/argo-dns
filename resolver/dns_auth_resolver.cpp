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
 
#include <set>
#include <list>
#include <mutex>

#include "types.hpp"
#include "dns_auth_resolver.hpp"
#include "dns_message_parser.hpp"
#include "dns_rr_NS.hpp"
#include "dns_rr_SOA.hpp"
#include "dns_rr_CNAME.hpp"
#include "dns_rr_DNAME.hpp"
#include "dns_rr_SRV.hpp"

using namespace std;
using namespace adns;

shared_ptr<dns_auth_resolver> dns_auth_resolver::get_instance(const server_config &server_config)
{
    static shared_ptr<dns_auth_resolver> instance;
    static mutex lock;
    lock_guard<std::mutex> guard(lock);

    if (!instance)
    {
        instance.reset(new dns_auth_resolver(server_config));
    }

    return instance;
}

dns_auth_resolver::dns_auth_resolver(const server_config &server_config)
{
}

dns_auth_resolver::~dns_auth_resolver()
{
}

shared_ptr<dns_name> dns_auth_resolver::apply_dname(const dns_name &name, const dns_name &dname_from, const dns_name &dname_to)
{
    // name:
    //      a.b.c.com
    // dname:
    //      b.c.com -> x.c.com
    // cname:
    //      a.x.c.com
    
    if (name.size() < dname_from.size())
    {
        THROW(auth_resolver_fail_exception, "invalid DNAME application");
    }

    auto pre_labels = name.get_labels(name.size() - dname_from.size());

    shared_ptr<dns_name> new_name(dname_to.clone());

    new_name->prepend(pre_labels);

    return new_name;
}

dns_name dns_auth_resolver::traverse_pointer_chain(
                    const dns_horizon        &h, 
                    const dns_name           &qname, 
                    bool                     &loop_found, 
                    list<shared_ptr<dns_rr>> &pointer_records)
{
    dns_name name = qname;
    loop_found = false;
    pointer_records.clear();

    set<string> pointers_applied;

    auto zone = dns_zone::find(h.get_horizon_id(), qname);

    while (zone)
    {
        // is there a CNAME record for the thing we're querying? CNAMES take priority
        // over DNAMES as they're more specific (there's nothing in the RFCs that I can
        // easily find that clarify this point). 
        auto cname = zone->get_CNAME(name);

        if (cname)
        {
            string s = cname->to_string_no_ttl();

            if (pointers_applied.find(s) == pointers_applied.end())
            {
                pointers_applied.insert(cname->to_string_no_ttl());
            }
            else
            {
                loop_found = true;
                break;
            }

            pointer_records.push_back(cname);
            name = *(cname->get_cname());
            zone = dns_zone::find(h.get_horizon_id(), name);
        }
        else
        {
            auto dname = zone->get_DNAME(name);

            if (dname)
            {
                string s = dname->to_string_no_ttl();

                if (pointers_applied.find(s) == pointers_applied.end())
                {
                    pointers_applied.insert(dname->to_string_no_ttl());
                }
                else
                {
                    loop_found = true;
                    break;
                }

                pointers_applied.insert(dname->to_string_no_ttl());
                pointer_records.push_back(dname);

                // name:
                //      a.b.c.com
                // dname:
                //      b.c.com -> x.c.com
                // cname:
                //      a.x.c.com
                auto new_name = apply_dname(name, *dname->get_name(), *dname->get_dname());

                auto cname = make_shared<dns_rr_CNAME>();
                cname->set_name(name.clone());
                cname->set_cname(new_name);
                cname->set_ttl(dname->get_ttl());
                pointer_records.push_back(cname);

                name = *new_name;

                zone = dns_zone::find(h.get_horizon_id(), name);
            }
            else
            {
                break;
            }
        }
    }
    
    return name;
}

dns_message *dns_auth_resolver::resolve_soa(const dns_horizon &h, dns_message *request)
{
    // Special case of query for the SOA.
    // If the query is at the zone level (e.g. blah.com):
    //      Add the SOA to the answer section
    //      Add NS to the authority section
    //      Add any glue (A or AAAA) we have for the zone level to the additional section
    // Else:
    //      If we have any records of any type for the name queried, then add the SOA as is to the authority section. NOERROR.
    //          In addition, if the query is for a delegated domain (i.e. we have NS records), then put them + glue into the response
    //      If we have no records of any type, then set the SOA TTL to 0 and add it to the authority section. NXDOMAIN.

    auto response = create_response(request);

    bool loop_found = false;

    list<shared_ptr<dns_rr>> pointer_records;

    auto query_name = traverse_pointer_chain(h, request->get_question()->get_qname(), loop_found, pointer_records);

    response->add_answers(pointer_records);

    if (loop_found) 
    {
        LOG(error) << "found pointer loop for " << query_name;
        response->set_response_code(dns_message::no_error_e);
        return response;
    }

    auto zone = dns_zone::find(h.get_horizon_id(), query_name);

    if (!zone)
    {
        // points to zone not managed by this server
        response->set_response_code(dns_message::no_error_e);
        return response;
    }

    if (query_name == *(zone->get_name()))
    {
        response->set_response_code(dns_message::no_error_e);

        list<shared_ptr<dns_rr>> answers;
        answers.push_back(zone->get_SOA());
        response->add_answers(answers);
        response->add_nameservers(zone->get_NS());
        response->add_additional_records(get_glue(h, answers, zone->get_NS()));
    }
    else
    {
        auto rrs = zone->get_resource_records(dns_question(query_name, dns_question::QT_ALL_e));

        if (rrs.size() == 0)
        {
            auto ns_rrs = zone->get_delegated_NS(query_name);

            if (ns_rrs.size() == 0)
            {
                shared_ptr<dns_rr> copy_soa(zone->get_SOA()->clone());
                copy_soa->set_ttl(0);
                response->add_nameserver(copy_soa);
                response->set_response_code(dns_message::name_error_e);
            }
            else
            {
                response->set_is_authoritative(false);

                response->add_nameservers(ns_rrs);
                response->set_response_code(dns_message::no_error_e);
                response->add_additional_records(get_glue(h, list<shared_ptr<dns_rr>>(), ns_rrs));
                response->set_response_code(dns_message::no_error_e);
            }
        }
        else
        {
            auto ns_rrs = zone->get_delegated_NS(query_name);

            if (ns_rrs.size() == 0)
            {
                response->add_nameserver(zone->get_SOA());
                response->set_response_code(dns_message::no_error_e);
            }
            else
            {
                response->set_is_authoritative(false);

                response->add_nameservers(ns_rrs);
                response->set_response_code(dns_message::no_error_e);
                response->add_additional_records(get_glue(h, list<shared_ptr<dns_rr>>(), ns_rrs));
                response->set_response_code(dns_message::no_error_e);
            }
        }
    }

    return response;
}

dns_message *dns_auth_resolver::resolve_ns(const dns_horizon &h, dns_message *request)
{
    // Special case of query for NS records.
    // If the query is at the zone level (e.g. blah.com):
    //      Add the SOA to the answer section
    //      Add NS to the authority section
    //      Add any glue (A or AAAA) we have for the zone level to the additional section
    // Else:
    //      If we have any records of any type for the name queried, then add the SOA as is to the authority section. NOERROR.
    //          In addition, if the query is for a delegated domain (i.e. we have NS records), then put them + glue into the response
    //      If we have no records of any type, then set the SOA TTL to 0 and add it to the authority section. NXDOMAIN.

    auto response = create_response(request);

    bool loop_found = false;

    list<shared_ptr<dns_rr>> pointer_records;

    auto query_name = traverse_pointer_chain(h, request->get_question()->get_qname(), loop_found, pointer_records);

    response->add_answers(pointer_records);

    if (loop_found) 
    {
        response->set_response_code(dns_message::no_error_e);
        return response;
    }

    auto zone = dns_zone::find(h.get_horizon_id(), query_name);

    if (!zone)
    {
        // points to zone not managed by this server
        response->set_response_code(dns_message::no_error_e);
        return response;
    }

    if (query_name == *(zone->get_name()))
    {
        response->set_response_code(dns_message::no_error_e);

        response->add_answers(zone->get_NS());
        response->add_additional_records(get_glue(h, list<shared_ptr<dns_rr>>(), zone->get_NS()));
    }
    else
    {
        auto ns_rrs = zone->get_delegated_NS(query_name);

        if (ns_rrs.size() == 0)
        {
            response->add_nameserver(zone->get_SOA());
            response->set_response_code(dns_message::name_error_e);
        }
        else
        {
            response->set_is_authoritative(false);

            response->add_nameservers(ns_rrs);
            response->set_response_code(dns_message::no_error_e);
            response->add_additional_records(get_glue(h, list<shared_ptr<dns_rr>>(), ns_rrs));
            response->set_response_code(dns_message::no_error_e);
        }
    }

    return response;
}

dns_message *dns_auth_resolver::resolve_all(const dns_horizon &h, dns_message *request)
{
    // Special case of query for the SOA. Only follow CNAME/DNAMES in limited circumstances.
    //
    // cases:
    //  1. no zone match -> NXDOMAIN
    //  2. zone match with records -> NO ERROR
    //  3. zone match with no records -> NXDOMAIN
    //  4. zone match with no records but there is a DNAME
    //  5. query is for the delegation point for a sub-domain -> NO ERROR
    //  for 2 & 3:
    //      answers are any with an exact name match
    //      authority are any NS for the zone unless they're in the answers list
    //      additional are any glue for NS unless they're in the answer section
    //  for 4:
    //      resolve the DNAME and return NO ERROR
    //  for 5:
    //      no answers, delegating NS records, in authority, glue for them in additional
    auto name = request->get_question()->get_qname();
    auto response = create_response(request);
    auto zone = dns_zone::find(h.get_horizon_id(), name);

    if (zone)
    {
        // is the request for a record in a delegated domain?
        auto delegated_nameservers = zone->get_delegated_NS(name);

        if (delegated_nameservers.size() > 0)
        {
            response->add_nameservers(delegated_nameservers);
            response->add_additional_records(get_glue(h, list<shared_ptr<dns_rr>>(), delegated_nameservers));
            response->set_is_authoritative(false);
        }
        else
        {
            auto answers = zone->get_resource_records(*request->get_question());
            if (answers.size() == 0)
            {
                bool loop_found;
                list<shared_ptr<dns_rr>> pointer_records;

                auto new_name = traverse_pointer_chain(h, name, loop_found, pointer_records);

                response->add_answers(pointer_records);

                if (new_name == name)
                {
                    response->set_response_code(dns_message::name_error_e);
                    if (!loop_found)
                    {
                        response->add_nameserver(zone->get_SOA());
                    }
                }
                else
                {
                    auto new_zone = dns_zone::find(h.get_horizon_id(), new_name);
                    if (new_zone)
                    {
                        answers = new_zone->get_resource_records(dns_question(new_name, dns_question::QT_ALL_e));
                        if (answers.size() == 0)
                        {
                            response->set_response_code(dns_message::name_error_e);
                            if (!loop_found)
                            {
                                response->add_nameserver(zone->get_SOA());
                            }
                        }
                        else
                        {
                            response->set_response_code(dns_message::no_error_e);
                            response->add_answers(answers);

                            auto ns = filter_records(answers, zone->get_NS());
                            auto glue = get_glue(h, answers, zone->get_NS());
                            glue = filter_records(answers, glue);

                            response->add_nameservers(ns);
                            response->add_additional_records(glue);
                        }
                    }
                    else
                    {
                        response->set_response_code(dns_message::no_error_e);
                    }
                }
            }
            else
            {
                response->set_response_code(dns_message::no_error_e);
                response->add_answers(answers);

                auto ns = filter_records(answers, zone->get_NS());
                auto glue = get_glue(h, answers, zone->get_NS());
                glue = filter_records(answers, glue);

                response->add_nameservers(ns);
                response->add_additional_records(glue);
            }
        }
    }
    else
    {
        response->set_response_code(dns_message::name_error_e);
    }

    return response;
}

dns_message *dns_auth_resolver::resolve(const dns_horizon &h, dns_message *request, const shared_ptr<dns_zone> &zone)
{
    // should never get passed a request at this point without a question so it really is a server
    // error if that happens
    if (!request->get_question())
    {
        THROW(auth_resolver_fail_exception, "no question set on request");
    }

    auto q = request->get_question();
    auto qt = q->get_qtype();
    auto rrt = q->get_rr_type();

    if ((qt != dns_question::QT_RR_e) && (qt != dns_question::QT_ALL_e))
    {
        // unhandled qt - should have been filtered out elsewhere
        THROW(auth_resolver_fail_exception, "invalid question type");
    }

    if (qt == dns_question::QT_ALL_e)
    {
        return resolve_all(h, request);
    }

    if ((qt == dns_question::QT_RR_e) && (rrt == dns_rr::T_SOA_e))
    {
        return resolve_soa(h, request);
    }

    if ((qt == dns_question::QT_RR_e) && (rrt == dns_rr::T_NS_e))
    {
        return resolve_ns(h, request);
    }

    dns_message *response = create_response(request);

    auto query_name = request->get_question()->get_qname();
    bool loop_found = false;
    list<shared_ptr<dns_rr>> pointer_records;

    // traverse any CNAME/DNAME chain - at the end of this we'll know the
    // actual thing we're returning records for.
    query_name = traverse_pointer_chain(h, query_name, loop_found, pointer_records);

    response->add_answers(pointer_records);

    auto final_zone = dns_zone::find(h.get_horizon_id(), query_name);

    if (final_zone)
    {
        // we manage the ultimate zone pointed at by the query
        auto answers = final_zone->get_resource_records(dns_question(query_name, qt, rrt), true);

        if (answers.size() == 0)
        {
            auto ns = final_zone->get_delegated_NS(query_name);

            if (ns.size() == 0)
            {
                if (!loop_found)    
                {
                    response->add_nameserver(final_zone->get_SOA());
                }
            }
            else
            {
                auto glue = get_glue(h, answers, ns);
                response->add_nameservers(ns);
                response->add_additional_records(glue);
                response->set_is_authoritative(false);
            }
        }
        else
        {
            response->add_answers(answers);

            auto ns = filter_records(answers, final_zone->get_NS());
            auto glue = get_glue(h, answers, final_zone->get_NS());
            glue = filter_records(answers, glue);

            response->add_nameservers(ns);
            response->add_additional_records(glue);
        }

        response->set_response_code(dns_message::no_error_e);
    }
    else
    {
        // we don't manage the ultimate zone pointed at by the query
    }

    return response;
}

dns_message *dns_auth_resolver::create_response(dns_message *request)
{
    dns_message *response(new dns_message);

    response->set_question(request->get_question());
    response->set_id(request->get_id());
    response->set_type(dns_message::response_e);
    response->set_op_code(dns_message::op_query_e);
    response->set_is_authoritative(true);
    
    return response;
}

template<class RR> list<shared_ptr<dns_rr>> dns_auth_resolver::filter_records(
                        const list<shared_ptr<dns_rr>> &answers, 
                        const list<shared_ptr<RR>>     &records)
{
    list<shared_ptr<dns_rr>> new_records;

    // Order(N^2) search. This is likely quicker than sorting the
    // records first since there will rarely be more than a couple of
    // glue or ns/soa records. The pathological case of thousands of each
    // seems unlikely so it doesn't make sense to slow down the vastly 
    // most common case to address that one.
    for (auto r : records)
    {
        bool found = false;

        for (auto a : answers)
        {
            if (*r == *a)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            new_records.push_back(r);
        }
    }

    return new_records;
}

template<class RR> list<shared_ptr<dns_rr>> dns_auth_resolver::get_glue(
                    const dns_horizon              &h,
                    const list<shared_ptr<dns_rr>> &answers,
                    const list<shared_ptr<RR>>     &ns_records)
{
    list<shared_ptr<dns_rr>> res;

    for (auto nsrr : ns_records)
    {
        dns_rr_NS *ns = dynamic_cast<dns_rr_NS*>(nsrr.get());

        // the glue records, if any, could be anywhere on the server
        auto glue_zone = dns_zone::find(h.get_horizon_id(), *ns->get_nsdname());

        if (glue_zone)
        {
            auto a_rr = glue_zone->get_resource_records(*(ns->get_nsdname()), dns_rr::T_A_e);
            auto aaaa_rr = glue_zone->get_resource_records(*(ns->get_nsdname()), dns_rr::T_AAAA_e);

            res.insert(res.end(), a_rr.begin(), a_rr.end());
            res.insert(res.end(), aaaa_rr.begin(), aaaa_rr.end());
        }
    }

    for (auto x : answers)
    {
        if (x->get_type() == dns_rr::T_SRV_e)
        {
            dns_rr_SRV *srv = dynamic_cast<dns_rr_SRV *>(x.get());

            auto srv_zone = dns_zone::find(h.get_horizon_id(), *srv->get_target());

            if (srv_zone)
            {
                auto a_rr = srv_zone->get_resource_records(*(srv->get_target()), dns_rr::T_A_e);
                auto aaaa_rr = srv_zone->get_resource_records(*(srv->get_target()), dns_rr::T_AAAA_e);

                res.insert(res.end(), a_rr.begin(), a_rr.end());
                res.insert(res.end(), aaaa_rr.begin(), aaaa_rr.end());
            }
        }
    }

    return res;
}
