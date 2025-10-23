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
 
#include <math.h>
#include <random>

#include "config.hpp"
#include "run_state.hpp"
#include "dns_serial_client.hpp"
#include "udp_socket.hpp"
#include "tcp_client_socket.hpp"
#include "dns_message_parser.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

static random_device rd;

#define CLIENT_LOG(x) LOG(x) << "dns client connecting to " << m_remote_addr << ":"

dns_serial_client::dns_serial_client(const socket_address &addr) : m_remote_addr(addr)
{
}

dns_serial_client::~dns_serial_client()
{
}

shared_ptr<dns_message> dns_serial_client::send(
                    dns_message &request,
                    bool        try_udp,
                    bool        try_tcp,
                    uint        num_retries,
                    uint        response_timeout)
{
    shared_ptr<dns_message> res;

    if (try_udp)
    {
        res = send_by_udp(request, num_retries, response_timeout);
    }

    if (res)
    {
        return res;
    }

    if (try_tcp)
    {
        res = send_by_tcp(request, num_retries, response_timeout);
    }

    return res;
}

shared_ptr<dns_message> dns_serial_client::send_by_udp(
                    dns_message &request,
                    uint        num_retries,
                    uint        response_timeout)
{
    uint packet_size = config::edns_size();
    uint i = 0;

    while (i < num_retries)
    {
        if (run_state::o_state == run_state::shutdown_e)
        {
            break;
        }

        try
        {
            auto r = send_by_udp_once(request, packet_size, response_timeout);
            if (r == nullptr)
            {
                LOG(info) << "got null message";
                if (i < (num_retries - 1))
                {
                    sleep(1);
                }
                i++;
            }
            else
            {
                LOG(info) << "got message";
                return r;
            }
        }
        catch (dns_serial_client_truncated_reply_exception &e)
        {
            if (packet_size <= STANDARD_MESSAGE_SIZE)
            {
                // all edns options exhausted, return nullptr and leave caller to try tcp if they want to
                return nullptr;
            }
            else
            {
                packet_size /= 2;
            }
        }
        catch (adns::exception &e)
        {
            e.log(warning, "unexpected exception");
            CLIENT_LOG(warning) << "unexpected exception whilst sending via UDP";
            return nullptr;
        }
    }

    return nullptr;
}

shared_ptr<dns_message> dns_serial_client::send_by_udp_once(dns_message &request, uint packet_size, uint response_timeout)
{
    dns_message_parser p;

    request.set_id(rd() & 0xffff);
    buffer raw = p.to_wire(request);

    if (raw.get_size() > packet_size)
    {
        CLIENT_LOG(error) << "request size bigger than available packet size " << raw.get_size() << " " << packet_size;
        return nullptr;
    }

    if (packet_size > STANDARD_MESSAGE_SIZE)
    {
        request.set_edns(true, packet_size);
    }
    else
    {
        request.set_edns(false, STANDARD_MESSAGE_SIZE);
    }

    LOG(info) << "sending query";
    m_udp_socket.send(raw, m_remote_addr);

    auto response = get_matching_response(request.get_id(), response_timeout);

    if (response->get_response_code() == dns_message::no_error_e)
    {
        if (response->get_is_truncated())
        {
            THROW(dns_serial_client_truncated_reply_exception, "truncated reply received");
            return nullptr;
        }
        else
        {
            return response;
        }
    }
    else
    {
        CLIENT_LOG(warning) << "error returned from server" << response->get_response_code();
        return nullptr;
    }
}

shared_ptr<dns_message> dns_serial_client::get_matching_response(uint msg_id, uint response_timeout)
{
    dns_message_parser p;

    buffer raw_response;
    socket_address receive_remote_address;

    while (true)
    {
        raw_response = m_udp_socket.receive(receive_remote_address, response_timeout);
        LOG(info) << "received response";

        if (receive_remote_address == m_remote_addr)
        {
            LOG(info) << "right address";
            if (p.extract_id(raw_response) == msg_id)
            {
                LOG(info) << "right message ID";
                break;
            }
            else
            {
                LOG(info) << "wrong ID";
                CLIENT_LOG(warning) << "got a response with the wrong message id";
            }
        }
        else
        {
            LOG(info) << "wrong address";
            CLIENT_LOG(warning) << "got a response from an address other than the one a message was sent to" << receive_remote_address;
        }
    }

    try
    {
        return shared_ptr<dns_message>(p.from_wire(raw_response));
    }
    catch (message_serial_overrun_exception &e)
    {
        THROW(dns_serial_client_truncated_reply_exception, "response message overrun whilst parsing");
    }
}

shared_ptr<dns_message> dns_serial_client::send_by_tcp(
                    dns_message &request,
                    uint        num_retries,
                    uint        response_timeout)
{
    for (uint i = 0; i < num_retries; i++)
    {
        if (run_state::o_state == run_state::shutdown_e)
        {
            break;
        }

        try
        {
            auto res = send_by_tcp_once(request, response_timeout);

            if (res)
            {
                return res;
            }
        }
        catch (adns::exception &e)
        {
            e.log(warning, "unexpected exception");
            CLIENT_LOG(warning) << "unexpected exception whilst sending via TCP";
        }

        if (i < (num_retries - 1))
        {
            sleep(1);
        }
    }

    return nullptr;
}

void dns_serial_client::send_request_by_tcp(dns_message &request)
{
    dns_message_parser p;

    request.set_id(rd() & 0xffff);
    request.set_edns(false, STANDARD_MESSAGE_SIZE);
    buffer raw = p.to_wire(request);

    if (raw.get_size() > 65534)
    {
        THROW(dns_serial_client_exception, "request size bigger than max tcp message size ", raw.get_size());
    }

    unsigned short s = htons(raw.get_size());
    m_tcp_socket.write(reinterpret_cast<octet *>(&s), 2, 5);
    m_tcp_socket.write(raw, 5);
}

shared_ptr<dns_message> dns_serial_client::send_by_tcp_once(dns_message &request, uint response_timeout)
{
    try
    {
        m_tcp_socket.connect(m_remote_addr);
        send_request_by_tcp(request);

        auto response = read_response_by_tcp(response_timeout);

        m_tcp_socket.shutdown();

        if (response->get_id() != request.get_id())
        {
            CLIENT_LOG(warning) << "got a response with the wrong message id, possible cache poisining attempt";
            return nullptr;
        }
        else
        {
            return response;
        }
    }
    catch (message_serial_overrun_exception &e)
    {
        m_tcp_socket.shutdown();
        THROW(dns_serial_client_truncated_reply_exception, "response message overrun whilst parsing");
    }
    catch (adns::exception &e)
    {
        m_tcp_socket.shutdown();
        e.log(warning, "exception encountered using tcp connection");
        CLIENT_LOG(warning) << "unexpected exception";
        return nullptr;
    }
}

shared_ptr<dns_message> dns_serial_client::read_response_by_tcp(uint response_timeout)
{
    dns_message_parser p;

    auto sb = m_tcp_socket.read(2, response_timeout);
    unsigned short s;
    memcpy(&s, sb.get_data(), 2);
    s = ntohs(s);
    auto raw = m_tcp_socket.read(s, response_timeout);

    return shared_ptr<dns_message>(p.from_wire(raw));
}
