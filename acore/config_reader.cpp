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

#include "address_list.hpp"
#include "config_reader.hpp"
#include "server_config.hpp"
#include "exception.hpp"
#include "connection_pool.hpp"
#include "transaction.hpp"
#include "row_basic_config.hpp"
#include "row_cache_config.hpp"

using namespace std;
using namespace adns;
using namespace db;

config_reader::config_reader()
{
}

void config_reader::translate_client_row(client_config &c, const row_dns_client &r)
{
    c.client_id = r.get_client_id();
    c.server_port = r.get_server_port();
    c.use_ip4 = r.get_use_ip4();
    c.use_ip6 = r.get_use_ip6();
    c.use_udp = r.get_use_udp();
    c.use_tcp = r.get_use_tcp();
    c.num_parallel_udp = r.get_num_parallel_udp();
    c.total_timeout_ms = r.get_total_timeout_ms();
    c.udp_timeout_ms = r.get_udp_timeout_ms();
    c.wait_udp_response_ms = r.get_wait_udp_response_ms();
    c.connect_tcp_timeout_ms = r.get_connect_tcp_timeout_ms();
    c.read_tcp_timeout_ms = r.get_read_tcp_timeout_ms();
    c.write_tcp_timeout_ms = r.get_write_tcp_timeout_ms();
}

void config_reader::read()
{
    lock_guard<mutex> guard(m_lock);

    auto conn = connection_pool::get_connection();

    for (auto row : row_server::get_rows(*conn))
    {
        server_config s;

        s.enabled = row->get_is_enabled();
        s.server_id = row->get_server_id();
        s.type = server_config::string_to_server_type(row->get_protocol(), row->get_transport());

        s.protocol = row->get_protocol();
        s.transport = row->get_transport();
        s.http2_deflate_table_size = row->get_http2_deflate_table_size();

        for (auto sa_row : row_server_socket_address::get_by_server_id(*conn, row->get_server_id()))
        {
            server_config::socket_address_t sa;
            sa.ip_address = sa_row->get_ip_address();
            sa.port = sa_row->get_port();
            s.socket_addresses.push_back(sa);
        }

        switch (s.type)
        {
        case server_config::dns_udp_e:
        case server_config::dns_tcp_e:
        case server_config::dns_dot_e:
        case server_config::dns_doh_e:
            {
                auto dns_row = row_dns_server::get_by_server_id(*conn, row->get_server_id());

                s.dns.max_in_message_queue_length = dns_row->get_max_in_message_queue_length();
                s.dns.max_out_message_queue_length = dns_row->get_max_out_message_queue_length();
                s.dns.num_udp_threads = dns_row->get_num_udp_threads();
                s.dns.num_tcp_threads = dns_row->get_num_tcp_threads();
                s.dns.num_forwarding_threads = dns_row->get_num_forwarding_threads();
                s.dns.num_send_threads = dns_row->get_num_send_threads();
                s.dns.num_receive_threads = dns_row->get_num_receive_threads();
                s.dns.allowed_connection_backlog = dns_row->get_allowed_connection_backlog();
                s.dns.tcp_max_len_wait_ms = dns_row->get_tcp_max_len_wait_ms();
                s.dns.tcp_max_body_wait_ms = dns_row->get_tcp_max_body_wait_ms();
                s.dns.num_udp_recursive_slots = dns_row->get_num_udp_recursive_slots();
                s.dns.num_tcp_recursive_slots = dns_row->get_num_tcp_recursive_slots();
                s.dns.recursive_timeout_ms = dns_row->get_recursive_timeout_ms();
                s.dns.max_recursion_depth = dns_row->get_max_recursion_depth();
                s.dns.max_external_resolutions = dns_row->get_max_external_resolutions();
                s.dns.min_cache_ttl = dns_row->get_min_cache_ttl();
                s.dns.udp_reply_timeout_ms = dns_row->get_udp_reply_timeout_ms();
                s.dns.udp_retry_count = dns_row->get_udp_retry_count();
                s.dns.max_queued_per_question = dns_row->get_max_queued_per_question();
                s.dns.doh_client_timeout_ms = dns_row->get_doh_client_timeout_ms();
                s.dns.maximum_http_request_size = dns_row->get_maximum_http_request_size();
                s.dns.doh_path = dns_row->get_doh_path();
                s.dns.use_forwarding_cache = dns_row->get_use_forwarding_cache();
                s.dns.forward_cache_max_age_seconds = dns_row->get_forward_cache_max_age_seconds();
                s.dns.forward_cache_max_entries = dns_row->get_forward_cache_max_entries();
                s.dns.forward_cache_garbage_collect_pct = dns_row->get_forward_cache_garbage_collect_pct();

                auto dns_client_row = row_dns_client::get_by_client_id(*conn, dns_row->get_client_id());

                translate_client_row(s.dns.client, *dns_client_row);

                m_server_configs.push_back(s);
            }
            break;
        case server_config::control_e:
            {
                auto control_row = row_control_server::get_by_server_id(*conn, row->get_server_id());

                s.control.use_ssl = control_row->get_use_ssl();
                s.control.num_threads = control_row->get_num_threads();
                s.control.allowed_connection_backlog = control_row->get_allowed_connection_backlog();
                s.control.maximum_payload_size = control_row->get_maximum_payload_size();
                s.control.client_connection_timeout_ms = control_row->get_client_connection_timeout_ms();
                s.control.control_address_list = address_list::get(control_row->get_address_list_id());
                if (!s.control.control_address_list)
                {
                    THROW(config_reader_exception, "no address list found for ID", control_row->get_address_list_id());
                }
                m_server_configs.push_back(s);
            }
            break;
        case server_config::ui_e:
            {
                auto ui_row = row_ui_server::get_by_server_id(*conn, row->get_server_id());

                s.ui.use_ssl = ui_row->get_use_ssl();
                s.ui.num_threads = ui_row->get_num_threads();
                s.ui.allowed_connection_backlog = ui_row->get_allowed_connection_backlog();
                s.ui.client_connection_timeout_ms = ui_row->get_client_connection_timeout_ms();
                s.ui.ui_address_list = address_list::get(ui_row->get_address_list_id());
                s.ui.document_root = ui_row->get_document_root();
                if (!s.ui.ui_address_list)
                {
                    THROW(config_reader_exception, "no address list found for ID", ui_row->get_address_list_id());
                }
                m_server_configs.push_back(s);
            }
            break;
        default:
            LOG(warning) << "unknown server type";
        }
    }

    for (auto row : row_basic_config::get_rows(*conn))
    {
        m_raw_values[row->get_name()] = row->get_value();
        m_raw_value_ids[row->get_name()] = row->get_config_id();
    }

    auto cc_rows = row_cache_config::get_rows(*conn);

    if (cc_rows.size() == 0)
    {
        THROW(config_reader_exception, "no cache config row found");
    }

    if (cc_rows.size() > 1)
    {
        THROW(config_reader_exception, "more than one cache config row found");
    }

    auto rcc = cc_rows[0];
    auto rccc = row_dns_client::get_by_client_id(*conn, rcc->get_client_id());

    m_cache_config.cache_config_id = rcc->get_cache_config_id();
    m_cache_config.default_ttl = rcc->get_default_ttl();
    m_cache_config.cache_garbage_collect_ms = rcc->get_cache_garbage_collect_ms();
    m_cache_config.cache_max_referral_rrs = rcc->get_cache_max_referral_rrs();
    m_cache_config.cache_max_answer_rrs = rcc->get_cache_max_answer_rrs();

    translate_client_row(m_cache_config.client, *rccc);
}

config_reader::~config_reader()
{
}

string config_reader::get_string(const string &name, const string &default_value)
{
    lock_guard<mutex> guard(m_lock);

    auto item_iter = m_raw_values.find(name);

    if (item_iter == m_raw_values.end())
    {
        LOG(info) << "configuration item " << name << " not found, using default value " << default_value;
        return default_value;
    }
    else
    {
        return item_iter->second;
    }
}

int config_reader::get_int(const string &name, int default_value)
{
    lock_guard<mutex> guard(m_lock);

    auto item_iter = m_raw_values.find(name);

    if (item_iter == m_raw_values.end())
    {
        LOG(info) << "configuration item " << name << " not found, using default value " << default_value;
        return default_value;
    }
    else
    {
        try
        {
            return boost::lexical_cast<int>(item_iter->second);
        }
        catch (boost::bad_lexical_cast&)
        {
            THROW(config_reader_exception, "expecting integer in parameter value, got something else", item_iter->second);
        }
    }
}

uint config_reader::get_uint(const string &name, uint default_value)
{
    lock_guard<mutex> guard(m_lock);

    auto item_iter = m_raw_values.find(name);

    if (item_iter == m_raw_values.end())
    {
        LOG(info) << "configuration item " << name << " not found, using default value " << default_value;
        return default_value;
    }
    else
    {
        try
        {
            return boost::lexical_cast<uint>(item_iter->second);
        }
        catch (boost::bad_lexical_cast&)
        {
            THROW(config_reader_exception, "expecting uinteger in parameter value, got something else", item_iter->second);
        }
    }
}

bool config_reader::get_bool(const string &name, bool default_value)
{
    lock_guard<mutex> guard(m_lock);

    auto item_iter = m_raw_values.find(name);

    if (item_iter == m_raw_values.end())
    {
        LOG(info) << "configuration item " << name << " not found, using default value " << default_value;
        return default_value;
    }
    else
    {
        if (item_iter->second == "true")
        {
            return true;
        }
        else if (item_iter->second == "false")
        {
            return false;
        }
        else
        {
            THROW(config_reader_exception, "expecting bool paramter ('true' or 'false'), got something else", item_iter->second);
        }
    }
}

void config_reader::set_string_db(connection &conn, const string &name, const string &value)
{
    lock_guard<mutex> guard(m_lock);

    row_basic_config r;

    r.set_name(name);
    r.set_value(boost::lexical_cast<string>(value));

    if (m_raw_value_ids.find(name) == m_raw_value_ids.end())
    {
        // new DB row
        r.insert_row(conn);
        m_pending_raw_value_ids[name] = r.get_config_id();
    }
    else
    {
        // existing DB row
        r.set_config_id(m_raw_value_ids[name]);
        r.update_row(conn);
    }
}

void config_reader::set_string_cache(const string &name, const string &value)
{
    lock_guard<mutex> guard(m_lock);

    if (m_raw_value_ids.find(name) == m_raw_value_ids.end())
    {
        if (m_pending_raw_value_ids.find(name) == m_pending_raw_value_ids.end())
        {
            THROW(config_reader_exception, "raw value ID not found in pending list", name);
        }
        m_raw_value_ids[name] = m_pending_raw_value_ids[name];
        m_raw_value_ids.erase(name);
        m_raw_values[name] = value;
    }
    else
    {
        m_raw_values[name] = value;
    }
}

void config_reader::set_uint_db(connection &conn, const string &name, uint value)
{
    set_string_db(conn, name, boost::lexical_cast<string>(value));
}

void config_reader::set_bool_db(connection &conn, const string &name, bool value)
{
    set_string_db(conn, name, value ? "true" : "false");
}

void config_reader::set_uint_cache(const string &name, uint value)
{
    set_string_cache(name, boost::lexical_cast<string>(value));
}

void config_reader::set_bool_cache(const string &name, bool value)
{
    set_string_cache(name, value ? "true" : "false");
}

list<server_config> config_reader::servers()
{
    lock_guard<mutex> guard(m_lock);

    return m_server_configs;
}

cache_config config_reader::get_cache_config()
{
    lock_guard<mutex> guard(m_lock);

    return m_cache_config;
}

void config_reader::build_row(const server_config &sc, row_server &rs)
{
    rs.set_is_enabled(sc.enabled);
    rs.set_server_id(sc.server_id);
    rs.set_protocol(sc.protocol);
    rs.set_transport(sc.transport);
    rs.set_http2_deflate_table_size(sc.http2_deflate_table_size);
}

void config_reader::build_row(const server_config &sc, row_dns_server &rs)
{
    rs.set_server_id(sc.server_id);
    rs.set_max_in_message_queue_length(sc.dns.max_in_message_queue_length);
    rs.set_max_out_message_queue_length(sc.dns.max_out_message_queue_length);
    rs.set_num_udp_threads(sc.dns.num_udp_threads);
    rs.set_num_tcp_threads(sc.dns.num_tcp_threads);
    rs.set_num_forwarding_threads(sc.dns.num_forwarding_threads);
    rs.set_num_send_threads(sc.dns.num_send_threads);
    rs.set_num_receive_threads(sc.dns.num_receive_threads);
    rs.set_allowed_connection_backlog(sc.dns.allowed_connection_backlog);
    rs.set_tcp_max_len_wait_ms(sc.dns.tcp_max_len_wait_ms);
    rs.set_tcp_max_body_wait_ms(sc.dns.tcp_max_body_wait_ms);
    rs.set_num_udp_recursive_slots(sc.dns.num_udp_recursive_slots);
    rs.set_num_tcp_recursive_slots(sc.dns.num_tcp_recursive_slots);
    rs.set_recursive_timeout_ms(sc.dns.recursive_timeout_ms);
    rs.set_max_recursion_depth(sc.dns.max_recursion_depth);
    rs.set_max_external_resolutions(sc.dns.max_external_resolutions);
    rs.set_min_cache_ttl(sc.dns.min_cache_ttl);
    rs.set_udp_reply_timeout_ms(sc.dns.udp_reply_timeout_ms);
    rs.set_udp_retry_count(sc.dns.udp_retry_count);
    rs.set_max_queued_per_question(sc.dns.max_queued_per_question);
    rs.set_doh_path(sc.dns.doh_path);
    rs.set_use_forwarding_cache(sc.dns.use_forwarding_cache);
    rs.set_forward_cache_max_age_seconds(sc.dns.forward_cache_max_age_seconds);
    rs.set_forward_cache_max_entries(sc.dns.forward_cache_max_entries);
    rs.set_forward_cache_garbage_collect_pct(sc.dns.forward_cache_garbage_collect_pct);

    rs.set_doh_client_timeout_ms(sc.dns.doh_client_timeout_ms);
    rs.set_maximum_http_request_size(sc.dns.maximum_http_request_size);
}

void config_reader::build_row(const server_config &sc, row_control_server &rs)
{
    rs.set_server_id(sc.server_id);
    rs.set_address_list_id(sc.control.control_address_list->get_address_list_id());
    rs.set_use_ssl(sc.control.use_ssl);
    rs.set_num_threads(sc.control.num_threads);
    rs.set_allowed_connection_backlog(sc.control.allowed_connection_backlog);
    rs.set_maximum_payload_size(sc.control.maximum_payload_size);
    rs.set_client_connection_timeout_ms(sc.control.client_connection_timeout_ms);
}

void config_reader::build_row(const server_config &sc, row_ui_server &rs)
{
    rs.set_server_id(sc.server_id);
    rs.set_address_list_id(sc.ui.ui_address_list->get_address_list_id());
    rs.set_use_ssl(sc.ui.use_ssl);
    rs.set_num_threads(sc.ui.num_threads);
    rs.set_allowed_connection_backlog(sc.ui.allowed_connection_backlog);
    rs.set_client_connection_timeout_ms(sc.ui.client_connection_timeout_ms);
    rs.set_document_root(sc.ui.document_root);
}

void config_reader::build_socket_address_rows(const server_config &sc, vector<shared_ptr<row_server_socket_address>> &sar)
{
    for (auto sa : sc.socket_addresses)
    {
        auto rssa = make_shared<row_server_socket_address>();

        rssa->set_new_primary_key_value();
        rssa->set_server_id(sc.server_id);
        rssa->set_ip_address(sa.ip_address);
        rssa->set_port(sa.port);

        sar.push_back(rssa);
    }
}

void config_reader::add_server(server_config sc)
{
    lock_guard<mutex> guard(m_lock);

    if (sc.type != server_config::dns_udp_e && sc.type != server_config::dns_tcp_e)
    {
        THROW(config_reader_exception, "only new DNS server types can be created");
    }

    auto conn = connection_pool::get_connection();
    transaction t(*conn);

    row_server rs;
    build_row(sc, rs);
    // set a UUID for the new server
    rs.set_new_primary_key_value();
    rs.insert_row(*conn);
    sc.server_id = rs.get_server_id();

    row_dns_client rc;
    build_row(sc.dns.client, rc);
    rc.set_new_primary_key_value();
    rc.insert_row(*conn);
    sc.dns.client.client_id = rc.get_client_id();

    row_dns_server rds;
    build_row(sc, rds);
    rds.set_server_id(rs.get_server_id());
    rds.set_client_id(rc.get_client_id());
    rds.insert_row(*conn);

    vector<shared_ptr<row_server_socket_address>> sar;
    build_socket_address_rows(sc, sar);

    for (auto r : sar)
    {
        r->set_server_id(rs.get_server_id());
        r->insert_row(*conn);
    }

    t.commit();

    m_server_configs.push_back(sc);
}

void config_reader::update_server(const server_config &sc)
{
    lock_guard<mutex> guard(m_lock);

    server_config *current_server_config = nullptr;

    for (auto &esc : m_server_configs)
    {
        if (esc.server_id == sc.server_id)
        {
            current_server_config = &esc;
        }
    }

    if (!current_server_config)
    {   
        THROW(config_reader_exception, "server not found for update", to_string(sc.server_id));
    }

    auto conn = connection_pool::get_connection();
    transaction t(*conn);

    if (sc.type == server_config::dns_udp_e || sc.type == server_config::dns_tcp_e)
    {
        row_dns_client rc;
        build_row(sc.dns.client, rc);
        rc.set_client_id(current_server_config->dns.client.client_id);
        rc.update_row(*conn);

        row_server rs;
        build_row(sc, rs);
        rs.update_row(*conn);

        row_dns_server rds;
        build_row(sc, rds);
        rds.update_row(*conn);
    }
    else if (sc.type == server_config::control_e)   
    {
        if (sc.control.control_address_list)
        {
            sc.control.control_address_list->update_address_list_db(*conn);
        }
        else
        {
            THROW(config_reader_exception, "null address list found");
        }

        row_server rs;
        build_row(sc, rs);
        rs.update_row(*conn);

            row_control_server rcs;
            build_row(sc, rcs);
            rcs.update_row(*conn);
        }
        else if (sc.type == server_config::ui_e)   
        {
            if (sc.ui.ui_address_list)
            {
                sc.ui.ui_address_list->update_address_list_db(*conn);
            }
            else
            {
                THROW(config_reader_exception, "null address list found");
        }

        row_server rs;
        build_row(sc, rs);
        rs.update_row(*conn);

        row_ui_server rcs;
        build_row(sc, rcs);
        rcs.update_row(*conn);
    }
    else
    {
        THROW(config_reader_exception, "invalid server type");
    }

    row_server_socket_address::delete_by_server_id(*conn, sc.server_id);

    vector<shared_ptr<row_server_socket_address>> sar;
    build_socket_address_rows(sc, sar);

    for (auto r : sar)
    {
        r->set_server_id(sc.server_id);
        r->insert_row(*conn);
    }

    t.commit();

    if (sc.type == server_config::control_e)   
    {
        sc.control.control_address_list->update_address_list_cache();
    }

    if (sc.type == server_config::ui_e)   
    {
        sc.ui.ui_address_list->update_address_list_cache();
    }

    *current_server_config = sc;
}

void config_reader::delete_server(const server_config &sc)
{
    lock_guard<mutex> guard(m_lock);

    auto conn = connection_pool::get_connection();
    transaction t(*conn);
    row_server_socket_address::delete_by_server_id(*conn, sc.server_id);
    row_control_server::delete_by_server_id(*conn, sc.server_id);
    row_ui_server::delete_by_server_id(*conn, sc.server_id);
    row_dns_server::delete_by_server_id(*conn, sc.server_id);
    row_server::delete_by_server_id(*conn, sc.server_id);
    m_server_configs.remove(sc);

    t.commit();

    for (auto i = m_server_configs.begin(); i != m_server_configs.end(); i++)
    {
        if (sc.server_id == (*i).server_id)
        {
            m_server_configs.erase(i);
            return;
        }
    }
}

void config_reader::build_row(const client_config &c, row_dns_client &rc)
{
    rc.set_client_id(c.client_id);
    rc.set_server_port(c.server_port);
    rc.set_use_ip4(c.use_ip4);
    rc.set_use_ip6(c.use_ip6);
    rc.set_use_udp(c.use_udp);
    rc.set_use_tcp(c.use_tcp);
    rc.set_num_parallel_udp(c.num_parallel_udp);
    rc.set_total_timeout_ms(c.total_timeout_ms);
    rc.set_udp_timeout_ms(c.udp_timeout_ms);
    rc.set_wait_udp_response_ms(c.wait_udp_response_ms);
    rc.set_connect_tcp_timeout_ms(c.connect_tcp_timeout_ms);
    rc.set_read_tcp_timeout_ms(c.read_tcp_timeout_ms);
    rc.set_write_tcp_timeout_ms(c.write_tcp_timeout_ms);
}

void config_reader::update_cache_config(const cache_config &c)
{
    lock_guard<mutex> guard(m_lock);

    auto conn = connection_pool::get_connection();
    transaction t(*conn);

    row_cache_config rcc;

    rcc.set_cache_config_id(c.cache_config_id);
    rcc.set_default_ttl(c.default_ttl);
    rcc.set_cache_garbage_collect_ms(c.cache_garbage_collect_ms);
    rcc.set_cache_max_referral_rrs(c.cache_max_referral_rrs);
    rcc.set_cache_max_answer_rrs(c.cache_max_answer_rrs);
    rcc.update_row(*conn);

    row_dns_client rc;
    build_row(c.client, rc);
    rc.set_client_id(m_cache_config.client.client_id);
    rc.update_row(*conn);

    t.commit();

    m_cache_config = c;
    m_cache_config.client.client_id = rc.get_client_id();
}
