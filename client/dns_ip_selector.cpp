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
#include "dns_ip_selector.hpp"

using namespace std;
using namespace chrono;
using namespace adns;

map<ip_address, time_point<steady_clock>> dns_ip_selector::o_last_contact;
map<ip_address, time_point<steady_clock>> dns_ip_selector::o_dead_ips;
mutex dns_ip_selector::o_last_contact_m;

void dns_ip_selector::update_last_contact(const ip_address &i)
{
    unique_lock<mutex> guard(o_last_contact_m);
    o_last_contact[i] = steady_clock::now();
}

void dns_ip_selector::mark_dead(const ip_address &i)
{
    unique_lock<mutex> guard(o_last_contact_m);
    o_dead_ips[i] = steady_clock::now();
}

void dns_ip_selector::find_ips(const set<ip_address> &all_ips, vector<pair<time_point<steady_clock>, ip_address>> &ip_by_lc, bool include_dead)
{
    for (auto i : all_ips)
    {
        if (!include_dead && o_dead_ips.count(i) != 0)
        {
            continue;
        }        

        auto lc = o_last_contact.find(i);
        if (lc == o_last_contact.end())
        {
            ip_by_lc.push_back(pair<time_point<steady_clock>, ip_address>(steady_clock::now(), i));
        }
        else
        {
            ip_by_lc.push_back(pair<time_point<steady_clock>, ip_address>(lc->second, i));
        }
    }
}

void dns_ip_selector::select_ips(const set<ip_address> &all_ips, set<ip_address> &selected_ips, bool include_dead, uint num_to_select)
{
    if (all_ips.size() <= num_to_select)
    {
        selected_ips = all_ips;
    }
    else
    {
        vector<pair<time_point<steady_clock>, ip_address>> ip_by_lc;

        unique_lock<mutex> guard(o_last_contact_m);

        find_ips(all_ips, ip_by_lc, include_dead);

        // if we can't find enough, include dead ones regardless
        if (ip_by_lc.size() <= num_to_select)
        {
            find_ips(all_ips, ip_by_lc, true);
        }

        sort(ip_by_lc.begin(), ip_by_lc.end());
        selected_ips.clear();

        for (size_t i = 0; i < num_to_select; i++)
        {
            if (i < ip_by_lc.size())
            {
                selected_ips.insert(ip_by_lc[i].second);
            }
            else
            {
                break;
            }
        }
    }
}
