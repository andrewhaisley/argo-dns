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
 
#include <iostream>
#include <set>

#include "datetime.hpp"
#include "api_monitor_v1.hpp"
#include "api.hpp"
#include "monitor.hpp"

using namespace std;
using namespace adns;

static datetime start_dt = datetime::now();

shared_ptr<http_response> api_monitor_v1::handle_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    if (req->get_method() == "GET")
    {
        return handle_get_request(req, p);
    }
    else
    {
        THROW(api_method_not_allowed_exception, "method not allowed for monitor");
    }
}

shared_ptr<http_response> api_monitor_v1::handle_get_request(shared_ptr<http_request> &req, const api_url_v1 &p)
{
    auto response = json(json::object_e);

    auto m = json(json::object_e);
    auto t = json(json::object_e);

    auto things = monitor::get_things();
    auto uint_values = monitor::get_uint_values();
    auto string_values = monitor::get_string_values();

    set<string> c;

    for (uint i = 0; i < things.size(); i++)
    {
        if (c.find(things[i].monitor_class) == c.end())
        {
            m[things[i].monitor_class] = json(json::object_e);
            c.insert(things[i].monitor_class);
        }
    }

    set<pair<string, string>> ci;

    for (uint i = 0; i < things.size(); i++)
    {
        if (ci.count(pair<string, string>(things[i].monitor_class, things[i].monitor_instance)) == 0)
        {
            m[things[i].monitor_class][things[i].monitor_instance] = json(json::object_e);
            ci.insert(pair<string, string>(things[i].monitor_class, things[i].monitor_instance));
        }
    }

    for (uint i = 0; i < things.size(); i++)
    {
        switch (things[i].type)
        {
        case monitor::uint_e:
            m[things[i].monitor_class][things[i].monitor_instance].insert(things[i].monitor_item, json(static_cast<int>(uint_values[i])));
            break;
        case monitor::string_e:
            m[things[i].monitor_class][things[i].monitor_instance].insert(things[i].monitor_item, json(string_values[i]));
            break;
        default:
            break;
        }
    }

    t["server_start_date_time"] = start_dt.to_string();
    t["server_current_date_time"] = datetime::now().to_string();

    response["datetimes"] = t;
    response["monitors"] = m;

    return make_shared<http_response>(req->get_stream_id(), http_response::ok_200, response);
}
