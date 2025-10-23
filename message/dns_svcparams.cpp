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
