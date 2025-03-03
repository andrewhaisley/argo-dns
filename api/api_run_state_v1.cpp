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
#include "run_state.hpp"
#include "api.hpp"
#include "api_run_state_v1.hpp"

using namespace std;
using namespace adns;

shared_ptr<http_response> api_run_state_v1::handle_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    if (req->get_method() == "GET")
    {
        auto m = json(json::object_e);
        m["run_state"] = json(run_state::current_state_as_string());

        return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, json(m));
    }
    else if (req->get_method() == "PUT")
    {
        string rs = req->get_json_payload()["run_state"];

        run_state::set(rs);
        LOG(info) << "set run state to " << rs << " after API call";
        return make_shared<http_response>(req->get_stream_id(), http_response::ok_200);
    }
    else
    {
        THROW(api_method_not_allowed_exception, "method not allowed for run state");
    }
}
