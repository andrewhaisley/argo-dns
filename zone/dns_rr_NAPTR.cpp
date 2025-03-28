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
#include "dns_rr_NAPTR.hpp"

using namespace adns;
using namespace std;

dns_rr_NAPTR::dns_rr_NAPTR() : dns_rr()
{
    set_type(dns_rr::T_NAPTR_e);
}

dns_rr_NAPTR::~dns_rr_NAPTR()
{
}

dns_rr *dns_rr_NAPTR::clone()
{
    return new dns_rr_NAPTR(*this);
}

void dns_rr_NAPTR::json_serialize() const
{
    dns_rr::json_serialize();
    set_json("naptr_order", int(get_naptr_order()));
    set_json("preference", int(get_preference()));
    set_json("flags", get_flags()->to_string());
    set_json("services", get_services()->to_string());
    set_json("naptr_regexp", get_naptr_regexp()->to_string());
    set_json("replacement", get_replacement()->to_string());

}

void dns_rr_NAPTR::json_unserialize() 
{
    m_naptr_order = int((*m_json_object)["naptr_order"]);
    m_preference = int((*m_json_object)["preference"]);
    m_flags = make_shared<dns_label>((*m_json_object)["flags"]);
    m_services = make_shared<dns_label>((*m_json_object)["services"]);
    m_naptr_regexp = make_shared<dns_label>((*m_json_object)["naptr_regexp"]);
    m_replacement = make_shared<dns_name>((*m_json_object)["replacement"]);

}

void dns_rr_NAPTR::set_naptr_order(unsigned short naptr_order)
{
    m_naptr_order = naptr_order;
}
void dns_rr_NAPTR::set_preference(unsigned short preference)
{
    m_preference = preference;
}
void dns_rr_NAPTR::set_flags(const std::shared_ptr<dns_label> &flags)
{
    m_flags = flags;
}
void dns_rr_NAPTR::set_services(const std::shared_ptr<dns_label> &services)
{
    m_services = services;
}
void dns_rr_NAPTR::set_naptr_regexp(const std::shared_ptr<dns_label> &naptr_regexp)
{
    m_naptr_regexp = naptr_regexp;
}
void dns_rr_NAPTR::set_replacement(const std::shared_ptr<dns_name> &replacement)
{
    m_replacement = replacement;
}

unsigned short dns_rr_NAPTR::get_naptr_order() const
{
    return m_naptr_order;
}
unsigned short dns_rr_NAPTR::get_preference() const
{
    return m_preference;
}
const std::shared_ptr<dns_label> &dns_rr_NAPTR::get_flags() const
{
    return m_flags;
}
const std::shared_ptr<dns_label> &dns_rr_NAPTR::get_services() const
{
    return m_services;
}
const std::shared_ptr<dns_label> &dns_rr_NAPTR::get_naptr_regexp() const
{
    return m_naptr_regexp;
}
const std::shared_ptr<dns_name> &dns_rr_NAPTR::get_replacement() const
{
    return m_replacement;
}


void dns_rr_NAPTR::dump() const
{
    LOG(debug) << to_string();
}

string dns_rr_NAPTR::to_string() const
{
    return (get_name() ? get_name()->to_string() : string("")) + " " + dns_rr::type_to_string(get_type()) + " " + boost::lexical_cast<string>(get_ttl()) + " " + boost::lexical_cast<string>(get_naptr_order()) + " " + boost::lexical_cast<string>(get_preference()) + " " + get_flags()->to_string() + " " + get_services()->to_string() + " " + get_naptr_regexp()->to_string() + " " + get_replacement()->to_string();
}

string dns_rr_NAPTR::to_string_no_ttl() const
{
    return (get_name() ? get_name()->to_string() : string("")) + " " + dns_rr::type_to_string(get_type()) + " " + boost::lexical_cast<string>(get_naptr_order()) + " " + boost::lexical_cast<string>(get_preference()) + " " + get_flags()->to_string() + " " + get_services()->to_string() + " " + get_naptr_regexp()->to_string() + " " + get_replacement()->to_string();
}
