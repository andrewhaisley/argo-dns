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
