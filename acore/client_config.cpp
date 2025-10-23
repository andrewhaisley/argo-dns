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
