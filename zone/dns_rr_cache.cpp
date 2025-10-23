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
#include <algorithm>
#include <random>
#include "dns_rr_cache.hpp"
#include "dns_rr_dal.hpp"

using namespace std;
using namespace adns;
using namespace db;

dns_rr_cache::dns_rr_cache()
{
}

dns_rr_cache::~dns_rr_cache()
{
}

void dns_rr_cache::read_records(const dns_zone &zone, dns_rr::type_t t)
{
    // forwarded zones have no records
    if (zone.get_is_forwarded())
    {
        return;
    }

    auto z = m_cache.find(zone.get_zone_id());

    if (z == m_cache.end())
    {
        m_cache[zone.get_zone_id()] = map<dns_rr::type_t, list<shared_ptr<dns_rr>>>();
    }

    auto rr = m_cache[zone.get_zone_id()].find(t);

    if (rr == m_cache[zone.get_zone_id()].end())
    {
        m_cache[zone.get_zone_id()][t] = list<shared_ptr<dns_rr>>();
        dns_rr_dal::read_resource_records(zone.get_zone_id(), t, m_cache[zone.get_zone_id()][t]);

        switch (t)
        {
        case dns_rr::T_SOA_e :
            if (m_cache[zone.get_zone_id()][t].empty())
            {
                THROW(rr_cache_exception, "no SOA found for zone", zone.get_zone_id());
            }
            m_SOA_cache[zone.get_zone_id()] = shared_ptr<dns_rr_SOA>(dynamic_cast<dns_rr_SOA *>(m_cache[zone.get_zone_id()][t].front()->clone()));
            break;

        case dns_rr::T_NS_e :
            if (m_cache[zone.get_zone_id()][t].empty())
            {
                THROW(rr_cache_exception, "no NS records found for zone", zone.get_zone_id());
            }

            for (auto &ns : m_cache[zone.get_zone_id()][t])
            {
                if (*ns->get_name() == *zone.get_name())
                {
                    m_NS_cache[zone.get_zone_id()].push_back(shared_ptr<dns_rr_NS>(dynamic_cast<dns_rr_NS *>(ns->clone())));
                }
                else
                {
                    m_delegated_NS_cache[zone.get_zone_id()][*ns->get_name()].push_back(shared_ptr<dns_rr_NS>(dynamic_cast<dns_rr_NS *>(ns->clone())));
                }
            }
            break;

        case dns_rr::T_CNAME_e :
            for (auto &cname : m_cache[zone.get_zone_id()][t])
            {
                m_CNAME_cache[zone.get_zone_id()][*cname->get_name()] = shared_ptr<dns_rr_CNAME>(dynamic_cast<dns_rr_CNAME *>(cname->clone()));
            }
            break;

        case dns_rr::T_DNAME_e :
            for (auto &dname : m_cache[zone.get_zone_id()][t])
            {
                m_DNAME_cache[zone.get_zone_id()][*dname->get_name()] = shared_ptr<dns_rr_DNAME>(dynamic_cast<dns_rr_DNAME *>(dname->clone()));
            }
            break;

        default:
            break;
        }
    }
}

shared_ptr<dns_rr> dns_rr_cache::get_resource_record(const dns_zone &zone, uuid id)
{
    lock_guard<mutex> guard(m_lock);

    auto rrl  = get_resource_records(zone, false);
    for (auto rr : rrl)
    {
        if (rr->get_id() == id)
        {
            return rr;
        }
    }

    return nullptr;
}

list<shared_ptr<dns_rr>> dns_rr_cache::get_resource_records(const dns_zone &zone, bool shuffle)
{
    list<shared_ptr<dns_rr>> res;

    lock_guard<mutex> guard(m_lock);

    for (int i = 0; i < dns_rr_dal::NUM_RESOURCE_RECORD_TYPES; i++)
    {
        read_records(zone, dns_rr_dal::rr_types[i]);
        res.insert(
                res.end(), 
                m_cache[zone.get_zone_id()][dns_rr_dal::rr_types[i]].begin(),
                m_cache[zone.get_zone_id()][dns_rr_dal::rr_types[i]].end());
    }

    shuffle_list(res, shuffle);
    return res;
}
        
list<shared_ptr<dns_rr>> dns_rr_cache::get_resource_records(const dns_zone &zone, const dns_name &name, bool shuffle)
{
    list<shared_ptr<dns_rr>> res;

    lock_guard<mutex> guard(m_lock);

    for (int i = 0; i < dns_rr_dal::NUM_RESOURCE_RECORD_TYPES; i++)
    {
        read_records(zone, dns_rr_dal::rr_types[i]);
        auto t = dns_rr_dal::rr_types[i];

        for (auto &rr : m_cache[zone.get_zone_id()][t])
        {
            if (rr->get_name()->wildcard_equals(name))
            {
                shared_ptr<dns_rr> copy(rr->clone());
                copy->set_name(make_shared<dns_name>(name));
                res.push_back(copy);
            }
        }
    }

    shuffle_list(res, shuffle);
    return res;
}

list<shared_ptr<dns_rr>> dns_rr_cache::get_resource_records(
                                        const dns_zone                 &zone,
                                        const nullable<dns_name>       &name, 
                                        const nullable<dns_rr::type_t> type)
{
    list<shared_ptr<dns_rr>> res;

    lock_guard<mutex> guard(m_lock);

    if (type.is_null())
    {
        for (int i = 0; i < dns_rr_dal::NUM_RESOURCE_RECORD_TYPES; i++)
        {
            read_records(zone, dns_rr_dal::rr_types[i]);
            for (auto &rr : m_cache[zone.get_zone_id()][dns_rr_dal::rr_types[i]])
            {
                if (name.is_null())
                {
                    res.push_back(rr);
                }
                else
                {
                    if (name.value() == *rr->get_name())
                    {
                        res.push_back(rr);
                    }
                }
            }
        }
    }
    else
    {
        read_records(zone, type.value());
        for (auto &rr : m_cache[zone.get_zone_id()][type.value()])
        {
            if (name.is_null())
            {
                res.push_back(rr);
            }
            else
            {
                if (name.value() == *rr->get_name())
                {
                    res.push_back(rr);
                }
            }
        }
    }

    return res;
}

list<shared_ptr<dns_rr>> dns_rr_cache::get_resource_records(const dns_zone &zone, const dns_name &name, dns_rr::type_t t, bool shuffle)
{
    list<shared_ptr<dns_rr>> res;

    lock_guard<mutex> guard(m_lock);
    read_records(zone, t);

    for (auto &rr : m_cache[zone.get_zone_id()][t])
    {
        if (rr->get_name()->wildcard_equals(name))
        {
            shared_ptr<dns_rr> copy(rr->clone());
            copy->set_name(make_shared<dns_name>(name));
            res.push_back(copy);
        }
    }

    shuffle_list(res, shuffle);
    return res;
}

void dns_rr_cache::shuffle_list(list<shared_ptr<dns_rr>> &rrs, bool shuffle)
{
    static random_device rd;
    static mt19937 g(rd());

    if (shuffle)
    {
        vector<shared_ptr<dns_rr>> trrs(rrs.begin(), rrs.end());

        std::shuffle(trrs.begin(), trrs.end(), g);
        rrs.clear();
        rrs.insert(rrs.end(), trrs.begin(), trrs.end());
    }
}

const list<shared_ptr<dns_rr_NS>> &dns_rr_cache::get_NS(const dns_zone &zone) 
{
    lock_guard<mutex> guard(m_lock);
    read_records(zone, dns_rr::T_NS_e);
    return m_NS_cache[zone.get_zone_id()];
}

list<shared_ptr<dns_rr_NS>> dns_rr_cache::get_delegated_NS(const dns_zone &zone, const dns_name &name) 
{
    if (*zone.get_name() == name)
    {
        return list<shared_ptr<dns_rr_NS>>();
    }

    lock_guard<mutex> guard(m_lock);
    read_records(zone, dns_rr::T_NS_e);

    dns_name t = name;

    while (t.size() > 0)
    {
        auto i = m_delegated_NS_cache[zone.get_zone_id()].find(t);
        if (i == m_delegated_NS_cache[zone.get_zone_id()].end())
        {
            t.remove_first();
        }
        else
        {
            return i->second;
        }
    }

    return list<shared_ptr<dns_rr_NS>>();
}

const shared_ptr<dns_rr_SOA> &dns_rr_cache::get_SOA(const dns_zone &zone) 
{
    lock_guard<mutex> guard(m_lock);
    read_records(zone, dns_rr::T_SOA_e);
    return m_SOA_cache[zone.get_zone_id()];
}

shared_ptr<dns_rr_CNAME> dns_rr_cache::get_CNAME(const dns_zone &zone, const dns_name &name) 
{
    lock_guard<mutex> guard(m_lock);
    read_records(zone, dns_rr::T_CNAME_e);

    auto i = m_CNAME_cache[zone.get_zone_id()].find(name);

    if (i == m_CNAME_cache[zone.get_zone_id()].end())
    {
        for (auto cn : m_CNAME_cache[zone.get_zone_id()])
        {
            if (cn.first.wildcard_equals(name))
            {
                auto res = make_shared<dns_rr_CNAME>(*cn.second);
                res->set_name(make_shared<dns_name>(name));
                return res;
            }
        }
        return nullptr;
    }
    else
    {
        return i->second;
    }
}

shared_ptr<dns_rr_DNAME> dns_rr_cache::get_DNAME(const dns_zone &zone, const dns_name &name) 
{
    lock_guard<mutex> guard(m_lock);
    read_records(zone, dns_rr::T_SOA_e);

    if (m_DNAME_cache[zone.get_zone_id()].size() == 0)
    {
        return nullptr;
    }

    dns_name t = name;

    while (t.size() > 0)
    {
        auto i = m_DNAME_cache[zone.get_zone_id()].find(t);
        if (i != m_DNAME_cache[zone.get_zone_id()].end() && (i->second->get_name()->size() < name.size()))
        {
            return i->second;
        }
        t.remove_first();
    }

    return nullptr;
}

void dns_rr_cache::delete_record_db(db::connection &conn, const std::shared_ptr<dns_rr> &rr)
{
    dns_rr_dal::delete_resource_record(conn, rr);
}

void dns_rr_cache::delete_record_cache(const std::shared_ptr<dns_rr> &rr)
{
    lock_guard<mutex> guard(m_lock);

    for (
        auto i = m_cache[rr->get_zone_id()][rr->get_type()].begin();
        i != m_cache[rr->get_zone_id()][rr->get_type()].end();
        i++)
    {
        if (rr->get_id() == (*i)->get_id())
        {
            m_cache[rr->get_zone_id()][rr->get_type()].erase(i);
            break;
        }
    }

    if (rr->get_type() == dns_rr::T_SOA_e)
    {
        m_SOA_cache.erase(rr->get_zone_id());
    }

    if (rr->get_type() == dns_rr::T_NS_e)
    {
        for (
            auto i = m_NS_cache[rr->get_zone_id()].begin();
            i != m_NS_cache[rr->get_zone_id()].end();
            i++)
        {
            if (rr->get_id() == (*i)->get_id())
            {
                m_NS_cache[rr->get_zone_id()].erase(i);
                break;
            }
        }

        for (
            auto i = m_delegated_NS_cache[rr->get_zone_id()].begin();
            i != m_delegated_NS_cache[rr->get_zone_id()].end();
            i++)
        {
            for (auto j = i->second.begin(); j != i->second.end(); j++)
            {
                if (rr->get_id() == (*j)->get_id())
                {
                    i->second.erase(j);
                    break;
                }
            }
        }
    }

    if (rr->get_type() == dns_rr::T_CNAME_e)
    {
        for (
            auto i = m_CNAME_cache[rr->get_zone_id()].begin();
            i != m_CNAME_cache[rr->get_zone_id()].end();
            i++)
        {
            if (rr->get_id() == i->second->get_id())
            {
                m_CNAME_cache[rr->get_zone_id()].erase(i);
                break;
            }
        }
    }

    if (rr->get_type() == dns_rr::T_DNAME_e)
    {
        for (
            auto i = m_DNAME_cache[rr->get_zone_id()].begin();
            i != m_DNAME_cache[rr->get_zone_id()].end();
            i++)
        {
            if (rr->get_id() == i->second->get_id())
            {
                m_DNAME_cache[rr->get_zone_id()].erase(i);
                break;
            }
        }
    }
}

void dns_rr_cache::delete_records_db(db::connection &conn, const dns_zone &zone)
{
    for (int i = 0; i < dns_rr_dal::NUM_RESOURCE_RECORD_TYPES; i++)
    {
        dns_rr_dal::delete_resource_records(conn, zone.get_zone_id(), dns_rr_dal::rr_types[i]);
    }
}

void dns_rr_cache::delete_records_cache(const dns_zone &zone)
{
    lock_guard<mutex> guard(m_lock);

    m_cache.erase(zone.get_zone_id());
    m_SOA_cache.erase(zone.get_zone_id());
    m_NS_cache.erase(zone.get_zone_id());
    m_delegated_NS_cache.erase(zone.get_zone_id());
    m_CNAME_cache.erase(zone.get_zone_id());
    m_DNAME_cache.erase(zone.get_zone_id());
}

uuid dns_rr_cache::insert_record_db(db::connection &conn, const std::shared_ptr<dns_rr> &rr)
{
    dns_rr_dal::insert_resource_record(conn, rr);
    return rr->get_id();
}

void dns_rr_cache::update_record_db(db::connection &conn, const std::shared_ptr<dns_rr> &rr)
{
    dns_rr_dal::update_resource_record(conn, rr);
}

void dns_rr_cache::insert_record_cache(const std::shared_ptr<dns_rr> &rr)
{
    auto z = dns_zone::find(rr->get_zone_id());

    if (!z)
    {
        THROW(rr_cache_exception, "no zone found for record", rr->get_zone_id());
    }

    if (m_cache.find(rr->get_zone_id()) == m_cache.end())
    {
        m_cache[rr->get_zone_id()] = map<dns_rr::type_t, list<shared_ptr<dns_rr>>>();
    }

    if (m_cache[rr->get_zone_id()].find(rr->get_type()) == m_cache[rr->get_zone_id()].end())
    {
        m_cache[rr->get_zone_id()][rr->get_type()] = list<shared_ptr<dns_rr>>();
    }

    m_cache[rr->get_zone_id()][rr->get_type()].push_back(rr);

    switch (rr->get_type())
    {
    case dns_rr::T_SOA_e :
        m_SOA_cache[rr->get_zone_id()] = shared_ptr<dns_rr_SOA>(dynamic_cast<dns_rr_SOA *>(rr->clone()));
        break;

    case dns_rr::T_NS_e :
        if (*(rr->get_name()) == *(z->get_name()))
        {
            m_NS_cache[rr->get_zone_id()].push_back(shared_ptr<dns_rr_NS>(dynamic_cast<dns_rr_NS *>(rr->clone())));
        }
        else
        {
            m_delegated_NS_cache[rr->get_zone_id()][*rr->get_name()].push_back(shared_ptr<dns_rr_NS>(dynamic_cast<dns_rr_NS *>(rr->clone())));
        }
        break;

    case dns_rr::T_CNAME_e :
        m_CNAME_cache[rr->get_zone_id()][*rr->get_name()] = shared_ptr<dns_rr_CNAME>(dynamic_cast<dns_rr_CNAME *>(rr->clone()));
        break;

    case dns_rr::T_DNAME_e :
        m_DNAME_cache[rr->get_zone_id()][*rr->get_name()] = shared_ptr<dns_rr_DNAME>(dynamic_cast<dns_rr_DNAME *>(rr->clone()));
        break;

    default:
        break;
    }
}

void dns_rr_cache::update_record_cache(const std::shared_ptr<dns_rr> &rr)
{
    delete_record_cache(rr);
    insert_record_cache(rr);
}
