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
