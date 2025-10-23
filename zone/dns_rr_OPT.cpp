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
