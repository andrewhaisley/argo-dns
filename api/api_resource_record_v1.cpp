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
#include "api_resource_record_v1.hpp"
#include "dns_zone.hpp"
#include "dns_zone_guard.hpp"
#include "dns_rr_parser.hpp"
#include "connection_pool.hpp"
#include "transaction.hpp"
#include "unparser.hpp"

using namespace std;
using namespace adns;
using namespace db;

shared_ptr<http_response> api_resource_record_v1::handle_get_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::read_e);

    auto zone_id = p.get_resource_filter_id(api_url_v1::zone_e);

    if (zone_id.is_null())
    {
        THROW(api_resource_record_exception, "no zone filter given");
    }

    auto zone = dns_zone::find(zone_id.value());
    auto rr_id = p.get_resource_id();
    json m = json(json::object_e);

    if (rr_id.is_null())
    {
        // get all resource records for the zone matching name and type as appropriate
        json v = json(json::array_e);

        for (auto rr : zone->get_resource_records())
        {
            if (!p.get_name_filter().is_null())
            {
                if (!regex_search(rr->get_name()->to_string(), p.get_name_filter().value()))
                {
                    continue;
                }
            }

            if (!p.get_record_type_filter().is_null())
            {
                if (rr->get_type() != p.get_record_type_filter().value())
                {
                    continue;
                }
            }

            v.append(rr->to_json());
        }

        m["resource_records"] = json(v);
    }
    else
    {
        auto rr = zone->get_resource_record(rr_id.value());

        if (rr)
        {
            return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(rr->to_json()));
        }
        else
        {
            THROW(api_resource_not_found_exception, "unknown resource record requested", rr_id.value());
        }
    }

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(m));
}

shared_ptr<http_response> api_resource_record_v1::handle_update_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::write_e);

    auto conn = connection_pool::get_connection();
    transaction t(*conn);

    dns_rr_parser rp;

    auto rr = rp.from_json(req->get_json_payload());
    auto z = dns_zone::find(rr->get_zone_id());

    if (!z)
    {
        THROW(api_resource_record_exception, "zone not found");
    }

    z->update_record_db(*conn, rr);

    t.commit();

    z->update_record_cache(rr);
    
    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200);
}

shared_ptr<http_response> api_resource_record_v1::handle_insert_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::write_e);

    auto conn = connection_pool::get_connection();
    transaction t(*conn);

    dns_rr_parser rp;

    auto j = req->get_json_payload();

    if (j.get_instance_type() == json::object_e)
    {
        auto rr = rp.from_json(j);
        auto z = dns_zone::find(rr->get_zone_id());

        if (!z)
        {
            THROW(api_resource_record_exception, "zone not found");
        }

        z->insert_record_db(*conn, rr);
        t.commit();
        z->insert_record_cache(rr);
        
        return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(rr->to_json()));
    }
    else if (j.get_instance_type() == json::array_e)
    {
        json res(json::array_e);

        for (auto &jp : j.get_array())
        {
            auto rr = rp.from_json(*jp);
            auto z = dns_zone::find(rr->get_zone_id());

            if (!z)
            {
                THROW(api_resource_record_exception, "zone not found");
            }

            z->insert_record_db(*conn, rr);
            res.append(rr->to_json());
        }

        t.commit();

        for (auto &jp : j.get_array())
        {
            auto rr = rp.from_json(*jp);
            auto z = dns_zone::find(rr->get_zone_id());
            z->insert_record_cache(rr);
        }

        return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(res));
    }
    else
    {
        THROW(api_resource_record_exception, "invalid payload type");
    }
}

shared_ptr<http_response> api_resource_record_v1::handle_delete_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    dns_zone_guard guard(dns_zone_guard::write_e);

    auto conn = connection_pool::get_connection();
    transaction t(*conn);

    auto zone_id = p.get_resource_filter_id(api_url_v1::zone_e);
    auto rr_id = p.get_resource_id();

    if (zone_id.is_null())
    {
        THROW(api_resource_record_exception, "no zone filter given");
    }

    if (rr_id.is_null())
    {
        THROW(api_resource_record_exception, "no resource record given");
    }

    auto z = dns_zone::find(zone_id.value());
    auto rr = z->get_resource_record(rr_id.value());
    z->delete_record_db(*conn, rr);

    t.commit();

    z->delete_record_cache(rr);
    
    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200);
}

shared_ptr<http_response> api_resource_record_v1::handle_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    if (req->get_method() == "GET")
    {
        return handle_get_request(req, p);
    }
    else if (req->get_method() == "POST")
    {
        return handle_insert_request(req, p);
    }
    else if (req->get_method() == "PUT")
    {
        return handle_update_request(req, p);
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
