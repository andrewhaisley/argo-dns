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
#include "dns_rr_SSHFP.hpp"

using namespace adns;
using namespace std;

dns_rr_SSHFP::dns_rr_SSHFP() : dns_rr()
{
    set_type(dns_rr::T_SSHFP_e);
}

dns_rr_SSHFP::~dns_rr_SSHFP()
{
}

dns_rr *dns_rr_SSHFP::clone()
{
    return new dns_rr_SSHFP(*this);
}

void dns_rr_SSHFP::json_serialize() const
{
    dns_rr::json_serialize();
    set_json("algorithm", int(get_algorithm()));
    set_json("fp_type", int(get_fp_type()));
    set_json("fingerprint", util::tohex(get_fingerprint()));

}

void dns_rr_SSHFP::json_unserialize() 
{
    m_algorithm = int((*m_json_object)["algorithm"]);
    m_fp_type = int((*m_json_object)["fp_type"]);
    m_fingerprint = util::fromhex((*m_json_object)["fingerprint"]);

}

void dns_rr_SSHFP::set_algorithm(octet algorithm)
{
    m_algorithm = algorithm;
}
void dns_rr_SSHFP::set_fp_type(octet fp_type)
{
    m_fp_type = fp_type;
}
void dns_rr_SSHFP::set_fingerprint(const buffer &fingerprint)
{
    m_fingerprint = fingerprint;
}

octet dns_rr_SSHFP::get_algorithm() const
{
    return m_algorithm;
}
octet dns_rr_SSHFP::get_fp_type() const
{
    return m_fp_type;
}
const buffer &dns_rr_SSHFP::get_fingerprint() const
{
    return m_fingerprint;
}


void dns_rr_SSHFP::dump() const
{
    LOG(debug) << to_string();
}

string dns_rr_SSHFP::to_string() const
{
    return (get_name() ? get_name()->to_string() : string("")) + " " + dns_rr::type_to_string(get_type()) + " " + boost::lexical_cast<string>(get_ttl()) + " " + boost::lexical_cast<string>(int(get_algorithm())) + " " + boost::lexical_cast<string>(int(get_fp_type())) + " " + util::tohex(get_fingerprint());
}

string dns_rr_SSHFP::to_string_no_ttl() const
{
    return (get_name() ? get_name()->to_string() : string("")) + " " + dns_rr::type_to_string(get_type()) + " " + boost::lexical_cast<string>(int(get_algorithm())) + " " + boost::lexical_cast<string>(int(get_fp_type())) + " " + util::tohex(get_fingerprint());
}
