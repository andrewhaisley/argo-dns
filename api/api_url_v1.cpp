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
#include <boost/lexical_cast.hpp>

#include "api_url_v1.hpp"
#include "api_exception.hpp"

using namespace std;
using namespace adns;

api_url_v1::api_url_v1() : url()
{
}

api_url_v1::~api_url_v1()
{
}

api_url_v1::api_url_v1(const string &p) : url(p)
{
    extract_version_type_and_resources();
    extract_resource_filters();
}

api_url_v1::resource_type_t api_url_v1::string_to_type(string type) const
{
    if (type == "monitor")
    {
        return monitor_e;
    }
    else if (type == "run")
    {
        return run_e;
    }
    else if (type == "server")
    {
        return server_e;
    }
    else if (type == "horizon")
    {
        return horizon_e;
    }
    else if (type == "zone")
    {
        return zone_e;
    }
    else if (type == "rr")
    {
        return rr_e;
    }
    else if (type == "address_list")
    {
        return address_list_e;
    }
    else if (type == "base_configuration")
    {
        return base_configuration_e;
    }
    else if (type == "cache_configuration")
    {
        return cache_configuration_e;
    }
    else
    {
        THROW(api_malformed_exception, "invalid resource type", type);
    }
}

api_url_v1::resource_type_t api_url_v1::get_resource_type() const
{
    return m_type;
}

void api_url_v1::extract_version_type_and_resources()
{
    vector<string> bits;

    split(bits, get_path(), boost::is_any_of("/"));

    if (bits.size() >= 2)
    {
        try
        {
            m_version = boost::lexical_cast<int>(bits[1]);
        }
        catch (boost::bad_lexical_cast&)
        {
            THROW(api_malformed_exception, "API path didn't start with a valid version number");
        }
    }
    else
    {
        THROW(api_malformed_exception, "API path empty");
    }

    if (bits.size() >= 3)
    {
        m_type = string_to_type(bits[2]);

        if (bits.size() >= 4 && bits[3] != "")
        {
            try
            {
                m_resource_id = boost::lexical_cast<uuid>(bits[3]);
            }
            catch (boost::bad_lexical_cast&)
            {
                THROW(api_malformed_exception, "invalid UUID", bits[3]);
            }
        }
    }
    else
    {
        THROW(api_malformed_exception, "API path had nothing after version number");
    }
}

void api_url_v1::extract_resource_filters()
{
    for (auto i : get_parameters()) 
    {
        if (i.first == "name")
        {
            m_name_filter = regex(i.second);
        }
        else if (i.first == "type")
        {
            m_record_type_filter = dns_rr::string_to_type(i.second);
        }
        else
        {
            auto t = string_to_type(i.first);
            try
            {
                auto u = boost::lexical_cast<uuid>(i.second);
                m_filter_uuids[t] = u;
            }
            catch (boost::bad_lexical_cast&)
            {
                THROW(api_malformed_exception, "invalid resource UUID", i.first);
            }
        }
    }
}

nullable<regex> api_url_v1::get_name_filter() const
{
    return m_name_filter;
}

nullable<uuid> api_url_v1::get_resource_filter_id(resource_type_t t) const
{
    auto i = m_filter_uuids.find(t);
    if (i == m_filter_uuids.end())
    {
        return nullable<uuid>();
    }
    else
    {
        return nullable<uuid>(i->second);
    }
}

uint api_url_v1::get_version() const
{
    return m_version;
}

nullable<uuid> api_url_v1::get_resource_id() const
{
    return m_resource_id;
}

nullable<dns_rr::type_t> api_url_v1::get_record_type_filter() const
{
    return m_record_type_filter;
}
