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
#include "api_horizon_v1.hpp"
#include "api.hpp"
#include "row.hpp"
#include "dns_horizon.hpp"
#include "dns_zone_guard.hpp"
#include "dns_zone.hpp"
#include "connection_pool.hpp"
#include "transaction.hpp"

using namespace std;
using namespace adns;
using namespace db;

shared_ptr<http_response> api_horizon_v1::handle_get_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::read_e);

    json m = json(json::object_e);

    auto horizon_id = p.get_resource_id();

    if (horizon_id.is_null())
    {
        json v = json(json::array_e);

        // no id supplied, so show all of them
        for (auto s : dns_horizon::get_all())
        {
            v.append(s->to_json());
        }

        m["horizons"] = json(v);
    }
    else
    {
        auto horizon = dns_horizon::get(horizon_id.value());

        if (horizon)
        {
            m = horizon->to_json();
        }
        else
        {
            THROW(api_resource_not_found_exception, "unknown horizon requested", horizon_id.value());
        }
    }

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(m));
}

shared_ptr<http_response> api_horizon_v1::handle_update_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::write_e);

    auto conn = connection_pool::get_connection();

    transaction t(*conn);

    dns_horizon h;
    h.from_json(req->get_json_payload());

    if (h.get_horizon_id().is_nil())
    {
        THROW(api_resource_not_found_exception, "null horizon specified in payload");
    }

    auto existing_h = dns_horizon::get(h.get_horizon_id());

    if (!existing_h)
    {
        THROW(api_resource_not_found_exception, "unknown horizon specified", h.get_horizon_id());
    }

    h.update_horizon_db(*conn);

    t.commit();

    h.update_horizon_cache();

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200);
}

shared_ptr<http_response> api_horizon_v1::handle_insert_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::write_e);

    auto conn = connection_pool::get_connection();

    transaction t(*conn);

    dns_horizon h;
    h.from_json(req->get_json_payload());

    h.insert_horizon_db(*conn);

    t.commit();

    h.insert_horizon_cache();

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(h.to_json()));
}

shared_ptr<http_response> api_horizon_v1::handle_delete_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::write_e);

    auto conn = connection_pool::get_connection();

    transaction t(*conn);

    auto horizon_id = p.get_resource_id();

    if (horizon_id.is_null())
    {
        THROW(api_resource_not_found_exception, "no horizon specified");
    }

    auto h = dns_horizon::get(horizon_id.value());

    if (h)
    {
        h->delete_horizon_db(*conn);
        t.commit();
        h->delete_horizon_cache();
    }
    else
    {
        THROW(api_resource_not_found_exception, "unknown horizon specified", horizon_id.value());
    }

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200);
}

shared_ptr<http_response> api_horizon_v1::handle_request(shared_ptr<http_request> &req, const api_url_v1 &p)
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
        THROW(api_method_not_allowed_exception, "method not allowed for horizon");
    }
}
