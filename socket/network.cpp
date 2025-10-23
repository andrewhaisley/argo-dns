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
 
#include <string.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "network.hpp"
#include "util.hpp"

using namespace std;
using namespace adns;

uint network::o_bitmask[32];

network::network()
{
    m_count = 0;
}

network::network(const string &cidr_string)
{
    init(cidr_string);
}

void network::init(const string &cidr_string)
{
    vector<string> bits;
    split(bits, cidr_string, boost::is_any_of("/"));

    if (bits.size() != 2)
    {
        THROW(network_exception, "cidr network string format invalid", cidr_string);
    }

    m_ip = ip_address(bits[0]);
    try
    {
        m_count = boost::lexical_cast<int>(bits[1]);
    }
    catch (boost::bad_lexical_cast&)
    {
        THROW(network_exception, "cidr network string format invalid", cidr_string);
    }

    validate();
}

network::network(const ip_address &ip, uint count) : m_ip(ip), m_count(count)
{
    validate();
}

void network::validate()
{
    if (m_ip.get_type() == ip_address::ip_v4_e)
    {
        if (m_count > 31)
        {
            m_count = 0;
            THROW(network_exception, "count for ip4 address is greater than 31", m_count);
        }
    }
    else
    {
        if (m_count > 127)
        {
            m_count = 0;
            THROW(network_exception, "count for ip6 address is greater than 127", m_count);
        }
    }
}

network::~network()
{
}

bool network::contains(const ip_address &ip) const
{
    if (m_ip.get_type() == ip.get_type())
    {
        if (m_ip.get_type() == ip_address::ip_v4_e)
        {
            return contains(m_ip.m_in_addr, m_count, ip.m_in_addr);
        }
        else
        {
            return contains(m_ip.m_in6_addr, m_count, ip.m_in6_addr);
        }
    }
    else
    {
        THROW(network_exception, "mismatched IP address types in call to contains()");
    }
}

bool network::contains(const struct in_addr &network, uint count, const struct in_addr &ip) const
{
    uint n;
    uint a;

    memcpy(&n, &network, sizeof(n));
    memcpy(&a, &ip, sizeof(a));

    return (n & o_bitmask[count]) == (a & o_bitmask[count]);
}

void network::init()
{
    o_bitmask[0] = 0;

    uint bit = 1;

    for (int i = 1; i < 32; i++)
    {
        o_bitmask[i] = bit;
        bit <<= 1;
        bit |= 1;
    }
}

bool network::contains(const struct in6_addr &network, uint count, const struct in6_addr &ip) const
{
    uint n[4];
    uint a[4];

    memcpy(n, &network, sizeof(n));
    memcpy(a, &ip, sizeof(a));

    uint x;

    for (x = 0; x < count / 8; x++)
    {
        if (n[x] != a[x])
        {
            return false;
        }
    }

    return (n[x] & o_bitmask[count % 8]) == (a[x] & o_bitmask[count % 8]);
}

ip_address::ip_type_t network::get_type() const
{
    return m_ip.get_type();
}

void network::json_serialize() const
{
    set_json("address", to_string());
}

void network::json_unserialize()
{
    init((*m_json_object)["address"]);
}

string network::to_string() const
{
    return m_ip.to_string() + "/" + boost::lexical_cast<string>(m_count);
}

ostream &adns::operator<<(ostream& stream, const network &n)
{
    stream << n.to_string();
    return stream;
}

