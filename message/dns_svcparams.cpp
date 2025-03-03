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
#include <boost/lexical_cast.hpp> 

#include "util.hpp"
#include "parser.hpp"
#include "unparser.hpp"
#include "dns_message.hpp"
#include "dns_svcparams.hpp"


using namespace std;
using namespace adns;

dns_svcparams::dns_svcparams()
{
}

dns_svcparams::dns_svcparams(const std::string &s)
{
    auto j = parser::parse(s);
    from_json(*j);
}

dns_svcparams::dns_svcparams(const json &j)
{
    from_json(j);
}

dns_svcparams::~dns_svcparams()
{
}

string dns_svcparams::to_string() const
{
    string res;

    // debug string
    for (auto i : m_params)
    {
        if (res != "")
        {
            res += " ";
        }
        res += i.second.to_string();
    }
    
    return res;
}

void dns_svcparams::json_serialize() const
{
    auto params = json(json::object_e);

    for (auto i : m_params)
    {
        set_json(dns_svcparam::type_to_string(i.first), i.second.to_json());
    }
}

void dns_svcparams::json_unserialize()
{
    for (const auto &i : m_json_object->get_object())
    {
        dns_svcparam p(dns_svcparam::string_to_type(i.first));
        p.from_json(*i.second);
        m_params[p.get_type()] = p;
    }
}

ostream &operator<<(ostream& stream, const dns_svcparams &t)
{
    stream << t.to_string();
    return stream;
}
