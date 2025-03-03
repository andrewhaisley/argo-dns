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
#include "config.hpp"
#include "api.hpp"
#include "api_cache_configuration_v1.hpp"
#include "connection_pool.hpp"
#include "transaction.hpp"

using namespace std;
using namespace adns;
using namespace db;

shared_ptr<http_response> api_cache_configuration_v1::handle_get_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, config::cache_config_to_json());
}

shared_ptr<http_response> api_cache_configuration_v1::handle_update_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    config::set_cache_config_from_json(req->get_json_payload());
    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, config::cache_config_to_json());
}

shared_ptr<http_response> api_cache_configuration_v1::handle_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    if (req->get_method() == "GET")
    {
        return handle_get_request(req, p);
    }
    else if (req->get_method() == "PUT")
    {
        return handle_update_request(req, p);
    }
    else
    {
        THROW(api_method_not_allowed_exception, "method not allowed for cache configuration");
    }
}
