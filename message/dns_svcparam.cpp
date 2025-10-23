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
#include "dns_svcparam.hpp"

using namespace std;
using namespace adns;

map<dns_svcparam::svcparam_type_t, string> type_to_name =
{
    { dns_svcparam::mandatory_e, "mandatory" },
    { dns_svcparam::alpn_e, "alpn" },
    { dns_svcparam::no_default_alpn_e, "no-default-alpn" },
    { dns_svcparam::port_e, "port" },
    { dns_svcparam::ipv4hint_e, "ipv4hint" },
    { dns_svcparam::ech_e, "ech" },
    { dns_svcparam::ipv6hint_e, "ipv6hint" }
}; 

map<string, dns_svcparam::svcparam_type_t> name_to_type =
{
    { "mandatory", dns_svcparam::mandatory_e },
    { "alpn", dns_svcparam::alpn_e },
    { "no-default-alpn", dns_svcparam::no_default_alpn_e },
    { "port", dns_svcparam::port_e },
    { "ipv4hint" , dns_svcparam::ipv4hint_e },
    { "ech", dns_svcparam::ech_e },
    { "ipv6hint", dns_svcparam::ipv6hint_e }
}; 

dns_svcparam::dns_svcparam() : m_type(invalid_e)
{
}

dns_svcparam::dns_svcparam(svcparam_type_t t) : m_type(t)
{
}

dns_svcparam::~dns_svcparam()
{
}

bool dns_svcparam::valid_type(unsigned short s)
{
    return s >=0 && s <= 6;
}

string dns_svcparam::to_string() const
{
    switch (m_type)
    {
    case alpn_e :
        {
            string alpns;
            for (auto a : m_alpns)
            {
                if (alpns != "")
                {
                    alpns += ",";
                }
                alpns += a;
            }
            return type_to_string(m_type) + "=" + alpns;
        }

    case no_default_alpn_e :
        return type_to_string(m_type);

    case ipv4hint_e :
        {
            string ipv4s;
            for (auto a : m_ipv4_addrs)
            {
                if (ipv4s != "")
                {
                    ipv4s += ",";
                }
                ipv4s += a.to_string();
            }
            return type_to_string(m_type) + "=" + ipv4s;
        }

    case ipv6hint_e :
        {
            string ipv6s;
            for (auto a : m_ipv6_addrs)
            {
                if (ipv6s != "")
                {
                    ipv6s += ",";
                }
                ipv6s += a.to_string();
            }
            return type_to_string(m_type) + "=" + ipv6s;
        }

    case port_e :
        return type_to_string(m_type) + "=" + boost::lexical_cast<string>(m_port);

    case ech_e :
        return type_to_string(m_type) + "=" + util::tobase64(m_ech);

    case mandatory_e :
        {
            string m;
            for (auto a : m_mandatory)
            {
                if (m != "")
                {
                    m += ",";
                }
                m += type_to_string(a);
            }
            return type_to_string(m_type) + "=" + m;
        }

    default :
        THROW(dns_svcparam_exception, "unknown type", m_type);
    }
}

void dns_svcparam::json_serialize() const
{
    switch (m_type)
    {
    case alpn_e :
        m_json_object = make_shared<json>(json::array_e);
        for (auto a : m_alpns)
        {
            m_json_object->append(a);
        }
        break;

    case no_default_alpn_e :
        m_json_object = make_shared<json>("set");
        break;

    case ipv4hint_e :
        m_json_object = make_shared<json>(json::array_e);
        for (auto a : m_ipv4_addrs)
        {
            m_json_object->append(a.to_string());
        }
        break;

    case ipv6hint_e :
        m_json_object = make_shared<json>(json::array_e);
        for (auto a : m_ipv6_addrs)
        {
            m_json_object->append(a.to_string());
        }
        break;

    case port_e :
        m_json_object = make_shared<json>(m_port);
        break;

    case ech_e :
        m_json_object = make_shared<json>(util::tobase64(m_ech));
        break;

    case mandatory_e :
        m_json_object = make_shared<json>(json::array_e);
        for (auto a : m_mandatory)
        {
            m_json_object->append(type_to_string(a));
        }
        break;

    default :
        THROW(dns_svcparam_exception, "unknown type", m_type);
    }
}

void dns_svcparam::json_unserialize()
{
    switch (m_type)
    {
    case alpn_e :
        {
            const vector<unique_ptr<json>> &a = m_json_object->get_array();
            for (size_t i = 0; i < a.size(); i++)
            {
                m_alpns.push_back(*a[i]);
            }
        }
        break;

    case no_default_alpn_e :
        // do nothing - the json value is just a placeholder
        break;

    case ipv4hint_e :
        {
            const vector<unique_ptr<json>> &a = m_json_object->get_array();
            for (size_t i = 0; i < a.size(); i++)
            {
                m_ipv4_addrs.push_back(ip_address(*a[i]));
            }
        }
        break;

    case ipv6hint_e :
        {
            const vector<unique_ptr<json>> &a = m_json_object->get_array();
            for (size_t i = 0; i < a.size(); i++)
            {
                m_ipv6_addrs.push_back(ip_address(*a[i]));
            }
        }
        break;

    case port_e :
        m_port = (int)*m_json_object;
        break;

    case ech_e :
        m_ech = util::frombase64(*m_json_object);
        break;

    case mandatory_e :
        {
            const vector<unique_ptr<json>> &a = m_json_object->get_array();
            for (size_t i = 0; i < a.size(); i++)
            {
                m_mandatory.push_back(string_to_type(*a[i]));
            }
        }
        break;

    default :
        THROW(dns_svcparam_exception, "unknown type", m_type);
    }
}

string dns_svcparam::type_to_string(svcparam_type_t t)
{
    auto i = type_to_name.find(t);
    if (i == type_to_name.end())
    {
        THROW(dns_svcparam_exception, "unknown type", t);
    }
    else
    {
        return i->second;
    }
}

dns_svcparam::svcparam_type_t dns_svcparam::string_to_type(const std::string &name)
{
    auto i = name_to_type.find(name);
    if (i == name_to_type.end())
    {
        THROW(dns_svcparam_exception, "unknown type", name);
    }
    else
    {
        return i->second;
    }
}

dns_svcparam::svcparam_type_t dns_svcparam::get_type() const
{
    return m_type;
}
