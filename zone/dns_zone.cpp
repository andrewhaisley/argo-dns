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
 
#include <mutex>
#include <boost/lexical_cast.hpp>

#include "types.hpp"
#include "config.hpp"
#include "unparser.hpp"
#include "dns_zone.hpp"
#include "dns_zone_validator.hpp"
#include "connection_pool.hpp"
#include "connection_guard.hpp"
#include "connection_pool.hpp"
#include "row_zone.hpp"
#include "dns_rr_cache.hpp"
#include "dns_rr_dal.hpp"
#include "dns_rr_parser.hpp"
#include "dns_rr_SOA.hpp"
#include "dns_rr_MX.hpp"

using namespace std;
using namespace adns;
using namespace db;

static recursive_mutex o_lock;

static shared_ptr<dns_rr_cache> o_rr_cache;

map<uuid, shared_ptr<dns_zone::dns_zone_cache>> dns_zone::o_zone_cache;

void dns_zone::init()
{
    lock_guard<recursive_mutex> guard(o_lock);

    if (!o_rr_cache)
    {
        o_rr_cache = make_shared<dns_rr_cache>();

        for (auto h : dns_horizon::get_all())
        {
            try
            {
                o_zone_cache[h->get_horizon_id()] = make_shared<dns_zone_cache>(h->get_horizon_id());
            }
            catch (adns::exception &e)
            {
                e.log(boost::log::trivial::error, "exception whilst caching horizon, ignoring");
            }
        }
    }
}

dns_zone::dns_zone()
{
    m_zone_id = boost::uuids::nil_uuid();
    m_horizon_id = boost::uuids::nil_uuid();
    m_is_forwarded = false;
    m_forward_port = 0;
}

void dns_zone::delete_zone_db(connection &conn)
{
    o_rr_cache->delete_records_db(conn, *this);
    auto row = row_zone::get_by_zone_id(conn, get_zone_id());
    row->delete_row(conn);
}

void dns_zone::delete_zone_cache()
{
    lock_guard<recursive_mutex> guard(o_lock);
    o_rr_cache->delete_records_cache(*this);
    o_zone_cache[get_horizon_id()]->delete_zone(*this);
}

dns_zone::dns_zone(
            uuid                        zone_id, 
            const shared_ptr<dns_name>  &name,
            uuid                        horizon_id, 
            bool                        is_forwarded,
            const nullable<ip_address>  &forward_ip_address,
            nullable<uint>              forward_port)
{
    m_zone_id = zone_id;
    m_name = name;
    m_horizon_id = horizon_id;
    m_is_forwarded = is_forwarded;

    if (m_is_forwarded)
    {
        m_forward_ip_address = forward_ip_address.value();
        m_forward_port = forward_port.value();
    }
}

dns_zone::~dns_zone()
{
}

uuid dns_zone::get_horizon_id() const
{
    return m_horizon_id;
}

bool dns_zone::get_is_forwarded() const
{
    return m_is_forwarded;
}

uuid dns_zone::get_zone_id() const
{
    return m_zone_id;
}

void dns_zone::dump() const
{
    LOG(debug) 
            << "zone_id " 
            << m_zone_id 
            << "horizon_id " 
            << m_horizon_id 
            << " name " 
            << *m_name
            << " is_forwarded " 
            << (m_is_forwarded ? "TRUE" : "FALSE");
}

const shared_ptr<dns_name> &dns_zone::get_name() const
{
    return m_name;
}

void dns_zone::from_json(const json &j)
{
    // when inserting a new zone, the zone_id will be blank, so allow for that here
    try
    {
        m_zone_id = boost::lexical_cast<uuid>(string(j["zone_id"]));
    }
    catch (json_exception &e)
    {
    }

    m_name = make_shared<dns_name>(j["name"]);
    m_horizon_id = boost::lexical_cast<uuid>(string(j["horizon_id"]));
    m_is_forwarded = j["is_forwarded"];

    if (m_is_forwarded)
    {
        m_forward_ip_address = ip_address(j["forward_ip_address"]);
        m_forward_port = int(j["forward_port"]);
    }
    else
    {
        dns_rr_parser p;

        // we store the resource records in a temporary list that only gets added to the 
        // cache when the appropriate method is called. This is to preserve DB and 
        // cache consistancy.
        auto authority = j["authority"];

        if (authority != json())
        {
            auto soa = authority["soa"];
            auto nsrs = authority["ns"];

            m_pending_resource_records.push_back(p.from_json(soa));

            for (auto &i : nsrs.get_array())
            {
                m_pending_resource_records.push_back(p.from_json(*i));
            }
        }

        auto rrs = j["resource_records"];

        if (rrs != json())
        {
            for (auto &i : rrs.get_array())
            {
                m_pending_resource_records.push_back(p.from_json(*i));
            }
        }
    }
}

json dns_zone::to_json() const
{
    json m(json::object_e);

    m["zone_id"] = to_string(m_zone_id);
    m["name"] = m_name->to_string();
    m["horizon_id"] = to_string(m_horizon_id);
    m["is_forwarded"] = m_is_forwarded;

    if (m_is_forwarded)
    {
        m["forward_ip_address"] = m_forward_ip_address.to_string();
        m["forward_port"] = int(m_forward_port);
    }

    auto rrs = get_resource_records();

    if (rrs.size() > 0)
    {
        // For a non-forwarded zone, SOA and NS records have their own section since the zone is invalid
        // without them.
        if (!m_is_forwarded)
        {
            json auth = json(json::object_e);
            auto SOA = get_SOA();
            if (SOA)
            {
                auth["soa"] = SOA->to_json();
            }
            json ns = json(json::array_e);

            for (auto nsr : o_rr_cache->get_NS(*this))
            {
                if (*nsr->get_name() == *get_name())
                {
                    ns.append(nsr->to_json());
                }
            }

            auth["ns"] = ns;
            m["authority"] = auth;
        }

        json j = json(json::array_e);

        for (auto rr : rrs)
        {
            if (rr->get_type() == dns_rr::T_SOA_e)
            {
                continue;
            }

            if ((rr->get_type() == dns_rr::T_NS_e) && (*rr->get_name() == *get_name()))
            {
                continue;
            }

            j.append(rr->to_json());
        }

        m["resource_records"] = j;
    }

    return m;
}

shared_ptr<dns_zone> dns_zone::find(const uuid &horizon_id, const dns_name &name)
{
    lock_guard<recursive_mutex> guard(o_lock);

    auto i = o_zone_cache.find(horizon_id);
    if (i == o_zone_cache.end())
    {
        return nullptr;
    }
    else
    {
        return i->second->find(name);
    }
}

shared_ptr<dns_zone> dns_zone::find(uuid id)
{
    lock_guard<recursive_mutex> guard(o_lock);

    for (auto i : o_zone_cache)
    {
        auto z = i.second->find(id);
        if (z != nullptr)
        {
            return z;
        }
    }

    return nullptr;
}

shared_ptr<dns_zone> dns_zone::find_exact(const uuid &horizon_id, const dns_name &name)
{
    lock_guard<recursive_mutex> guard(o_lock);

    auto i = o_zone_cache.find(horizon_id);
    if (i == o_zone_cache.end())
    {
        return nullptr;
    }
    else
    {
        return i->second->find_exact(name);
    }
}

list<shared_ptr<dns_zone>> dns_zone::find_by_horizon(const uuid &horizon_id)
{
    lock_guard<recursive_mutex> guard(o_lock);

    auto i = o_zone_cache.find(horizon_id);
    if (i == o_zone_cache.end())
    {
        return list<shared_ptr<dns_zone>>();
    }
    else
    {
        return i->second->find();
    }
}

const list<shared_ptr<dns_rr_NS>> &dns_zone::get_NS() const
{
    return o_rr_cache->get_NS(*this);
}

const ip_address &dns_zone::get_forward_ip_address() const
{
    if (m_is_forwarded)
    {
        return m_forward_ip_address;
    }
    else
    {
        THROW(dns_zone_exception, "zone not forwarded");
    }
}

int dns_zone::get_forward_port() const
{
    if (m_is_forwarded)
    {
        return m_forward_port;
    }
    else
    {
        THROW(dns_zone_exception, "zone not forwarded");
    }
}

list<shared_ptr<dns_rr_NS>> dns_zone::get_delegated_NS(const dns_name &name) const
{
    return o_rr_cache->get_delegated_NS(*this, name);
}

const shared_ptr<dns_rr_SOA> &dns_zone::get_SOA() const
{
    return o_rr_cache->get_SOA(*this);
}

const shared_ptr<dns_rr_CNAME> dns_zone::get_CNAME(const dns_name &name) const
{
    return o_rr_cache->get_CNAME(*this, name);
}

const shared_ptr<dns_rr_DNAME> dns_zone::get_DNAME(const dns_name &name) const
{
    return o_rr_cache->get_DNAME(*this, name);
}

shared_ptr<dns_rr> dns_zone::get_resource_record(uuid id) const
{
    return o_rr_cache->get_resource_record(*this, id);
}

list<shared_ptr<dns_rr>> dns_zone::get_resource_records(bool shuffle) const
{
    return o_rr_cache->get_resource_records(*this, shuffle);
}

list<shared_ptr<dns_rr>> dns_zone::get_resource_records(const dns_name &name, bool shuffle) const
{
    return o_rr_cache->get_resource_records(*this, name, shuffle);
}

list<shared_ptr<dns_rr>> dns_zone::get_resource_records(const dns_name &name, dns_rr::type_t t, bool shuffle) const
{
    return o_rr_cache->get_resource_records(*this, name, t, shuffle);
}

list<shared_ptr<dns_rr>> dns_zone::get_resource_records(const dns_question &q, bool shuffle) const
{
    if (q.get_qtype() == dns_question::QT_ALL_e)
    {
        return get_resource_records(q.get_qname(), shuffle);
    }
    else if (q.get_qtype() == dns_question::QT_RR_e)
    {
        return get_resource_records(q.get_qname(), q.get_rr_type(), shuffle);
    }
    else
    {
        THROW(dns_zone_exception, "invalid question type for get_resource_records()");
    }
}

list<shared_ptr<dns_zone>> dns_zone::find()
{
    lock_guard<recursive_mutex> guard(o_lock);

    list<shared_ptr<dns_zone>> res;

    for (auto i : o_zone_cache)
    {
        auto s = i.second->find();
        res.insert(res.end(), s.begin(), s.end());
    }

    return res;
}

list<shared_ptr<dns_rr>> dns_zone::get_resource_records(
                                const nullable<dns_name>       &name, 
                                const nullable<dns_rr::type_t> type) const
{
    return o_rr_cache->get_resource_records(*this, name, type);
}

dns_zone::dns_zone_cache::dns_zone_cache(const uuid &horizon_id)
{
    m_horizon_id = horizon_id;
    read_data(horizon_id);
}

dns_zone::dns_zone_cache::~dns_zone_cache()
{
}

void dns_zone::dns_zone_cache::insert_zone(const shared_ptr<dns_zone> &z)
{
    m_zones_by_id[z->get_zone_id()] = z;
    m_zones_by_name[*(z->get_name())] = z;
}

void dns_zone::dns_zone_cache::read_data(const uuid &horizon_id)
{
    m_zones_by_id.clear();
    m_zones_by_name.clear();

    auto conn = connection_pool::get_connection();

    LOG(info) << "reading zone data for horizon " << horizon_id;

    for (auto row : row_zone::get_by_horizon_id(*conn, horizon_id))
    {
        uuid zone_id = row->get_zone_id();

        shared_ptr<dns_zone> z(
                new dns_zone(
                    zone_id, 
                    make_shared<dns_name>(row->get_name()),
                    row->get_horizon_id(),
                    row->get_is_forwarded(),
                    row->get_forward_ip_address().is_null() ? nullable<ip_address>() : ip_address(row->get_forward_ip_address().value()),
                    row->get_forward_port()));

        insert_zone(z);
    }
}

shared_ptr<dns_zone> dns_zone::dns_zone_cache::find(const dns_name &name)
{
    dns_name t = name;

    while (true)
    {
        auto i = m_zones_by_name.find(t);
        if (i != m_zones_by_name.end())
        {
            return i->second;
        }

        if (t.size() == 0)
        {
            return nullptr;
        }
        else
        {
            t.remove_first();
        }
    }

}

shared_ptr<dns_zone> dns_zone::dns_zone_cache::find_exact(const dns_name &name)
{
    dns_name t = name;

    auto i = m_zones_by_name.find(t);
    if (i == m_zones_by_name.end())
    {
        return nullptr;
    }
    else
    {
        return i->second;
    }
}

list<shared_ptr<dns_zone>> dns_zone::dns_zone_cache::find()
{
    list<shared_ptr<dns_zone>> res;

    for (auto i : m_zones_by_id)
    {
        res.push_back(i.second);
    }

    return res;
}

void dns_zone::dns_zone_cache::delete_zone(const dns_zone &z)
{
    m_zones_by_id.erase(z.get_zone_id());
    m_zones_by_name.erase(*z.get_name());
}

shared_ptr<dns_zone> dns_zone::dns_zone_cache::find(uuid id)
{
    auto i = m_zones_by_id.find(id);
    if (i == m_zones_by_id.end())
    {
        return nullptr;
    }
    else
    {
        return i->second;
    }
}

uuid dns_zone::insert_zone_db(connection &conn)
{
    // forwarded zones don't have records etc and the name can be blank if we're
    // forwarding everything elsewhere
    if (!m_is_forwarded)
    {
        dns_zone_validator::validate_insert(*this);
    }

    // check for duplicates first
    auto existing_zone = find_exact(m_horizon_id, *m_name);

    // check for duplicates first
    if (existing_zone)
    {
        THROW(dns_zone_exception, "attempt to insert duplicate zone", m_name->to_string());
    }

    row_zone z;

    // ignore any primary key that might have been in the payload
    z.set_new_primary_key_value();
    m_zone_id = z.get_zone_id();

    z.set_name(m_name->to_string());
    z.set_horizon_id(m_horizon_id);
    z.set_is_forwarded(m_is_forwarded);

    if (m_is_forwarded)
    {
        z.set_forward_ip_address(m_forward_ip_address.to_string());
        z.set_forward_port(m_forward_port);
    }

    z.insert_row(conn);

    if (!m_is_forwarded) 
    {
        for (auto rr : m_pending_resource_records)
        {
            rr->set_zone_id(m_zone_id);
            o_rr_cache->insert_record_db(conn, rr);
        }
    }

    return m_zone_id;
}

void dns_zone::update_zone_db(connection &conn)
{
    dns_zone_validator::validate_update(*this);

    // check that the zone exists and that the name hasn't been changed
    auto existing_zone = find(m_zone_id);

    if (existing_zone)
    {
        if (*(existing_zone->m_name) != *m_name)
        {
            THROW(dns_zone_exception, "zone names can't be changed, create a new one instead", boost::lexical_cast<string>(m_zone_id));
        }
    }
    else
    {
        THROW(dns_zone_exception, "zone not found", boost::lexical_cast<string>(m_zone_id));
    }

    row_zone z;

    z.set_name(m_name->to_string());
    z.set_horizon_id(m_horizon_id);
    z.set_is_forwarded(m_is_forwarded);

    if (m_is_forwarded)
    {
        z.set_forward_ip_address(m_forward_ip_address.to_string());
        z.set_forward_port(m_forward_port);
    }

    z.update_row(conn);

    if (m_pending_resource_records.size() > 0)
    {
        o_rr_cache->delete_records_db(conn, *this);

        for (auto rr : m_pending_resource_records)
        {
            rr->set_zone_id(m_zone_id);
            o_rr_cache->insert_record_db(conn, rr);
        }
    }
}

shared_ptr<dns_zone> dns_zone::clone() const
{
    auto res = make_shared<dns_zone>();

    res->m_zone_id = m_zone_id;
    res->m_horizon_id = m_horizon_id;
    res->m_is_forwarded = m_is_forwarded;
    res->m_name = m_name;
    res->m_forward_ip_address = m_forward_ip_address;
    res->m_forward_port = m_forward_port;

    return res;
}

void dns_zone::insert_zone_cache()
{
    lock_guard<recursive_mutex> guard(o_lock);


    if (o_zone_cache.find(m_horizon_id) == o_zone_cache.end())
    {
        o_zone_cache[m_horizon_id] = make_shared<dns_zone_cache>(m_horizon_id);
    }

    o_zone_cache[m_horizon_id]->insert_zone(clone());

    for (auto rr : m_pending_resource_records)
    {
        o_rr_cache->insert_record_cache(rr);
    }

    m_pending_resource_records.clear();
}

void dns_zone::update_zone_cache()
{
    lock_guard<recursive_mutex> guard(o_lock);

    o_rr_cache->delete_records_cache(*this);
    o_zone_cache[get_horizon_id()]->delete_zone(*this);

    o_zone_cache[m_horizon_id]->insert_zone(clone());

    for (auto rr : m_pending_resource_records)
    {
        o_rr_cache->insert_record_cache(rr);
    }

    m_pending_resource_records.clear();
}

void dns_zone::delete_records_db(connection &conn, const list<shared_ptr<dns_rr>> &rrs)
{
    for (auto &rr : rrs)
    {
        o_rr_cache->delete_record_db(conn, rr);
    }
}

void dns_zone::delete_records_cache(const list<shared_ptr<dns_rr>> &rrs)
{
    for (auto &rr : rrs)
    {
        o_rr_cache->delete_record_cache(rr);
    }
}

void dns_zone::delete_record_db(connection &conn, const shared_ptr<dns_rr> &rr)
{
    dns_zone_validator::validate_delete_record(*this, rr);

    o_rr_cache->delete_record_db(conn, rr);
}

void dns_zone::delete_record_cache(const shared_ptr<dns_rr> &rr)
{
    o_rr_cache->delete_record_cache(rr);
}

uuid dns_zone::insert_record_db(connection &conn, const shared_ptr<dns_rr> &rr)
{
    dns_zone_validator::validate_insert_record(*this, rr);

    return o_rr_cache->insert_record_db(conn, rr);
}

void dns_zone::insert_record_cache(const shared_ptr<dns_rr> &rr)
{
    o_rr_cache->insert_record_cache(rr);
}

void dns_zone::update_record_db(connection &conn, const shared_ptr<dns_rr> &rr)
{
    dns_zone_validator::validate_update_record(*this, rr);

    o_rr_cache->update_record_db(conn, rr);
}

void dns_zone::update_record_cache(const shared_ptr<dns_rr> &rr)
{
    o_rr_cache->update_record_cache(rr);
}


const list<shared_ptr<dns_rr>> &dns_zone::get_pending_resource_records() const
{
    return m_pending_resource_records;
}
