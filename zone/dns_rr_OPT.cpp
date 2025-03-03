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
#include "types.hpp"
#include "dns_rr_OPT.hpp"
#include "dns_message_serial.hpp"

using namespace adns;
using namespace std;

dns_rr_OPT::dns_rr_OPT() : dns_rr()
{
    set_type(T_OPT_e);
}

dns_rr_OPT::~dns_rr_OPT()
{
}

dns_rr *dns_rr_OPT::clone()
{
    return new dns_rr_OPT(*this);
}

void dns_rr_OPT::set_data(const buffer &data)
{
    m_data = data;
}

const buffer &dns_rr_OPT::get_data() const
{
    return m_data;
}

unsigned short dns_rr_OPT::get_payload_size() const
{
    return static_cast<unsigned short>(get_class());
}

void dns_rr_OPT::dump() const
{
    dns_rr::dump();
    LOG(debug) << "dns payload size " << static_cast<uint>(get_class());
}

void dns_rr_OPT::json_serialize() const
{
}

void dns_rr_OPT::json_unserialize()
{
}
