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
#include <boost/uuid/uuid_generators.hpp>

#include "address_list.hpp"

#include "row_address_list.hpp"
#include "row_address_list_address.hpp"
#include "row_address_list_network.hpp"
#include "connection_pool.hpp"
#include "transaction.hpp"
#include "table.hpp"

using namespace std;
using namespace adns;
using namespace db;

map<uuid, shared_ptr<address_list>> address_list::o_cache;

void address_list::load_cache()
{
    auto conn = connection_pool::get_connection();

    for (auto alr : row_address_list::get_rows(*conn))
    {
        auto al = make_shared<address_list>();

        al->m_address_list_id = alr->get_address_list_id();

        for (auto rn : row_address_list_network::get_by_address_list_id(*conn, alr->get_address_list_id()))
        {
            al->add(rn->get_address_list_network_id(), network(rn->get_cidr_address()));
        }

        for (auto ra : row_address_list_address::get_by_address_list_id(*conn, alr->get_address_list_id()))
        {
            al->add(ra->get_address_list_address_id(), ip_address(ra->get_ip_address()));
        }

        o_cache[alr->get_address_list_id()] = al;
    }
}

shared_ptr<address_list> address_list::get(uuid id)
{
    auto i = o_cache.find(id);
    if (i == o_cache.end())
    {
        return nullptr;
    }
    else
    {
        return i->second;
    }
}

const map<uuid, shared_ptr<address_list>> &address_list::get_all()
{
    return o_cache;
}

bool address_list::match(const ip_address &addr) const
{
    for (auto n : m_networks)
    {
        if (n.second.get_type() == addr.get_type() && n.second.contains(addr))
        {
            return true;
        }
    }

    for (auto ip : m_ips)
    {
        if (ip.second.get_type() == addr.get_type() && ip.second == addr)
        {
            return true;
        }
    }

    return false;
}

address_list::address_list()
{
}

void address_list::add(uuid id, const ip_address &addr)
{
    m_ips[id] = addr;
}

void address_list::add(uuid id, const network &addr)
{
    m_networks[id] = addr;
}

uuid address_list::insert_address_list_db(db::connection &conn)
{
    row_address_list ral;

    // ignore any primary key that might have been in the payload
    ral.set_new_primary_key_value();

    m_address_list_id = ral.get_address_list_id();

    for (auto ip : m_ips)
    {
        row_address_list_address rala;
        rala.set_new_primary_key_value();
        rala.set_address_list_id(m_address_list_id);
        rala.set_ip_address(ip.second.to_string());

        rala.insert_row(conn);
    }

    for (auto network : m_networks)
    {
        row_address_list_network raln;
        raln.set_new_primary_key_value();
        raln.set_address_list_id(m_address_list_id);
        raln.set_cidr_address(network.second.to_string());

        raln.insert_row(conn);
    }

    // insert this last - if the underlying DB doesn't support
    // transactions this results in the least level of mess
    // to clean up should something go wrong
    ral.insert_row(conn);

    return m_address_list_id;
}

void address_list::insert_address_list_cache()
{
    o_cache[m_address_list_id] = shared_ptr<address_list>(clone());
}

void address_list::update_address_list_db(db::connection &conn)
{
    // delete the current persisted ips and networks
    row_address_list_address::delete_by_address_list_id(conn, m_address_list_id);
    row_address_list_network::delete_by_address_list_id(conn, m_address_list_id);

    // insert a whole new set of ips and networks
    for (auto ip : m_ips)
    {
        row_address_list_address rala;
        rala.set_new_primary_key_value();
        rala.set_address_list_id(m_address_list_id);
        rala.set_ip_address(ip.second.to_string());

        rala.insert_row(conn);
    }

    for (auto network : m_networks)
    {
        row_address_list_network raln;
        raln.set_new_primary_key_value();
        raln.set_address_list_id(m_address_list_id);
        raln.set_cidr_address(network.second.to_string());

        raln.insert_row(conn);
    }

}

void address_list::update_address_list_cache()
{
    o_cache[m_address_list_id] = shared_ptr<address_list>(clone());
}

void address_list::delete_address_list_db(db::connection &conn)
{
    auto al = row_address_list::get_by_address_list_id(conn, m_address_list_id);
    al->delete_row(conn);

    for (auto rn : row_address_list_network::get_by_address_list_id(conn, m_address_list_id))
    {
        rn->delete_row(conn);
    }

    for (auto ra : row_address_list_address::get_by_address_list_id(conn, m_address_list_id))
    {
        ra->delete_row(conn);
    }
}

void address_list::delete_address_list_cache()
{
    o_cache.erase(m_address_list_id);
}

void address_list::json_serialize() const
{
    set_json("address_list_id", to_string(m_address_list_id));

    auto ips = json(json::array_e);
    auto nws = json(json::array_e);

    for (auto i : m_ips)
    {
        ips.append(i.second.to_json());
    }

    set_json("address_list_addresses", ips);

    for (auto n : m_networks)
    {
        nws.append(n.second.to_json());
    }

    set_json("address_list_networks", nws);
}

void address_list::json_unserialize()
{
    // when inserting a new address list, the id will be blank, so allow for that here
    try
    {
        m_address_list_id = boost::lexical_cast<uuid>(string((*m_json_object)["address_list_id"]));
    }
    catch (json_exception &e)
    {
    }

    m_ips.clear();
    m_networks.clear();

    if (m_json_object->has("address_list_addresses"))
    {
        for (const auto &c : (*m_json_object)["address_list_addresses"].get_array())
        {
            ip_address i;
            i.from_json(*c);
            m_ips[boost::uuids::random_generator()()] = i;
        }
    }

    if (m_json_object->has("address_list_networks"))
    {
        for (const auto &c : (*m_json_object)["address_list_networks"].get_array())
        {
            network n;
            n.from_json(*c);
            m_networks[boost::uuids::random_generator()()] = n;
        }
    }
}

shared_ptr<address_list> address_list::clone() const
{
    return make_shared<address_list>(*this);
}

uuid address_list::get_address_list_id() const
{
    return m_address_list_id;
}

void address_list::dump() const
{
    LOG(info) << "ADDRESS LIST ID " << m_address_list_id;
    for (auto ip : m_ips)
    {
        LOG(info) << "ADDRESS LIST IP " << ip.second;
    }
    for (auto n : m_networks)
    {
        LOG(info) << "ADDRESS LIST NETWORK " << n.second;
    }
}
