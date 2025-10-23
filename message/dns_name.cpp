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
 
#include <ctype.h>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "types.hpp"
#include "dns_name.hpp"

using namespace std;
using namespace adns;

const dns_name dns_name::root(".");

dns_name::dns_name()
{
    m_hash = 0;
}

dns_name::dns_name(const dns_name &other)
{
    *this = other;
}

dns_name::dns_name(const string &s) : dns_name()
{
    if (s.size() != 0 && s != ".")
    {
        string r = s;

        if (r.back() == '.')
        {
            r.pop_back();
        }

        vector<string> bits;
        split(bits, r, boost::is_any_of("."));

        for (auto i : bits)
        {
            append(make_shared<dns_label>(i));
        }
    }
}

dns_name::~dns_name()
{
}

bool dns_name::is_root() const
{
    return (m_labels.size() == 0) || ((m_labels.size() == 1) && (m_labels.front()->get_length() == 0));
}

void dns_name::append(const shared_ptr<dns_label> &s)
{
    m_hash = 0;
    m_labels.push_back(s);
}

void dns_name::append(const dns_name &n)
{
    for (auto l : n.m_labels)
    {
        m_labels.push_back(l);
    }
}

void dns_name::remove_last_n_labels(size_t n)
{
    while ((n-- > 0) && !m_labels.empty())
    {
        m_labels.erase(prev(m_labels.end()));
    }
}

void dns_name::prepend(const shared_ptr<dns_label> &s)
{
    m_hash = 0;
    m_labels.push_front(s);
}

void dns_name::prepend(const list<shared_ptr<dns_label>> &labels)
{
    m_hash = 0;
    m_labels.insert(m_labels.begin(), labels.begin(), labels.end());
}

const list<shared_ptr<dns_label>> &dns_name::get_labels() const
{
    return m_labels;
}

size_t dns_name::size() const
{
    return m_labels.size();
}

list<shared_ptr<dns_label>> dns_name::get_labels(size_t n) const
{
    list<shared_ptr<dns_label>> res;
    
    for (auto x : m_labels)
    {
        if (n == 0)
        {
            return res;
        }
        else
        {
            res.push_back(x);
            n--;
        }
    }

    return res;
}

const dns_name &dns_name::operator=(const dns_name &other)
{
    m_hash = other.m_hash;
    m_labels = other.m_labels;
    return *this;
}

bool dns_name::operator==(const dns_name &other) const
{
    auto this_label = m_labels.begin();
    auto other_label = other.m_labels.begin();

    while (true)
    {
        if (this_label == m_labels.end() && other_label == other.m_labels.end())
        {
            return true;
        }
        else if ((this_label == m_labels.end() && other_label != other.m_labels.end()) ||
                 (this_label != m_labels.end() && other_label == other.m_labels.end()))
        {
            return false;
        }
        else if (**this_label != **other_label)
        {
            return false;
        }

        this_label++;
        other_label++;
    }
}

bool dns_name::wildcard_equals(const dns_name &other) const
{
    auto this_label = m_labels.rbegin();
    auto other_label = other.m_labels.rbegin();

    while (true)
    {
        if (this_label == m_labels.rend())
        {
            return other_label == other.m_labels.rend();
        }
        else if (other_label == other.m_labels.rend())
        {
            return false;
        }
        else
        {
            if ((next(this_label) == m_labels.rend()) && (**this_label == dns_label::wildcard))
            {
                return true;
            }
            else
            {
                if (**this_label != **other_label)
                {
                    return false;
                }
                else
                {
                    this_label++;
                    other_label++;
                }
            }
        }
    }
}

bool dns_name::operator<(const dns_name &other) const
{
    return this->to_string() < other.to_string();
}

void dns_name::remove_first()
{
    m_hash = 0;
    m_labels.pop_front();
}

bool dns_name::operator!=(const dns_name &other) const
{
    return !(*this == other);
}

string dns_name::to_string() const
{
    string res;

    for (auto i : m_labels)
    {
        if (res != "")
        {
            res += ".";
        }
        res += i.get()->to_string();
    }

    return res;
}

size_t dns_name::get_hash() const
{
    if (m_hash != 0)
    {
        return m_hash;
    }

    m_hash = 5381;

    for (auto l : m_labels)
    {
        const octet *x = l->m_data;
        auto size = l->m_size;
        for (auto j = 0; j < size; j++)
        {
            m_hash = ((m_hash << 5) + m_hash) + tolower(x[j]);
        }
    }

    return m_hash;
}

shared_ptr<dns_name> dns_name::clone() const
{
    auto res = make_shared<dns_name>();
    res->m_labels = m_labels;
    res->m_hash = m_hash;
    return res;
}

void dns_name::clear()
{
    m_labels.clear();
    m_hash = 0;
}

bool dns_name::is_subdomain_of(const dns_name &other) const
{
    auto last = m_labels.rbegin();   
    auto other_last = other.m_labels.rbegin();   

    while (last != m_labels.rend() && other_last != other.m_labels.rend())
    {
        if (**last++ != **other_last++)
        {
            return false;
        }
    }

    // if we haven't checked all labels in the other name, we can't be a sub-domain of it
    // and if we've checked all of both, then we're equal
    return (other_last == other.m_labels.rend()) && (last != m_labels.rend());
}

ostream &adns::operator<<(ostream& stream, const dns_name &n)
{
    stream << n.to_string();
    return stream;
}

