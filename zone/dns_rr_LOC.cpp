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
#include "dns_rr_LOC.hpp"

using namespace adns;
using namespace std;

dns_rr_LOC::dns_rr_LOC() : dns_rr()
{
    set_type(dns_rr::T_LOC_e);
}

dns_rr_LOC::~dns_rr_LOC()
{
}

dns_rr *dns_rr_LOC::clone()
{
    return new dns_rr_LOC(*this);
}

void dns_rr_LOC::json_serialize() const
{
    dns_rr::json_serialize();
    set_json("version", int(get_version()));
    set_json("size", int(get_size()));
    set_json("horizontal_precision", int(get_horizontal_precision()));
    set_json("vertical_precision", int(get_vertical_precision()));
    set_json("latitude", int(get_latitude()));
    set_json("longitude", int(get_longitude()));
    set_json("altitude", int(get_altitude()));

}

void dns_rr_LOC::json_unserialize() 
{
    m_version = int((*m_json_object)["version"]);
    m_size = int((*m_json_object)["size"]);
    m_horizontal_precision = int((*m_json_object)["horizontal_precision"]);
    m_vertical_precision = int((*m_json_object)["vertical_precision"]);
    m_latitude = int((*m_json_object)["latitude"]);
    m_longitude = int((*m_json_object)["longitude"]);
    m_altitude = int((*m_json_object)["altitude"]);

}

void dns_rr_LOC::set_version(octet version)
{
    m_version = version;
}
void dns_rr_LOC::set_size(octet size)
{
    m_size = size;
}
void dns_rr_LOC::set_horizontal_precision(octet horizontal_precision)
{
    m_horizontal_precision = horizontal_precision;
}
void dns_rr_LOC::set_vertical_precision(octet vertical_precision)
{
    m_vertical_precision = vertical_precision;
}
void dns_rr_LOC::set_latitude(int latitude)
{
    m_latitude = latitude;
}
void dns_rr_LOC::set_longitude(int longitude)
{
    m_longitude = longitude;
}
void dns_rr_LOC::set_altitude(int altitude)
{
    m_altitude = altitude;
}

octet dns_rr_LOC::get_version() const
{
    return m_version;
}
octet dns_rr_LOC::get_size() const
{
    return m_size;
}
octet dns_rr_LOC::get_horizontal_precision() const
{
    return m_horizontal_precision;
}
octet dns_rr_LOC::get_vertical_precision() const
{
    return m_vertical_precision;
}
int dns_rr_LOC::get_latitude() const
{
    return m_latitude;
}
int dns_rr_LOC::get_longitude() const
{
    return m_longitude;
}
int dns_rr_LOC::get_altitude() const
{
    return m_altitude;
}


void dns_rr_LOC::dump() const
{
    LOG(debug) << to_string();
}

string dns_rr_LOC::to_string() const
{
    return (get_name() ? get_name()->to_string() : string("")) + " " + dns_rr::type_to_string(get_type()) + " " + boost::lexical_cast<string>(get_ttl()) + " " + boost::lexical_cast<string>(int(get_version())) + " " + boost::lexical_cast<string>(int(get_size())) + " " + boost::lexical_cast<string>(int(get_horizontal_precision())) + " " + boost::lexical_cast<string>(int(get_vertical_precision())) + " " + boost::lexical_cast<string>(get_latitude()) + " " + boost::lexical_cast<string>(get_longitude()) + " " + boost::lexical_cast<string>(get_altitude());
}

string dns_rr_LOC::to_string_no_ttl() const
{
    return (get_name() ? get_name()->to_string() : string("")) + " " + dns_rr::type_to_string(get_type()) + " " + boost::lexical_cast<string>(int(get_version())) + " " + boost::lexical_cast<string>(int(get_size())) + " " + boost::lexical_cast<string>(int(get_horizontal_precision())) + " " + boost::lexical_cast<string>(int(get_vertical_precision())) + " " + boost::lexical_cast<string>(get_latitude()) + " " + boost::lexical_cast<string>(get_longitude()) + " " + boost::lexical_cast<string>(get_altitude());
}
