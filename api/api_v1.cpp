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
#include <boost/algorithm/string.hpp>    

#include "util.hpp"
#include "config.hpp"
#include "api.hpp"
#include "api_v1.hpp"
#include "api_run_state_v1.hpp"
#include "api_server_v1.hpp"
#include "api_zone_v1.hpp"
#include "api_horizon_v1.hpp"
#include "api_monitor_v1.hpp"
#include "api_resource_record_v1.hpp"
#include "api_address_list_v1.hpp"
#include "api_base_configuration_v1.hpp"
#include "api_cache_configuration_v1.hpp"

using namespace std;
using namespace adns;

api_v1::api_v1()
{
}

api_v1::~api_v1()
{
}

shared_ptr<http_response> api_v1::handle_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    if (!req->authorized())
    {
        return make_shared<http_response>(req->get_stream_id(), http_response::unauthorized_401);
    }

    switch (p.get_resource_type())
    {
        case api_url_v1::monitor_e :
            return api_monitor_v1::handle_request(req, p);
        case api_url_v1::run_e :
            return api_run_state_v1::handle_request(req, p);
        case api_url_v1::server_e :
            return api_server_v1::handle_request(req, p);
        case api_url_v1::horizon_e :
            return api_horizon_v1::handle_request(req, p);
        case api_url_v1::zone_e :
            return api_zone_v1::handle_request(req, p);
        case api_url_v1::rr_e :
            return api_resource_record_v1::handle_request(req, p);
        case api_url_v1::address_list_e :
            return api_address_list_v1::handle_request(req, p);
        case api_url_v1::cache_configuration_e :
            return api_cache_configuration_v1::handle_request(req, p);
        case api_url_v1::base_configuration_e :
            return api_base_configuration_v1::handle_request(req, p);

        default:
        case api_url_v1::none_e :
            THROW(api_resource_not_found_exception, "no resource requested");
    }
}
