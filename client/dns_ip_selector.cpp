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
