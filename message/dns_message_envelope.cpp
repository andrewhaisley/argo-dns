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
 
#include "types.hpp"
#include "dns_message_envelope.hpp"

using namespace std;
using namespace adns;

dns_message_envelope::dns_message_envelope() : m_channel(0), m_socket(nullptr), m_request(nullptr), m_response(nullptr)
{
}

dns_message_envelope::dns_message_envelope(const dns_message_envelope &other)
{
    m_raw = other.m_raw;
    m_remote_address = other.m_remote_address;
    m_socket = other.m_socket;
    m_channel = other.m_channel;
    m_min_ttl = other.m_min_ttl;

    if (other.m_request != nullptr)
    {
        m_request = new dns_message(*other.m_request);
    }
    else
    {
        m_request = nullptr;
    }

    if (other.m_response != nullptr)
    {
        m_response = new dns_message(*other.m_response);
    }
    else
    {
        m_response = nullptr;
    }
}

dns_message_envelope::dns_message_envelope(
                            const buffer         &raw, 
                            const socket_address &remote_address,
                            udp_socket           *s) :
    m_raw(raw),
    m_remote_address(remote_address),
    m_channel(0),
    m_min_ttl(0),
    m_socket(s),
    m_request(nullptr), 
    m_response(nullptr),
    m_forward_port(0)
{
}

dns_message_envelope::~dns_message_envelope()
{
    delete m_request;
    delete m_response;
}

const buffer &dns_message_envelope::get_raw() const
{
    return m_raw;
}

void dns_message_envelope::set_raw(const buffer &raw) 
{
    m_raw = raw;
}

const socket_address &dns_message_envelope::get_remote_address() const
{
    return m_remote_address;
}

udp_socket *dns_message_envelope::get_socket() const
{
    return m_socket;
}

dns_message *dns_message_envelope::get_request() const
{
    return m_request;
}

dns_message *dns_message_envelope::get_response() const
{
    return m_response;
}

void dns_message_envelope::set_request(dns_message *r)
{
    if (m_request)
    {
        delete m_request;
    }
    m_request = r;
}

void dns_message_envelope::set_response(dns_message *r)
{
    if (m_response)
    {
        delete m_response;
    }
    m_response = r;
}

void dns_message_envelope::set_channel(int channel)
{
    m_channel = channel;
}

int dns_message_envelope::get_channel() const
{
    return m_channel;
}

void dns_message_envelope::set_forwarding(const ip_address &forward_ip, int forward_port)
{
    m_forward_ip = forward_ip;
    m_forward_port = forward_port;
}

const ip_address &dns_message_envelope::get_forwarding_address() const
{
    return m_forward_ip;
}

int dns_message_envelope::get_forwarding_port() const
{
    return m_forward_port;
}

void dns_message_envelope::set_min_ttl(int min_ttl)
{
    m_min_ttl = min_ttl;
}

int dns_message_envelope::get_min_ttl() const
{
    return m_min_ttl;
}
