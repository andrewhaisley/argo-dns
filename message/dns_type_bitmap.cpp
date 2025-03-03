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
