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

#include <boost/lexical_cast.hpp>
 
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
            THROW(api_resource_not_found_exception, "unknown address_list requested", boost::lexical_cast<string>(address_list_id.value()));
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
