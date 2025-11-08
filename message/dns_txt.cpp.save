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
#include "util.hpp"
#include "parser.hpp"
#include "dns_txt.hpp"
#include "dns_message.hpp"

using namespace std;
using namespace adns;

dns_txt::dns_txt()
{
}

dns_txt::~dns_txt()
{
}

dns_txt::dns_txt(const string &s)
{
    auto j = parser::parse(s);
    from_json(*j);
}

dns_txt::dns_txt(const json &j)
{
    from_json(j);
}

string dns_txt::to_string() const
{
    string res;

    for (auto i : m_strings)
    {
        if (res != "")
        {
            res += ",";
        }
        res += i->to_string();
    }

    return res;
}

void dns_txt::append(const shared_ptr<dns_label> &string)
{
    m_strings.push_back(string);
}

ostream &adns::operator<<(ostream& stream, const dns_txt &t)
{
    stream << t.to_string();
    return stream;
}

void dns_txt::json_serialize() const
{
    (*m_json_object)["strings"] = json(json::array_e);
    for (auto s : m_strings)
    {
        (*m_json_object)["strings"].append(json(s->to_string()));
    }
}

void dns_txt::json_unserialize()
{
    for (auto const &s : (*m_json_object)["strings"].get_array())
    {
        m_strings.push_back(make_shared<dns_label>(string(*s)));
    }
}
