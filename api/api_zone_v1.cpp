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
#include <vector>

#include <boost/algorithm/string.hpp>    
#include <boost/lexical_cast.hpp>

#include "nullable.hpp"
#include "api.hpp"
#include "api_zone_v1.hpp"
#include "dns_zone_guard.hpp"
#include "dns_zone.hpp"
#include "connection_pool.hpp"
#include "transaction.hpp"

using namespace std;
using namespace adns;
using namespace db;

shared_ptr<http_response> api_zone_v1::handle_get_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::read_e);

    json m = json(json::object_e);

    auto zone_id = p.get_resource_id();

    if (zone_id.is_null())
    {
        json v = json(json::array_e);

        // no id supplied, so show all of them filtered by name if needed
        for (auto s : dns_zone::find())
        {
            if (p.get_name_filter().is_null())
            {
                v.append(s->to_json());
            }
            else
            {
                if (regex_search(s->get_name()->to_string(), p.get_name_filter().value()))
                {
                    v.append(s->to_json());
                }
            }
        }

        m["zones"] = json(v);
    }
    else
    {
        auto zone = dns_zone::find(zone_id.value());

        if (zone)
        {
            m = zone->to_json();
        }
        else
        {
            THROW(api_resource_not_found_exception, "unknown zone requested", zone_id.value());
        }
    }

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(m));
}

shared_ptr<http_response> api_zone_v1::handle_insert_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::write_e);

    auto conn = connection_pool::get_connection();

    transaction t(*conn);

    dns_zone z;
    z.from_json(req->get_json_payload());
    z.insert_zone_db(*conn);
    t.commit();
    z.insert_zone_cache();

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(z.to_json()));
}

shared_ptr<http_response> api_zone_v1::handle_update_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::write_e);

    auto conn = connection_pool::get_connection();

    transaction t(*conn);

    dns_zone z;
    z.from_json(req->get_json_payload());

    if (z.get_zone_id().is_nil())
    {
        THROW(api_resource_not_found_exception, "no zone ID specified in payload");
    }

    z.update_zone_db(*conn);
    t.commit();
    z.update_zone_cache();

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(z.to_json()));
}

shared_ptr<http_response> api_zone_v1::handle_delete_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::write_e);

    auto conn = connection_pool::get_connection();

    transaction t(*conn);

    auto zone_id = p.get_resource_id();

    if (zone_id.is_null())
    {
        THROW(api_resource_not_found_exception, "no zone specified");
    }

    auto z = dns_zone::find(zone_id.value());

    if (z)
    {
        z->delete_zone_db(*conn);
        t.commit();
        z->delete_zone_cache();
    }
    else
    {
        THROW(api_resource_not_found_exception, "unknown zone specified", zone_id.value());
    }

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200);
}

shared_ptr<http_response> api_zone_v1::handle_request(shared_ptr<http_request> &req, const api_url_v1 &p)
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
        THROW(api_method_not_implemented_exception, "method not implemented", req->get_method());
    }
}
