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
