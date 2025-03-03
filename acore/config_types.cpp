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
#include "config_types.hpp"
#include "exception.hpp"

using namespace std;
using namespace adns;

config_types::server_type_t config_types::string_to_server_type(const std::string &protocol, const std::string &transport)
{
    string s = protocol + "." + transport;

    if (s == "dns.udp")
        return config_types::dns_udp_e;
    else if (s == "dns.tcp")
        return config_types::dns_tcp_e;
    else if (s == "control.tcp")
        return config_types::control_e;
    else if (s == "control.dot")
        return config_types::dns_dot_e;
    else if (s == "control.doh")
        return config_types::dns_doh_e;

    THROW(config_types_exception, "unrecognised server type", s);
}

