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
 
#include <string.h>
#include "types.hpp"
#include "util.hpp"
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
    buffer bin = util::frombase64(s);

    if (s.size() >= MAX_EXTENDED_MESSAGE_SIZE)
    {
        THROW(txt_format_exception, "txt is too big", s.size());
    }

    size_t index = 0;

    while (index < bin.get_size())
    {
        octet *d = bin.get_data();
        octet length = d[index];

        if ((index + length) > bin.get_size())
        {
            THROW(txt_format_exception, "string overruns length of data");
        }

        index++;
        m_strings.push_back(make_shared<dns_label>(d + index, length));
        index += length;
    }
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

string dns_txt::to_base64() const
{
    // maximum possible buffer size is 256 data bytes + 1 length byte for each string
    buffer b(m_strings.size() * 256 + m_strings.size());
    size_t offset = 0;

    for (auto s : m_strings)
    {
        memcpy(b.get_data() + offset, &s->m_size, 1);
        offset += 1;
        memcpy(b.get_data() + offset, s->m_data, s->m_size);
        offset += s->m_size;
    }
    b.set_size(offset);
    return util::tobase64(b);
}

ostream &adns::operator<<(ostream& stream, const dns_txt &t)
{
    stream << t.to_string();
    return stream;
}
