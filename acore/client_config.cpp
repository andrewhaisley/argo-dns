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
#include <boost/lexical_cast.hpp> 

#include "client_config.hpp"
#include "exception.hpp"

using namespace std;
using namespace adns;

void client_config::json_serialize() const
{
    set_json("client_id", to_string(client_id));
    set_json("server_port", int(server_port));
    set_json("use_ip4", use_ip4);
    set_json("use_ip6", use_ip6);
    set_json("use_udp", use_udp);
    set_json("use_tcp", use_tcp);
    set_json("num_parallel_udp", int(num_parallel_udp));
    set_json("total_timeout_ms", int(total_timeout_ms));
    set_json("udp_timeout_ms", int(udp_timeout_ms));
    set_json("wait_udp_response_ms", int(wait_udp_response_ms));
    set_json("connect_tcp_timeout_ms", int(connect_tcp_timeout_ms));
    set_json("read_tcp_timeout_ms", int(read_tcp_timeout_ms));
    set_json("write_tcp_timeout_ms", int(write_tcp_timeout_ms));
}

void client_config::json_unserialize()
{
    // may not have an ID when inserting a new client
    try
    {
        client_id = boost::lexical_cast<uuid>(string((*m_json_object)["client_id"]));
    }
    catch (json_exception &e)
    {
    }

    server_port = int((*m_json_object)["server_port"]);
    use_ip4 = (*m_json_object)["use_ip4"];
    use_ip6 = (*m_json_object)["use_ip6"];
    use_udp = (*m_json_object)["use_udp"];
    use_tcp = (*m_json_object)["use_tcp"];
    num_parallel_udp = int((*m_json_object)["num_parallel_udp"]);
    total_timeout_ms = int((*m_json_object)["total_timeout_ms"]);
    udp_timeout_ms = int((*m_json_object)["udp_timeout_ms"]);
    wait_udp_response_ms = int((*m_json_object)["wait_udp_response_ms"]);
    connect_tcp_timeout_ms = int((*m_json_object)["connect_tcp_timeout_ms"]);
    read_tcp_timeout_ms = int((*m_json_object)["read_tcp_timeout_ms"]);
    write_tcp_timeout_ms = int((*m_json_object)["write_tcp_timeout_ms"]);
}
