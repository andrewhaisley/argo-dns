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
#include <boost/lexical_cast.hpp>
#include <algorithm>

#include "dns_horizon.hpp"
#include "row_horizon.hpp"
#include "address_list.hpp"
#include "dns_zone.hpp"
#include "connection_pool.hpp"

using namespace std;
using namespace adns;
using namespace db;

vector<dns_horizon *> dns_horizon::o_cache;

dns_horizon::dns_horizon()
{
}

uuid dns_horizon::get_horizon_id() const
{
    return m_horizon_id;
}

void dns_horizon::load_cache()
{
    auto conn = connection_pool::get_connection();

    for (auto rh : row_horizon::get_rows(*conn))
    {
        auto h = new dns_horizon();

        h->m_horizon_id = rh->get_horizon_id();
        h->m_address_list_id = rh->get_address_list_id();
        h->m_allow_recursion = rh->get_allow_recursion();
        h->m_check_order = rh->get_check_order();

        o_cache.push_back(h);
    }

    sort(o_cache.begin( ), o_cache.end( ), [](const auto& lhs, const auto& rhs)
    {
        return lhs->m_check_order < rhs->m_check_order;
    });
}

void dns_horizon::empty_cache()
{
    for (auto h : o_cache)
    {
        delete h;
    }

    o_cache.clear();
}

bool dns_horizon::address_list_used(const address_list &al)
{
    for (auto h : o_cache)
    {
        if (h->m_address_list_id == al.get_address_list_id())
        {
            return true;
        }
    }
    return false;
}

dns_horizon *dns_horizon::get(const ip_address &addr)
{
    for (auto h : o_cache)
    {
        auto al = address_list::get(h->m_address_list_id);
        if (al)
        {
            if (al->match(addr))
            {
                return h;
            }
        }
    }

    return nullptr;
}

dns_horizon *dns_horizon::get(uuid id)
{
    for (auto h : o_cache)
    {
        if (h->m_horizon_id == id)
        {
            return h;
        }
    }

    return nullptr;
}

const vector<dns_horizon *> &dns_horizon::get_all()
{
    return o_cache;
}

void dns_horizon::json_serialize() const
{
    auto al = address_list::get(m_address_list_id);

    set_json("horizon_id", to_string(m_horizon_id));
    set_json("address_list", al->to_json());
    set_json("allow_recursion", m_allow_recursion);
    set_json("check_order", int(m_check_order));
}

void dns_horizon::json_unserialize()
{
    // when inserting a new horizon, the horizon_id will be blank, so allow for that here
    try
    {
        m_horizon_id = boost::lexical_cast<uuid>(string((*m_json_object)["horizon_id"]));
    }
    catch (json_exception &e)
    {
    }

    m_address_list_id = boost::lexical_cast<uuid>(string((*m_json_object)["address_list_id"]));
    m_allow_recursion = (*m_json_object)["allow_recursion"];
    m_check_order = int((*m_json_object)["check_order"]);
}

void dns_horizon::delete_horizon_db(connection &conn)
{
    for (auto z : dns_zone::find_by_horizon(m_horizon_id))
    {
        z->delete_zone_db(conn);
    }

    auto h = row_horizon::get_by_horizon_id(conn, m_horizon_id);
    h->delete_row(conn);

}

void dns_horizon::delete_horizon_cache()
{
    for (auto z : dns_zone::find_by_horizon(m_horizon_id))
    {
        z->delete_zone_cache();
    }

    for (auto h = o_cache.begin(); h != o_cache.end(); h++)
    {
        if ((*h)->m_horizon_id == m_horizon_id)
        {
            o_cache.erase(h);
            return;
        }
    }
}

uuid dns_horizon::insert_horizon_db(db::connection &conn)
{
    row_horizon h;

    // ignore any primary key that might have been in the payload
    h.set_new_primary_key_value();
    m_horizon_id = h.get_horizon_id();

    h.set_address_list_id(m_address_list_id);
    h.set_allow_recursion(m_allow_recursion);

    h.insert_row(conn);
    return m_horizon_id;
}

void dns_horizon::insert_horizon_cache()
{
    o_cache.push_back(clone());

    sort(o_cache.begin( ), o_cache.end( ), [](const auto& lhs, const auto& rhs)
    {
        return lhs->m_check_order < rhs->m_check_order;
    });
}

void dns_horizon::update_horizon_db(db::connection &conn)
{
    row_horizon h;

    h.set_horizon_id(m_horizon_id);
    h.set_address_list_id(m_address_list_id);
    h.set_allow_recursion(m_allow_recursion);
    h.update_row(conn);
}

void dns_horizon::update_horizon_cache()
{
    for (size_t i = 0; i < o_cache.size(); i++)
    {
        if (o_cache[i]->get_horizon_id() == get_horizon_id())
        {
            delete o_cache[i];
            o_cache[i] = clone();
            break;
        }
    }
}

dns_horizon *dns_horizon::clone() const
{
    return new dns_horizon(*this);
}

bool dns_horizon::allow_recursion() const
{
    return m_allow_recursion;
}
