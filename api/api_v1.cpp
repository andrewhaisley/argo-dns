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
