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
#include "api.hpp"
#include "api_address_list_v1.hpp"
#include "row.hpp"
#include "address_list.hpp"
#include "dns_zone_guard.hpp"
#include "dns_zone.hpp"
#include "connection_pool.hpp"
#include "transaction.hpp"
#include "config.hpp"
#include "server_config.hpp"

using namespace std;
using namespace adns;
using namespace db;

shared_ptr<http_response> api_address_list_v1::handle_get_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::read_e);

    json m = json(json::object_e);

    auto address_list_id = p.get_resource_id();

    if (address_list_id.is_null())
    {
        json v = json(json::array_e);

        // no id supplied, so show all of them
        for (auto s : address_list::get_all())
        {
            v.append(s.second->to_json());
        }

        m["address_lists"] = json(v);
    }
    else
    {
        auto address_list = address_list::get(address_list_id.value());

        if (address_list)
        {
            m = address_list->to_json();
        }
        else
        {
            THROW(api_resource_not_found_exception, "unknown address_list requested", address_list_id.value());
        }
    }

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(m));
}

shared_ptr<http_response> api_address_list_v1::handle_update_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::write_e);

    auto conn = connection_pool::get_connection();

    transaction t(*conn);

    address_list h;
    h.from_json(req->get_json_payload());

    if (h.get_address_list_id().is_nil())
    {
        THROW(api_resource_not_found_exception, "null address_list specified in payload");
    }

    h.update_address_list_db(*conn);

    t.commit();

    // Finally, update the cache. Done after DB commit to ensure that, if that fails for some reason,
    // we don't end up with the DB and the cache out of step.
    h.update_address_list_cache();

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200);
}

shared_ptr<http_response> api_address_list_v1::handle_insert_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::write_e);

    auto conn = connection_pool::get_connection();

    transaction t(*conn);

    address_list al;
    json j = req->get_json_payload();
    al.from_json(req->get_json_payload());
    al.insert_address_list_db(*conn);

    t.commit();

    // Finally, put it in the cache. Done after DB commit to ensure that, if that fails for some reason,
    // we don't end up with the DB and the cache out of step.

    al.insert_address_list_cache();

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(al.to_json()));
}

shared_ptr<http_response> api_address_list_v1::handle_delete_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::write_e);

    auto conn = connection_pool::get_connection();

    transaction t(*conn);

    auto address_list_id = p.get_resource_id();

    if (address_list_id.is_null())
    {
        THROW(api_resource_not_found_exception, "no address_list specified");
    }

    auto al = address_list::get(address_list_id.value());

    if (al == nullptr)
    {
        THROW(api_resource_not_found_exception, "no such address_list");
    }

    // make sure there are no horizons that use this address list
    if (dns_horizon::address_list_used(*al))
    {
        THROW(api_method_conflict_exception, "address_list used by horizon");
    }

    // make sure there are no control servers that use this address list
    list<server_config> servers = config::get_servers();

    for (auto iter : servers)
    {
        if (iter.type == server_config::control_e)
        {
            if (iter.control.control_address_list->get_address_list_id() == address_list_id.value())
            {
                THROW(api_method_conflict_exception, "address_list used by control server");
            }
        }
    }

    al->delete_address_list_db(*conn);

    t.commit();

    al->delete_address_list_cache();

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200);
}

shared_ptr<http_response> api_address_list_v1::handle_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    if (req->get_method() == "GET")
    {
        return handle_get_request(req, p);
    }
    else if (req->get_method() == "PUT")
    {
        return handle_update_request(req, p);
    }
    else if (req->get_method() == "POST")
    {
        return handle_insert_request(req, p);
    }
    else if (req->get_method() == "DELETE")
    {
        return handle_delete_request(req, p);
    }
    else
    {
        THROW(api_method_not_allowed_exception, "method not allowed for address_list");
    }
}
