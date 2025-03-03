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
