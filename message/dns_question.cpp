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
#include "dns_question.hpp"

using namespace std;
using namespace adns;

dns_question::dns_question()
{
    m_hash = 0;
}

dns_question::dns_question(const dns_question &other)
{
    *this = other;
}

dns_question::~dns_question()
{
}

dns_question::dns_question(const dns_name &qname, qtype_t qtype, dns_rr::type_t rr_type, qclass_t qclass)
{
    m_qname = qname;
    m_qtype = qtype;
    m_qclass = qclass;
    m_rr_type = rr_type;
    m_hash = 0;
}

const dns_name &dns_question::get_qname() const
{
    return m_qname;
}

dns_question::qtype_t dns_question::get_qtype() const 
{
    return m_qtype;
}

dns_question::qclass_t dns_question::get_qclass() const 
{
    return m_qclass;
}

dns_rr::type_t dns_question::get_rr_type() const
{
    return m_rr_type;
}

void dns_question::set_qtype(qtype_t qt)
{
    m_qtype = qt;
    m_hash = 0;
}

size_t dns_question::get_hash() const
{
    if (m_hash != 0)
    {
        return m_hash;
    }

    m_hash = m_qname.get_hash();
    m_hash ^= m_qtype;
    m_hash ^= (m_qclass << 8);
    m_hash ^= (m_rr_type << 16);

    return m_hash;
}

const dns_question &dns_question::operator=(const dns_question &other)
{
    m_qname = other.m_qname;
    m_qtype = other.m_qtype;
    m_qclass = other.m_qclass;
    m_rr_type = other.m_rr_type;
    m_hash = other.m_hash;

    return *this;
}

bool dns_question::operator==(const dns_question &other) const
{
    return 
        (m_qtype == other.m_qtype) &&
        (m_qclass == other.m_qclass) &&
        (m_rr_type == other.m_rr_type) &&
        (m_qname == other.m_qname);
}

bool dns_question::operator!=(const dns_question &other) const
{
    return 
        (m_qtype != other.m_qtype) ||
        (m_qclass != other.m_qclass) ||
        (m_rr_type != other.m_rr_type) ||
        (m_qname != other.m_qname);
}

bool dns_question::operator<(const dns_question &other) const
{
    if (m_qtype < other.m_qtype)
    {
        return true;
    }
    else if (m_qtype > other.m_qtype)
    {
        return false;
    }
    else if (m_qclass < other.m_qclass)
    {
        return true;
    }
    else if (m_qclass > other.m_qclass)
    {
        return false;
    }
    else if (m_rr_type < other.m_rr_type)
    {
        return true;
    }
    else if (m_rr_type > other.m_rr_type)
    {
        return false;
    }
    else if (m_qname < other.m_qname)
    {
        return true;
    }
    else
    {
        return false;
    }
}

string dns_question::to_string() const
{
    string res;

    res = m_qname.to_string() + ' ';

    switch (m_qtype)
    {
    case QT_RR_e :
        res += dns_rr::type_to_string(m_rr_type); 
        break;
    case QT_IXFR_e :
        res += "IXFR";
        break;
    case QT_AXFR_e :
        res += "AXFR";
        break;
    case QT_ALL_e :
        res += "ANY";
        break;
    default : 
        res += "<INVALID>";
    }

    return res;
}

ostream &adns::operator<<(ostream& stream, const dns_question &q)
{
    stream << q.to_string();
    return stream;
}
