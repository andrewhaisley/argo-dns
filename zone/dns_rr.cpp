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
/**
 * automatically generated by rrgen.py at 05:05PM on January 30, 2024 do not edit by hand.
 */

#include <unordered_map>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "types.hpp"
#include "util.hpp"
#include "dns_rr.hpp"

using namespace adns;
using namespace std;

static unordered_map<uint16_t, string> s_type_to_name = 
{
    { dns_rr::T_SOA_e, "SOA"},
    { dns_rr::T_A_e, "A"},
    { dns_rr::T_AAAA_e, "AAAA"},
    { dns_rr::T_AFSDB_e, "AFSDB"},
    { dns_rr::T_NS_e, "NS"},
    { dns_rr::T_MX_e, "MX"},
    { dns_rr::T_CNAME_e, "CNAME"},
    { dns_rr::T_DNAME_e, "DNAME"},
    { dns_rr::T_PTR_e, "PTR"},
    { dns_rr::T_TXT_e, "TXT"},
    { dns_rr::T_NAPTR_e, "NAPTR"},
    { dns_rr::T_KX_e, "KX"},
    { dns_rr::T_OPT_e, "OPT"},
    { dns_rr::T_SRV_e, "SRV"},
    { dns_rr::T_HINFO_e, "HINFO"},
    { dns_rr::T_SVCB_e, "SVCB"},
    { dns_rr::T_IPSECKEY_e, "IPSECKEY"},
    { dns_rr::T_LOC_e, "LOC"},
    { dns_rr::T_HTTPS_e, "HTTPS"},
    { dns_rr::T_CAA_e, "CAA"},
    { dns_rr::T_ZONEMD_e, "ZONEMD"},
    { dns_rr::T_URI_e, "URI"},
    { dns_rr::T_SSHFP_e, "SSHFP"},
    { dns_rr::T_TLSA_e, "TLSA"},
    { dns_rr::T_OPENPGPKEY_e, "OPENPGPKEY"},
    { dns_rr::T_DHCID_e, "DHCID"},
    { dns_rr::T_CERT_e, "CERT"},
    { dns_rr::T_EUI48_e, "EUI48"},
    { dns_rr::T_EUI64_e, "EUI64"},
    { dns_rr::T_CSYNC_e, "CSYNC"},
    { dns_rr::T_TKEY_e, "TKEY"},
    { dns_rr::T_TSIG_e, "TSIG"},
    { dns_rr::T_GENERIC_e, "GENERIC"},

    { dns_rr::T_NONE_e, "NONE" }
};

static unordered_map<string, uint16_t> s_name_to_type =
{
    { "SOA", dns_rr::T_SOA_e},
    { "A", dns_rr::T_A_e},
    { "AAAA", dns_rr::T_AAAA_e},
    { "AFSDB", dns_rr::T_AFSDB_e},
    { "NS", dns_rr::T_NS_e},
    { "MX", dns_rr::T_MX_e},
    { "CNAME", dns_rr::T_CNAME_e},
    { "DNAME", dns_rr::T_DNAME_e},
    { "PTR", dns_rr::T_PTR_e},
    { "TXT", dns_rr::T_TXT_e},
    { "NAPTR", dns_rr::T_NAPTR_e},
    { "KX", dns_rr::T_KX_e},
    { "OPT", dns_rr::T_OPT_e},
    { "SRV", dns_rr::T_SRV_e},
    { "HINFO", dns_rr::T_HINFO_e},
    { "SVCB", dns_rr::T_SVCB_e},
    { "IPSECKEY", dns_rr::T_IPSECKEY_e},
    { "LOC", dns_rr::T_LOC_e},
    { "HTTPS", dns_rr::T_HTTPS_e},
    { "CAA", dns_rr::T_CAA_e},
    { "ZONEMD", dns_rr::T_ZONEMD_e},
    { "URI", dns_rr::T_URI_e},
    { "SSHFP", dns_rr::T_SSHFP_e},
    { "TLSA", dns_rr::T_TLSA_e},
    { "OPENPGPKEY", dns_rr::T_OPENPGPKEY_e},
    { "DHCID", dns_rr::T_DHCID_e},
    { "CERT", dns_rr::T_CERT_e},
    { "EUI48", dns_rr::T_EUI48_e},
    { "EUI64", dns_rr::T_EUI64_e},
    { "CSYNC", dns_rr::T_CSYNC_e},
    { "TKEY", dns_rr::T_TKEY_e},
    { "TSIG", dns_rr::T_TSIG_e},
    { "GENERIC", dns_rr::T_GENERIC_e},

    { "NONE", dns_rr::T_NONE_e }
};

dns_rr::dns_rr() : m_class(C_IN_e), m_type(T_NONE_e), m_ttl(0), m_id(boost::uuids::nil_generator()()), m_zone_id(boost::uuids::nil_generator()())
{
}

dns_rr::~dns_rr()
{
}

void dns_rr::set_name(const shared_ptr<dns_name> &n)
{
    m_name = n;
}

void dns_rr::set_type(type_t t)
{
    m_type = t;
}

void dns_rr::set_ttl(uint ttl)
{
    m_ttl = ttl;
}

void dns_rr::set_id(uuid id)
{
    m_id = id;
}

void dns_rr::set_zone_id(uuid zone_id)
{
    m_zone_id = zone_id;
}

void dns_rr::set_class(dns_rr::class_t c)
{
    m_class = c;
}

bool dns_rr::operator==(const dns_rr &other) const
{
    return (m_type == other.m_type) && (m_id == other.m_id);
}

void dns_rr::dump() const
{
    LOG(debug) << *this;
}

string dns_rr::type_to_string(type_t t)
{
    auto i = s_type_to_name.find(t);
    if (i == s_type_to_name.end())
    {
        return "<GENERIC(" + boost::lexical_cast<string>(t) + ")>";
    }
    else
    {
        return i->second;
    }
}

dns_rr::type_t dns_rr::string_to_type(const string &s)
{
    auto i = s_name_to_type.find(s);
    if (i == s_name_to_type.end())
    {
        return T_NONE_e;
    }
    else
    {
        return static_cast<type_t>(i->second);
    }
}

const shared_ptr<dns_name> &dns_rr::get_name() const
{
    return m_name;
}

dns_rr::type_t dns_rr::get_type() const
{
    return m_type;
}

uint dns_rr::get_ttl() const
{
    return m_ttl;
}

void dns_rr::update_ttl(int diff)
{
    m_ttl += diff;
}

dns_rr::class_t dns_rr::get_class() const
{
    return m_class;
}

uuid dns_rr::get_zone_id() const
{
    return m_zone_id;
}

uuid dns_rr::get_id() const
{
    return m_id;
}

void dns_rr::json_serialize() const
{
    set_json("zone_id", boost::lexical_cast<string>(m_zone_id));
    set_json("rr_id", boost::lexical_cast<string>(m_id));
    set_json("name", m_name->to_string());
    set_json("type", type_to_string(m_type));
    set_json("ttl", int(m_ttl));
}

string dns_rr::to_string() const
{
    return (get_name() ? get_name()->to_string() : string("")) + " " + dns_rr::type_to_string(get_type()) + " " + boost::lexical_cast<string>(get_ttl());
}

string dns_rr::to_string_no_ttl() const
{
    return (get_name() ? get_name()->to_string() : string("")) + " " + dns_rr::type_to_string(get_type());
}

ostream &adns::operator<<(ostream& stream, const dns_rr &rr)
{
    return stream << rr.to_string();
}
