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
 
#include <boost/algorithm/string.hpp>

#include "types.hpp"
#include "parser.hpp"
#include "dns_type_bitmap.hpp"

using namespace std;
using namespace adns;

dns_type_bitmap::dns_type_bitmap()
{
}

dns_type_bitmap::~dns_type_bitmap()
{
}

dns_type_bitmap::dns_type_bitmap(const string &s)
{
    auto j = parser::parse(s);
    from_json(*j);
}

dns_type_bitmap::dns_type_bitmap(const json &j)
{
    from_json(j);
}

string dns_type_bitmap::to_string() const
{
    string res;

    // debug string
    for (auto i : m_types)
    {
        if (res != "")
        {
            res += " ";
        }
        res += dns_rr::type_to_string(i);
    }

    return res;
}

const set<dns_rr::type_t> &dns_type_bitmap::get_types() const
{
    return m_types;
}

void dns_type_bitmap::add(dns_rr::type_t t)
{
    m_types.insert(t);
}

ostream &adns::operator<<(ostream& stream, const dns_type_bitmap &b)
{
    stream << b.to_string();
    return stream;
}

void dns_type_bitmap::json_serialize() const
{
    m_json_object = make_shared<json>(json::array_e);

    for (auto i : m_types)
    {
        m_json_object->append(dns_rr::type_to_string(i));
    }

}

void dns_type_bitmap::json_unserialize()
{
    if (m_json_object->get_instance_type() != json::array_e)
    {
        THROW(dns_type_bitmap_exception, "JSON is not array type");
    }

    const vector<unique_ptr<json>> &a = m_json_object->get_array();

    for (size_t i = 0; i < a.size(); i++)
    {
        auto t = dns_rr::string_to_type(*a[i]);
    
        if (t == dns_rr::T_NONE_e)
        {
            THROW(dns_type_bitmap_exception, "unsupported record type for bitmap", i);
        }
        
        m_types.insert(t);
    }
}
