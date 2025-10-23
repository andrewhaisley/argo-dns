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
 
#include "monitor.hpp"

using namespace std;
using namespace adns;

uint monitor::o_next_id = 0;

vector<monitor::info_t> monitor::o_things;
vector<uint>            monitor::o_uint_values;
vector<string>          monitor::o_string_values;

mutex monitor::o_lock;

uint monitor::add_thing(
            const string &monitor_class,
            const string &monitor_instance,
            const string &monitor_item,
            uint         initial_value)
{
    unique_lock<mutex> guard(o_lock);

    o_uint_values.push_back(initial_value);
    o_string_values.push_back("");

    info_t i;
    i.type = uint_e;
    i.monitor_class = monitor_class;
    i.monitor_instance = monitor_instance;
    i.monitor_item = monitor_item;
    o_things.push_back(i);

    return o_next_id++;
}

uint monitor::add_thing(
            const string      &monitor_class,
            const string      &monitor_instance,
            const string      &monitor_item,
            const std::string &initial_value)
{
    unique_lock<mutex> guard(o_lock);

    o_uint_values.push_back(0);
    o_string_values.push_back(initial_value);

    info_t i;
    i.type = string_e;
    i.monitor_class = monitor_class;
    i.monitor_instance = monitor_instance;
    i.monitor_item = monitor_item;
    o_things.push_back(i);

    return o_next_id++;
}

void monitor::set_value(uint id, uint value)
{
    unique_lock<mutex> guard(o_lock);

    if (id >= o_next_id)
    {
        THROW(monitor_exception, "invalid monitor id");
    }

    o_uint_values[id] = value;
}

void monitor::set_value(uint id, const std::string &value)
{
    unique_lock<mutex> guard(o_lock);

    if (id >= o_next_id)
    {
        THROW(monitor_exception, "invalid monitor id");
    }

    o_string_values[id] = value;
}

vector<monitor::info_t> monitor::get_things()
{
    unique_lock<mutex> guard(o_lock);

    return o_things;
}

vector<uint> monitor::get_uint_values()
{
    unique_lock<mutex> guard(o_lock);

    return o_uint_values;
}

vector<string> monitor::get_string_values()
{
    unique_lock<mutex> guard(o_lock);

    return o_string_values;
}
