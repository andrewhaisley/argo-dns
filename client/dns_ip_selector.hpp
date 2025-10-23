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
 
#pragma once

#include <thread>
#include <mutex>
#include <set>
#include <map>
#include <chrono>

#include "types.hpp"
#include "ip_address.hpp"

namespace adns
{
    class dns_ip_selector
    {
    public:

        dns_ip_selector() = delete;

        /**
         * mark the last time we tried to contact an address as now
         */
        static void update_last_contact(const ip_address &i);

        /**
         * mark an address as non-responsive
         */
        static void mark_dead(const ip_address &i);

        /**
         * select the least recently contacted from a set (aim is not to get rate throttled)
         * @param IN    all_ips - set to choose from
         * @param OUT   selected_ips - selection made
         * @param IN    include_dead - whether to include dead IPs or not
         * @param IN    num_to_select - how many we want
         */
        static void select_ips(const std::set<ip_address> &all_ips, std::set<ip_address> &selected_ips, bool include_dead, uint num_to_select);

    private:

        static std::map<ip_address, std::chrono::time_point<std::chrono::steady_clock>> o_last_contact;
        static std::map<ip_address, std::chrono::time_point<std::chrono::steady_clock>> o_dead_ips;
        static std::mutex o_last_contact_m;

        static void find_ips(
                const std::set<ip_address> &all_ips, 
                std::vector<std::pair<std::chrono::time_point<std::chrono::steady_clock>, ip_address>> &ip_by_lc, 
                bool include_dead);

    };
}
