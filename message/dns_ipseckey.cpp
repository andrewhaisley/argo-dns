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

#include "types.hpp"
#include "parser.hpp"
#include "util.hpp"
#include "dns_ipseckey.hpp"

using namespace std;
using namespace adns;

dns_ipseckey::dns_ipseckey() :
        m_gateway_type(dns_ipseckey::none_e),
        m_precedence(0),
        m_algorithm(0)
{
}

dns_ipseckey::~dns_ipseckey()
{
}

dns_ipseckey::dns_ipseckey(const string &s)
{
    auto j = parser::parse(s);
    from_json(*j);
}

dns_ipseckey::dns_ipseckey(const json &j)
{
    from_json(j);
}

string dns_ipseckey::to_string() const
{
    string res;

    // debug string
    res += boost::lexical_cast<string>(m_precedence);
    res += " ";
    switch (m_gateway_type)
    {
    case ipv4_e :
            res += "ipv4";
            break;
    case ipv6_e :
            res += "ipv6";
            break;
    case name_e :
            res += "name";
            break;
    default:
    case none_e :
            res += "none";
            break;
    }
    res += " ";
    res += boost::lexical_cast<string>(m_algorithm);
    res += " ";

    switch (m_gateway_type)
    {
    case ipv4_e :
            res += m_ipv4_gateway.to_string();
            break;
    case ipv6_e :
            res += m_ipv6_gateway.to_string();
            break;
    case name_e :
            res += m_name_gateway.to_string();
            break;
    default:
    case none_e :
            res += "<none>";
            break;
    }
    res += " ";
    res += util::tobase64(m_public_key);

    return res;
}

ostream &adns::operator<<(ostream& stream, const dns_ipseckey &b)
{
    stream << b.to_string();
    return stream;
}

void dns_ipseckey::json_serialize() const
{
    (*m_json_object)["precedence"] = m_precedence;
    (*m_json_object)["algorithm"] = m_algorithm;
    (*m_json_object)["gateway_type"] = m_gateway_type;

    switch (m_gateway_type)
    {
    case ipv4_e :
            (*m_json_object)["ipv4_gateway"] = m_ipv4_gateway.to_json();
            break;
    case ipv6_e :
            (*m_json_object)["ipv6_gateway"] = m_ipv6_gateway.to_json();
            break;
    case name_e :
            (*m_json_object)["name_gateway"] = m_name_gateway.to_string();
            break;
    default:
    case none_e :
            break;
    }
    (*m_json_object)["public_key"] = util::tobase64(m_public_key);
}

void dns_ipseckey::json_unserialize()
{
    m_precedence = int((*m_json_object)["precedence"]);
    m_algorithm = int((*m_json_object)["algorithm"]);
    m_gateway_type = gateway_type_t(int((*m_json_object)["gateway_type"]));
    switch (m_gateway_type)
    {
    case ipv4_e :
            m_ipv4_gateway = ip_address((*m_json_object)["ipv4_gateway"]);
            break;
    case ipv6_e :
            m_ipv6_gateway = ip_address((*m_json_object)["ipv6_gateway"]);
            break;
    case name_e :
            m_name_gateway = dns_name((*m_json_object)["name_gateway"]);
            break;
    default:
    case none_e :
            break;
    }
    m_public_key = util::frombase64((*m_json_object)["public_key"]);
}
