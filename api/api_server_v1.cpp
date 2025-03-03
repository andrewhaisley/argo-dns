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
#include "api_server_v1.hpp"
#include "api_exception.hpp"
#include "connection_pool.hpp"
#include "config.hpp"
#include "json.hpp"

using namespace std;
using namespace adns;
using namespace db;

shared_ptr<http_response> api_server_v1::handle_get_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    auto server_id = p.get_resource_id();

    if (server_id.is_null())
    {
        auto m = json(json::object_e);
        auto v = json(json::array_e);

        for (auto s : config::get_servers())
        {
            json j = s.to_json();
            v.append(j);
        }

        m["servers"] = json(v);

        return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(m));
    }
    else
    {
        for (auto s : config::get_servers())
        {
            if (s.server_id == server_id.value())   
            {
                return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, s.to_json());
            }
        }

        THROW(api_resource_not_found_exception, "unknown server", server_id.value());
    }
}

shared_ptr<http_response> api_server_v1::handle_update_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    auto server_id = p.get_resource_id();

    if (server_id.is_null())
    {
        THROW(api_resource_not_found_exception, "null server specified");
    }

    server_config sc;
    sc.from_json(req->get_json_payload());

    if (server_id.value() != sc.server_id)
    {
        THROW(api_method_conflict_exception, "id in payload doesn't match id in path");
    }

    for (auto s : config::get_servers())
    {
        if (s.server_id == server_id.value())
        {
            if (s.type == sc.type)
            {
                try
                {
                    config::update_server(sc);
                }
                catch (config_addresses_exception &e)
                {
                    THROW(api_method_conflict_exception, "overlapping socket addresses", server_id.value());
                }

                return make_shared<http_response>(req->get_stream_id(), http_response::ok_200);
            }
            else
            {
                THROW(api_method_conflict_exception, "cannot change server type, delete and insert instead", server_id.value());
            }
        }
    }

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200);
}

shared_ptr<http_response> api_server_v1::handle_insert_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    server_config sc;
    sc.from_json(req->get_json_payload());

    if (sc.type == server_config::control_e)
    {
        THROW(api_method_conflict_exception, "extra control server instance cannot be created");
    }
    else
    {
        try
        {
            config::add_server(sc);
        }
        catch (config_addresses_exception &e)
        {
            THROW(api_method_conflict_exception, "overlapping socket addresses");
        }
    }

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200);
}

shared_ptr<http_response> api_server_v1::handle_delete_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    auto server_id = p.get_resource_id();

    if (server_id.is_null())
    {
        THROW(api_resource_not_found_exception, "null server specified");
    }

    for (auto s : config::get_servers())
    {
        if (s.server_id == server_id.value())
        {
            if (s.type == server_config::control_e)
            {
                THROW(api_method_conflict_exception, "control server instance cannot be deleted", server_id.value());
            }
            else
            {
                config::delete_server(s);

                return make_shared<http_response>(req->get_stream_id(), http_response::ok_200);
            }
        }
    }

    THROW(api_resource_not_found_exception, "unknown server", server_id.value());
}

shared_ptr<http_response> api_server_v1::handle_request(shared_ptr<http_request> &req, const api_url_v1 &p)
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
        THROW(api_method_not_allowed_exception, "method not allowed for server");
    }
}
