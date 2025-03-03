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
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "types.hpp"
#include "util.hpp"
#include "ip_address.hpp"

using namespace std;
using namespace adns;

static ip_address ip4_localhost("127.0.0.1");
static ip_address ip6_localhost("::1");
static ip_address ip4_any("0.0.0.0");
static ip_address ip6_any("::");

ip_address::~ip_address()
{
}

ip_address::ip_address()
{
    m_type = none_e;
    memset(&m_in6_addr, 0, sizeof(struct in6_addr));
    memset(&m_in_addr, 0, sizeof(struct in_addr));
}

ip_address::ip_address(ip_type_t t)
{
    m_type = t;
    memset(&m_in6_addr, 0, sizeof(struct in6_addr));
    memset(&m_in_addr, 0, sizeof(struct in_addr));

    m_in_addr.s_addr = htonl(INADDR_ANY);
    m_in6_addr = in6addr_any;
}

bool ip_address::is_localhost() const
{
    if (m_type == ip_v4_e)
    {
        return *this == ip4_localhost;
    }
    else
    {
        return *this == ip6_localhost;
    }
}

void ip_address::init(const string &s)
{
    memset(&m_in6_addr, 0, sizeof(struct in6_addr));
    memset(&m_in_addr, 0, sizeof(struct in_addr));

    if (s == "")
    {
        m_type = ip_v6_e;
    }
    else
    {
        if (inet_pton(AF_INET6, s.c_str(), &m_in6_addr) > 0)
        {
            m_type = ip_v6_e;
            return;
        }

        if (inet_pton(AF_INET, s.c_str(), &m_in_addr) > 0)
        {
            m_type = ip_v4_e;
            return;
        }

        THROW(ip_address_exception, "invalid ip address string", s);
    }
}

ip_address::ip_address(const string &s)
{
    init(s);
}

ip_address::ip_type_t ip_address::get_type() const
{
    return m_type;
}

ip_address::ip_address(struct in_addr a)
{
    m_type = ip_v4_e;
    m_in_addr.s_addr = a.s_addr;
}

ip_address::ip_address(struct in6_addr a)
{
    m_type = ip_v6_e;
    m_in6_addr = a;
}

string ip_address::to_string() const
{
    char buf[100];
    const char *res = nullptr;

    if (m_type == ip_v4_e)
    {
        res = inet_ntop(AF_INET, &m_in_addr, buf, 100);
    }
    else if (m_type == ip_v6_e)
    {
        res = inet_ntop(AF_INET6, &m_in6_addr, buf, 100);
    }
    else
    {
        res = "<NULL>";
        //THROW(ip_address_exception, "to_string called on unitialised instance");
    }

    if (res == nullptr)
    {
        THROW(ip_address_exception, "inet_ntop failed", util::strerror(), errno);
    }
    else
    {
        return buf;
    }
}

bool ip_address::operator==(const ip_address &other) const
{
    if (m_type == other.m_type)
    {
        if (m_type == ip_v4_e)
        {
            return 
                (reinterpret_cast<const uint *>(&m_in_addr))[0] == 
                (reinterpret_cast<const uint *>(&other.m_in_addr))[0];
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                if ((reinterpret_cast<const uint *>(&m_in6_addr))[i] != 
                    (reinterpret_cast<const uint *>(&other.m_in6_addr))[i])
                {
                    return false;
                }
            }
            return true;
        }
    }
    else
    {
        return false;
    }
}

bool ip_address::operator<(const ip_address &other) const
{
    if (m_type == other.m_type)
    {
        if (m_type == ip_v4_e)
        {
            return 
                (reinterpret_cast<const uint *>(&m_in_addr))[0] <
                (reinterpret_cast<const uint *>(&other.m_in_addr))[0];
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                if ((reinterpret_cast<const uint *>(&m_in6_addr))[i] <
                    (reinterpret_cast<const uint *>(&other.m_in6_addr))[i])
                {
                    return true;
                }
                else if ((reinterpret_cast<const uint *>(&m_in6_addr))[i] >
                         (reinterpret_cast<const uint *>(&other.m_in6_addr))[i])
                {
                    return false;
                }
            }
            return true;
        }
    }
    else 
    {
        return m_type == ip_v4_e;
    }
}


bool ip_address::operator!=(const ip_address &other) const
{
    return !(*this == other);
}

void ip_address::json_serialize() const
{
    set_json("address", to_string());
}

void ip_address::json_unserialize()
{
    init(*m_json_object);
}

size_t ip_address::get_hash() const
{
    if (m_type == ip_v4_e)
    {
        return util::hash_by_byte(&m_in_addr, sizeof(m_in_addr));
    }
    else
    {
        return util::hash_by_byte(&m_in6_addr, sizeof(m_in6_addr));
    }
}

ostream &adns::operator<<(ostream& stream, const ip_address &addr)
{
    stream << addr.to_string();
    return stream;
}

bool ip_address::is_any() const
{
    if (m_type == ip_v4_e)
    {
        return *this == ip4_any;
    }
    else
    {
        return *this == ip6_any;
    }
}

