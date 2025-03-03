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

#include <boost/lexical_cast.hpp>

#include "types.hpp"
#include "util.hpp"
#include "buffer.hpp"
#include "dns_type_bitmap.hpp"
#include "dns_rr_DHCID.hpp"

using namespace adns;
using namespace std;

dns_rr_DHCID::dns_rr_DHCID() : dns_rr()
{
    set_type(dns_rr::T_DHCID_e);
}

dns_rr_DHCID::~dns_rr_DHCID()
{
}

dns_rr *dns_rr_DHCID::clone()
{
    return new dns_rr_DHCID(*this);
}

void dns_rr_DHCID::json_serialize() const
{
    dns_rr::json_serialize();
    set_json("contents", util::tobase64(get_contents()));

}

void dns_rr_DHCID::json_unserialize() 
{
    m_contents = util::frombase64((*m_json_object)["contents"]);

}

void dns_rr_DHCID::set_contents(const buffer &contents)
{
    m_contents = contents;
}

const buffer &dns_rr_DHCID::get_contents() const
{
    return m_contents;
}


void dns_rr_DHCID::dump() const
{
    LOG(debug) << to_string();
}

string dns_rr_DHCID::to_string() const
{
    return (get_name() ? get_name()->to_string() : string("")) + " " + dns_rr::type_to_string(get_type()) + " " + boost::lexical_cast<string>(get_ttl()) + " " + util::tobase64(get_contents());
}

string dns_rr_DHCID::to_string_no_ttl() const
{
    return (get_name() ? get_name()->to_string() : string("")) + " " + dns_rr::type_to_string(get_type()) + " " + util::tobase64(get_contents());
}
