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

#include "unparser.hpp"
#include "address_list.hpp"
#include "exception.hpp"
#include "server_config.hpp"

using namespace std;
using namespace adns;

server_config::server_type_t server_config::string_to_server_type(const string &protocol, const string &transport)
{
    string s = protocol + "." + transport;

    if (s == "dns.udp")
        return server_config::dns_udp_e;
    else if (s == "dns.tcp")
        return server_config::dns_tcp_e;
    else if (s == "dns.tls")
        return server_config::dns_dot_e;
    else if (s == "dns.https")
        return server_config::dns_doh_e;
    else if (s == "control.tcp")
        return server_config::control_e;
    else if (s == "ui.tcp")
        return server_config::ui_e;

    THROW(server_config_exception, "unrecognised server type", s);
}

void server_config::json_serialize() const
{
    set_json("enabled", enabled);
    set_json("server_id", to_string(server_id));
    set_json("protocol", protocol);
    set_json("transport", transport);
    set_json("http2_deflate_table_size", int(http2_deflate_table_size));

    if (protocol == "dns")
    {
        json dnsj(json::object_e);

        dnsj["max_in_message_queue_length"] = int(dns.max_in_message_queue_length);
        dnsj["max_out_message_queue_length"] = int(dns.max_out_message_queue_length);
        dnsj["num_udp_threads"] = int(dns.num_udp_threads);
        dnsj["num_tcp_threads"] = int(dns.num_tcp_threads);
        dnsj["num_forwarding_threads"] = int(dns.num_forwarding_threads);
        dnsj["num_send_threads"] = int(dns.num_send_threads);
        dnsj["num_receive_threads"] = int(dns.num_receive_threads);
        dnsj["allowed_connection_backlog"] = int(dns.allowed_connection_backlog);
        dnsj["tcp_max_len_wait_ms"] = int(dns.tcp_max_len_wait_ms);
        dnsj["tcp_max_body_wait_ms"] = int(dns.tcp_max_body_wait_ms);
        dnsj["num_udp_recursive_slots"] = int(dns.num_udp_recursive_slots);
        dnsj["num_tcp_recursive_slots"] = int(dns.num_tcp_recursive_slots);
        dnsj["recursive_timeout_ms"] = int(dns.recursive_timeout_ms);
        dnsj["max_recursion_depth"] = int(dns.max_recursion_depth);
        dnsj["max_external_resolutions"] = int(dns.max_external_resolutions);
        dnsj["min_cache_ttl"] = int(dns.min_cache_ttl);
        dnsj["udp_reply_timeout_ms"] = int(dns.udp_reply_timeout_ms);
        dnsj["udp_retry_count"] = int(dns.udp_retry_count);
        dnsj["max_queued_per_question"] = int(dns.max_queued_per_question);
        dnsj["doh_client_timeout_ms"] = int(dns.doh_client_timeout_ms);
        dnsj["maximum_http_request_size"] = int(dns.maximum_http_request_size);
        dnsj["doh_path"] = dns.doh_path;
        dnsj["use_forwarding_cache"] = dns.use_forwarding_cache;
        dnsj["use_forwarding_emergency_cache"] = dns.use_forwarding_emergency_cache;
        dnsj["forward_cache_max_age_seconds"] = int(dns.forward_cache_max_age_seconds);
        dnsj["forward_cache_max_entries"] = int(dns.forward_cache_max_entries);
        dnsj["forward_cache_garbage_collect_pct"] = int(dns.forward_cache_garbage_collect_pct);

        dnsj["client"] = dns.client.to_json();
            
        set_json("dns", dnsj);
    }
    else if (protocol == "control")
    {
        json cj(json::object_e);

        cj["address_list"] = control.control_address_list->to_json();
        cj["use_ssl"] = control.use_ssl;
        cj["num_threads"] = int(control.num_threads);
        cj["allowed_connection_backlog"] = int(control.allowed_connection_backlog);
        cj["maximum_payload_size"] = int(control.maximum_payload_size);
        cj["client_connection_timeout_ms"] = int(control.client_connection_timeout_ms);

        set_json("control", cj);
    }
    else if (protocol == "ui")
    {
        json uj(json::object_e);

        uj["address_list"] = ui.ui_address_list->to_json();
        uj["use_ssl"] = ui.use_ssl;
        uj["num_threads"] = int(ui.num_threads);
        uj["allowed_connection_backlog"] = int(ui.allowed_connection_backlog);
        uj["client_connection_timeout_ms"] = int(ui.client_connection_timeout_ms);
        uj["document_root"] = ui.document_root;

        set_json("ui", uj);
    }
    else
    {
        THROW(server_config_exception, "unrecognised server protocol", protocol);
    }

    json sas = json(json::array_e);

    for (auto sa : socket_addresses)
    {
        json saj = json(json::object_e);

        saj["ip_address"] = sa.ip_address;
        saj["port"] = int(sa.port);

        sas.append(saj);
    }

    set_json("socket_addresses", sas);
}

void server_config::json_unserialize()
{
    // when inserting a new server, server id will be blank. Handle that here
    try
    {
        server_id = boost::lexical_cast<uuid>(string((*m_json_object)["server_id"]));
    }
    catch (json_exception &e)
    {
    }

    enabled = bool((*m_json_object)["enabled"]);
    protocol = string((*m_json_object)["protocol"]);
    transport = string((*m_json_object)["transport"]);
    http2_deflate_table_size = int((*m_json_object)["http2_deflate_table_size"]);

    type = string_to_server_type(protocol, transport);

    if (protocol == "dns")
    {
        json dnsj = (*m_json_object)["dns"];

        dns.max_in_message_queue_length =       int(dnsj["max_in_message_queue_length"]);
        dns.max_out_message_queue_length =      int(dnsj["max_out_message_queue_length"]);
        dns.num_udp_threads =                   int(dnsj["num_udp_threads"]);
        dns.num_tcp_threads =                   int(dnsj["num_tcp_threads"]);
        dns.num_forwarding_threads =            int(dnsj["num_forwarding_threads"]);
        dns.num_send_threads =                  int(dnsj["num_send_threads"]);
        dns.num_receive_threads =               int(dnsj["num_receive_threads"]);
        dns.allowed_connection_backlog =        int(dnsj["allowed_connection_backlog"]);
        dns.tcp_max_len_wait_ms =               int(dnsj["tcp_max_len_wait_ms"]);
        dns.tcp_max_body_wait_ms =              int(dnsj["tcp_max_body_wait_ms"]);
        dns.num_udp_recursive_slots =           int(dnsj["num_udp_recursive_slots"]);
        dns.num_tcp_recursive_slots =           int(dnsj["num_tcp_recursive_slots"]);
        dns.recursive_timeout_ms =              int(dnsj["recursive_timeout_ms"]);
        dns.max_recursion_depth =               int(dnsj["max_recursion_depth"]);
        dns.max_external_resolutions=           int(dnsj["max_external_resolutions"]);
        dns.min_cache_ttl =                     int(dnsj["min_cache_ttl"]);
        dns.udp_reply_timeout_ms =              int(dnsj["udp_reply_timeout_ms"]);
        dns.udp_retry_count =                   int(dnsj["udp_retry_count"]);
        dns.max_queued_per_question =           int(dnsj["max_queued_per_question"]);
        dns.doh_path =                          string(dnsj["doh_path"]);
        dns.doh_client_timeout_ms =             int(dnsj["doh_client_timeout_ms"]);
        dns.maximum_http_request_size =         int(dnsj["maximum_http_request_size"]);
        dns.use_forwarding_cache =              bool(dnsj["use_forwarding_cache"]);
        dns.use_forwarding_emergency_cache =    bool(dnsj["use_forwarding_emergency_cache"]);
        dns.forward_cache_max_age_seconds =     int(dnsj["forward_cache_max_age_seconds"]);
        dns.forward_cache_max_entries =         int(dnsj["forward_cache_max_entries"]);
        dns.forward_cache_garbage_collect_pct = int(dnsj["forward_cache_garbage_collect_pct"]);


        dns.client.from_json(dnsj["client"]);
    }
    else if (protocol == "control")
    {
        if (transport != "tcp")
        {
            THROW(server_config_exception, "control server can only run over tcp", transport);
        }

        json cj = (*m_json_object)["control"];

        control.control_address_list = make_shared<address_list>();
        control.control_address_list->from_json(cj["address_list"]);

        control.use_ssl =                       cj["use_ssl"];
        control.num_threads =                   int(cj["num_threads"]);
        control.allowed_connection_backlog =    int(cj["allowed_connection_backlog"]);
        control.maximum_payload_size =          int(cj["maximum_payload_size"]);
        control.client_connection_timeout_ms =  int(cj["client_connection_timeout_ms"]);
    }
    else if (protocol == "ui")
    {
        if (transport != "tcp")
        {
            THROW(server_config_exception, "UI server can only run over tcp", transport);
        }

        json uj = (*m_json_object)["ui"];

        ui.ui_address_list = make_shared<address_list>();
        ui.ui_address_list->from_json(uj["address_list"]);

        ui.use_ssl =                       uj["use_ssl"];
        ui.num_threads =                   int(uj["num_threads"]);
        ui.allowed_connection_backlog =    int(uj["allowed_connection_backlog"]);
        ui.client_connection_timeout_ms =  int(uj["client_connection_timeout_ms"]);
        ui.document_root =                 string(uj["document_root"]);
    }
    else
    {
        THROW(server_config_exception, "unrecognised server protocol", protocol);
    }

    socket_addresses.clear();

    for (const auto &i : (*m_json_object)["socket_addresses"].get_array())
    {
        socket_address_t sa;

        sa.ip_address = string((*i)["ip_address"]);
        sa.port = int((*i)["port"]);

        socket_addresses.push_back(sa);
    }
}

bool server_config::operator==(const server_config &other) const
{
    return server_id == other.server_id;
}
