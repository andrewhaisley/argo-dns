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
 
#include <chrono> 
#include <thread>
#include <random>
#include <mutex>
#include <map>
#include <algorithm>

#include "config.hpp"
#include "socket_set.hpp"
#include "run_state.hpp"
#include "dns_message_parser.hpp"
#include "dns_parallel_client.hpp"
#include "dns_ip_selector.hpp"

using namespace std;
using namespace chrono;
using namespace adns;
using namespace boost::log::trivial;

static random_device rd;

dns_parallel_client::dns_parallel_client(const client_config &config) : m_config(config)
{
    if (m_config.use_udp)
    {
        reset_sockets();
    }
}

dns_parallel_client::~dns_parallel_client()
{
}

void dns_parallel_client::set_ip_address(const ip_address &ip)
{
    m_all_ips.clear();
    m_all_ips.insert(ip);
}

dns_message *dns_parallel_client::query(dns_message &request)
{
    LOG(debug) << "dns_parallel_client::query";

    unique_ptr<dns_message> res;

    m_total_sent = 0;
    m_received_matching_query = 0;
    m_query_start_time = steady_clock::now();

    if (m_config.use_udp)
    {
        LOG(debug) << "dns_parallel_client::query HERE 1";
        res = unique_ptr<dns_message>(query_udp(request));
        LOG(debug) << "dns_parallel_client::query HERE 2";
        check_timeout();
        LOG(debug) << "dns_parallel_client::query HERE 3";
    }

    if (m_config.use_tcp)
    {
        if (!res)
        {
            for (auto i : m_ips)
            {
                dns_ip_selector::mark_dead(i);
            }
            res = unique_ptr<dns_message>(query_tcp(request));
        }
    }

    return res.release();
}


bool dns_parallel_client::send_udp(dns_message &request, bool use_edns, set<unsigned short int> &ids)
{
    LOG(debug) << "dns_parallel_client::send_udp";

    dns_message_parser p;

    if (use_edns && (config::edns_size() > STANDARD_MESSAGE_SIZE))
    {
        request.set_edns(true, config::edns_size());
    }
    else
    {
        request.set_edns(false, STANDARD_MESSAGE_SIZE);
    }

    buffer raw = p.to_wire(request);

    if (raw.get_size() > config::edns_size())
    {
        // unclear why this would ever happen but, if it does, then fall back to TCP
        return false;
    }
    else
    {
        LOG(debug) << "selecting ips";
        dns_ip_selector::select_ips(m_all_ips, m_ips, false, m_config.num_parallel_udp);

        for (auto &ip : m_ips)
        {
            LOG(debug) << "trying ip " << ip;
            check_timeout();

            unsigned short int id = rd() & 0xffff;
            p.set_id(raw, id);
            ids.insert(id);

            socket_address sa(ip, m_config.server_port);

            dns_ip_selector::update_last_contact(ip);
            m_ip4_socket->send(raw, sa);
            m_total_sent++;
        }

        return true;
    }
}

dns_message *dns_parallel_client::query_udp(dns_message &request)
{
    set<unsigned short int> ids;
    bool format_error = false;

    LOG(debug) << "dns_parallel_client::query_udp";

    // try with EDNS first (second param)
    if (!send_udp(request, true, ids)) 
    {
        return nullptr;
    }

    unique_ptr<dns_message> res(receive_udp(request, ids, format_error));

    // format error most likely means EDNS isn't supported, so
    // try without 
    if (format_error)
    {
        ids.clear();
        if (send_udp(request, false, ids)) 
        {
            return receive_udp(request, ids, format_error);
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        return res.release();
    }
}

dns_message *dns_parallel_client::receive_udp(dns_message &request, set<unsigned short int> &ids, bool &format_error)
{
    socket_set ss;

    if (m_ip4_socket)
    {
        ss.add(m_ip4_socket);
    }
    if (m_ip6_socket)
    {
        ss.add(m_ip6_socket);
    }

    udp_socket::message *messages[20];
    auto start_time = steady_clock::now();

    // keep going for up to configued UDP timeout within this method
    while (duration_cast<milliseconds>(steady_clock::now() - start_time).count() < m_config.udp_timeout_ms)
    {
        // check the instance global timeout
        check_timeout();

        if (run_state::o_state == run_state::shutdown_e)
        {
            break;
        }

        try
        {
            auto e = ss.wait_one(m_config.wait_udp_response_ms);

            if (get<1>(e) == socket_set::ready_read_e)
            {
                int n = static_cast<udp_socket *>(get<0>(e).get())->receive_many(20, messages);
                auto res = extract_matching_response(messages, n, ids, request.get_question()->get_qname(), format_error);

                for (int i = 0; i < n; i++)
                {
                    delete messages[i];
                }

                if (res)
                {
                    return res;
                }
            }
        }
        catch (socket_set_timeout_exception&)
        {
        }
        catch (udp_socket_timeout_exception&)
        {
        }
    }

    return nullptr;
}

dns_message *dns_parallel_client::extract_matching_response(
                        udp_socket::message     *messages[], 
                        int                     n, 
                        set<unsigned short int> ids,
                        const dns_name          &qname,
                        bool                    &format_error)
{
    dns_message_parser p;

    format_error = false;

    for (int i = 0; i < n; i++)
    {
        auto &raw_response = messages[i]->get_message();
        const socket_address &receive_remote_address = messages[i]->get_remote_address();

        try
        {
            unique_ptr<dns_message> m(p.from_wire(raw_response));

            if (m->get_response_code() == dns_message::no_error_e)
            {
                if ((m_ips.find(receive_remote_address.get_ip_address()) != m_ips.end()) &&
                    (ids.find(m->get_id()) != ids.end()) &&
                    (m->get_question()->get_qname() == qname))
                {
                    return m.release();
                }
            }
            else if (m->get_response_code() == dns_message::format_error_e)
            {
                format_error = true;
            }
        }
        catch (adns::message_parser_exception &e)  
        {
            // badly formatted message, ignore and continue
            format_error = true;
        }
    }

    return nullptr;
}

dns_message *dns_parallel_client::query_tcp(dns_message &request)
{
    dns_ip_selector::select_ips(m_all_ips, m_ips, true, m_all_ips.size());

    for (auto ip : m_ips)
    {
        check_timeout();

        if (run_state::o_state == run_state::shutdown_e)
        {
            return nullptr;
        }

        shared_ptr<tcp_client_socket> socket;

        if (ip.get_type() == ip_address::ip_v4_e)
        {
            socket.reset(new tcp_client_socket(ip_address::ip_v4_e));
        }
        else
        {
            socket.reset(new tcp_client_socket(ip_address::ip_v6_e));
        }

        try
        {
            socket_address sa(ip, m_config.server_port);
            socket->start_connect(sa);

            socket_set ss;
            ss.add(socket);

            try 
            {
                auto e = ss.wait_one(m_config.connect_tcp_timeout_ms);
                check_timeout();

                dns_ip_selector::update_last_contact(ip);
                send_request_by_tcp(*socket, request);
                check_timeout();
                auto response = read_response_by_tcp(*socket);
                check_timeout();

                if (response->get_id() != request.get_id())
                {
                    LOG(warning) << "got a response with the wrong message id, possible cache poisoning attempt";
                }
                else
                {
                    return response;
                }
            }
            catch (socket_set_timeout_exception &e)
            {
                continue;
            }
        }
        catch (message_serial_overrun_exception &e)
        {
            e.log(warning, "response message overrun whilst parsing");
        }
        catch (adns::exception &e)
        {
            e.log(warning, "exception encountered using tcp connection");
        }
    }

    return nullptr;
}

void dns_parallel_client::send_request_by_tcp(tcp_socket &socket, dns_message &request)
{
    dns_message_parser p;

    request.set_id(rd() & 0xffff);
    request.set_edns(false, STANDARD_MESSAGE_SIZE);
    buffer raw = p.to_wire(request);

    if (raw.get_size() > 65534)
    {
        THROW(parallel_client_exception, "request size bigger than max tcp message size ", raw.get_size());
    }

    unsigned short s = htons(raw.get_size());
    socket.write(reinterpret_cast<octet *>(&s), sizeof(s), m_config.write_tcp_timeout_ms);
    socket.write(raw, m_config.write_tcp_timeout_ms);
}

dns_message *dns_parallel_client::read_response_by_tcp(tcp_socket &socket)
{
    dns_message_parser p;

    unsigned short s;
    auto sb = socket.read(sizeof(s), m_config.read_tcp_timeout_ms);
    memcpy(&s, sb.get_data(), sizeof(s));
    s = ntohs(s);
    check_timeout();
    auto raw = socket.read(s, m_config.read_tcp_timeout_ms);
    check_timeout();

    return p.from_wire(raw);
}

void dns_parallel_client::check_timeout() const
{
    if (m_config.total_timeout_ms > 0)
    {
        duration<double> elapsed = duration_cast<milliseconds>(steady_clock::now() - m_query_start_time);

        if (elapsed.count() > m_config.total_timeout_ms)
        {
            THROW(parallel_client_exception, "timeout");
        }
    }
}

void dns_parallel_client::set_ip_addresses(const std::set<ip_address> &ips)
{
    m_all_ips = ips;
}

void dns_parallel_client::reset_sockets()
{
    if (m_config.use_ip4)
    {
        m_ip4_socket.reset(new udp_socket(ip_address::ip_v4_e));
    }
    if (m_config.use_ip6)
    {
        m_ip6_socket.reset(new udp_socket(ip_address::ip_v6_e));
    }
}

void dns_parallel_client::close_sockets()
{
    if (m_ip4_socket)
    {
        m_ip4_socket = nullptr;
    }
    if (m_ip6_socket)
    {
        m_ip6_socket = nullptr;
    }
}
