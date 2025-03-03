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
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#include "types.hpp"
#include "dns_message_serial.hpp"
#include "dns_message.hpp"
#include "exception.hpp"
#include "buffer.hpp"

using namespace std;
using namespace adns;

dns_message_serial::dns_message_serial()
{
}

dns_message_serial::~dns_message_serial()
{
}

void dns_message_serial::length_check(size_t size, size_t offset, size_t count) const
{
    if (offset + count > size)
    {
        THROW(message_serial_overrun_exception, "overrun when parsing or unparsing dns message", offset);
    }
}

void dns_message_serial::parse_data(octet *target, const octet *raw, size_t length, size_t size, size_t &offset) const
{
    length_check(size, offset, length);
    memcpy(target, raw + offset, length);
    offset += length;
}

void dns_message_serial::unparse_unsigned_short(unsigned short s, octet *target, size_t size, size_t &offset) const
{
    length_check(size, offset, 2);
    unsigned short value = htons(s);
    memcpy(target + offset, &value, 2);
    offset += 2;
}

void dns_message_serial::unparse_short(short s, octet *target, size_t &offset) const
{
    unsigned short value = htons(static_cast<unsigned short>(s));
    memcpy(target + offset, &value, 2);
    offset += 2;
}

void dns_message_serial::unparse_uint(uint i, octet *target, size_t size, size_t &offset) const
{
    length_check(size, offset, 4);
    uint value = htonl(i);
    memcpy(target + offset, &value, 4);
    offset += 4;
}

void dns_message_serial::unparse_int(int i, octet *target, size_t size, size_t &offset) const
{
    length_check(size, offset, 4);
    uint value = htonl(static_cast<uint>(i));
    memcpy(target + offset, &value, 4);
    offset += 4;
}

void dns_message_serial::unparse_octet(octet *target, const void *o, size_t size, size_t &offset) const
{
    length_check(size, offset, 1);
    memcpy(target + offset++, o, 1);
}

void dns_message_serial::unparse_octet(octet o, octet *target, size_t size, size_t &offset) const
{
    length_check(size, offset, 1);
    memcpy(target + offset++, &o, 1);
}

void dns_message_serial::unparse_data(octet *target, const void *o, size_t length, size_t size, size_t &offset) const
{
    length_check(size, offset, length);
    memcpy(target + offset, o, length);
    offset += length;
}

void dns_message_serial::unparse_name(
                const dns_name &n, 
                octet *target, 
                unordered_map<dns_name, unsigned short> &name_offsets, 
                size_t size, 
                size_t &offset,
                bool compress)
{
    if (compress)
    {
        dns_name index(n);

        for (auto iter : n.m_labels)
        {
            if (name_offsets.find(index) == name_offsets.end())
            {
                name_offsets[index] = offset;
                unparse_octet(target, &iter.get()->m_size, size, offset);
                unparse_data(target, iter->m_data, iter.get()->m_size, size, offset);
            }
            else
            {
                unsigned short s = name_offsets[index] | 0xC000;
                unparse_short(s, target, offset);
                return;
            }

            index.remove_first();
        }
    }
    else
    {
        for (auto iter : n.m_labels)
        {
            unparse_octet(target, &iter.get()->m_size, size, offset);
            unparse_data(target, iter->m_data, iter.get()->m_size, size, offset);
        }
    }

    octet end = 0;
    unparse_octet(target, &end, size, offset);
}

unsigned short dns_message_serial::parse_unsigned_short(const octet *raw, size_t size, size_t &offset) const
{
    unsigned short s;
    length_check(size, offset, 2);
    memcpy(&s, raw + offset, 2);
    s = ntohs(s);
    offset += 2;
    return s;
}

short dns_message_serial::parse_short(const octet *raw, size_t size, size_t &offset) const
{
    short s;
    length_check(size, offset, 2);
    memcpy(&s, raw + offset, 2);
    s = ntohs(s);
    offset += 2;
    return s;
}

uint dns_message_serial::parse_uint(const octet *raw, size_t size, size_t &offset) const
{
    uint i;
    length_check(size, offset, 4);
    memcpy(&i, raw + offset, 4);
    i = ntohl(i);
    offset += 4;
    return i;
}

int dns_message_serial::parse_int(const octet *raw, size_t size, size_t &offset) const
{
    int i;
    length_check(size, offset, 4);
    memcpy(&i, raw + offset, 4);
    i = ntohl(i);
    offset += 4;
    return i;
}

void dns_message_serial::parse_uint(uint &target, const octet *raw, size_t size, size_t &offset) const
{
    uint i;
    length_check(size, offset, 4);
    memcpy(&i, raw + offset, 4);
    target = ntohl(i);
    offset += 4;
}

octet dns_message_serial::parse_octet(const octet *raw, size_t size, size_t &offset) const
{
    length_check(size, offset, 1);
    return raw[offset++];
}

void dns_message_serial::parse_octet(void *target, const octet *raw, size_t size, size_t &offset) const
{
    length_check(size, offset, 1);
    memcpy(target, &raw[offset++], 1);
}

shared_ptr<dns_name> dns_message_serial::parse_name(const octet *raw, size_t size, size_t &offset)
{
    auto res = make_shared<dns_name>();
    bool compressed = false;

    size_t saved_offset = 0;

    int n = 0;
    int total_length = 0;

    // limit the number of times we can go around the loop so as to avoid
    // choking on malformed requests
    while (n++ < MAX_DOMAIN_NAME_COMPONENTS)
    {
        octet len = parse_octet(raw, size, offset);

        if (len == 0)
        {
            if (compressed)
            {
                offset = saved_offset;
            }
            return res;
        }


        if ((len & 0xC0) != 0)
        {
            if ((len & 0xC0) == 0xC0)
            {
                // compressed item
                compressed = true;

                if (saved_offset == 0)
                {
                    saved_offset = offset + 1;
                }
        
                octet x = parse_octet(raw, size, offset);
                len &= 0x3F;

                unsigned short pointer;

                memcpy(&pointer, &len, 1);
                memcpy(((octet *)&pointer) + 1, &x, 1);
                offset = ntohs(pointer);
            }
            else
            {
                THROW(message_serial_format_exception, "Top two bits of a label length field are invalid", len & 0xC0);
            }
        }
        else
        {
            length_check(size, offset, len);
            auto s = make_shared<dns_label>(raw + offset, len);
            total_length += (len + 1);

            if (total_length > (MAX_DOMAIN_NAME_LENGTH + 1))
            {
                THROW(message_serial_format_exception, "Domain name is too long", total_length);
            }

            res->append(s);
            offset += len;
        }
    }

    THROW(message_serial_format_exception, "Domain name has too many components");
}

ip_address dns_message_serial::parse_ip4_address(const octet *raw, size_t size, size_t &offset)
{
    struct in_addr ina;

    parse_data(reinterpret_cast<octet *>(&ina.s_addr), raw, 4, size, offset);

    return ip_address(ina);
}

void dns_message_serial::unparse_ip4_address(const ip_address &addr, octet *target, size_t size, size_t &offset)
{
    if (addr.get_type() != ip_address::ip_v4_e)
    {
        THROW(message_serial_parameter_exception, "Not an ip4 address");
    }
    unparse_data(target, &addr.m_in_addr.s_addr, 4, size, offset);
}

ip_address dns_message_serial::parse_ip6_address(const octet *raw, size_t size, size_t &offset)
{
    struct in6_addr ina;

    parse_data(static_cast<octet *>(ina.s6_addr), raw, 16, size, offset);

    return ip_address(ina);
}

void dns_message_serial::unparse_ip6_address(const ip_address &addr, octet *target, size_t size, size_t &offset)
{
    if (addr.get_type() != ip_address::ip_v6_e)
    {
        THROW(message_serial_parameter_exception, "Not an ip6 address");
    }
    unparse_data(target, static_cast<const octet *>(addr.m_in6_addr.s6_addr), 16, size, offset);
}

buffer dns_message_serial::parse_buffer(const octet *raw, uint buffer_length, size_t size, size_t &offset)
{
    // special case, no data at all
    if (buffer_length == 0)
    {
        return buffer();
    }

    if (buffer_length > MAX_EXTENDED_MESSAGE_SIZE )
    {
        THROW(message_serial_format_exception, "Resource record is too large", buffer_length);
    }

    length_check(size, offset, buffer_length);

    buffer b(buffer_length, raw + offset);
    offset += buffer_length;

    return b;
}

void dns_message_serial::unparse_buffer(const buffer &data, octet *target, size_t size, size_t &offset)
{
    unparse_unsigned_short(data.get_size(), target, size, offset);
    unparse_data(target, data.get_data(), data.get_size(), size, offset);
}

shared_ptr<dns_txt> dns_message_serial::parse_txt(const octet *raw, size_t size, size_t &offset)
{
    auto res = make_shared<dns_txt>();

    while (offset < size)
    {
        octet len = parse_octet(raw, size, offset);
        length_check(size, offset, len);
        res->append(make_shared<dns_label>(raw + offset, len));
        offset += len;
    }

    return res;
}

shared_ptr<dns_svcparams> dns_message_serial::parse_svcparams(const octet *raw, size_t size, size_t &offset)
{
    auto res = make_shared<dns_svcparams>();

    while (offset < size)
    {
        auto key = parse_unsigned_short(raw, size, offset);

        if (dns_svcparam::valid_type(key))
        {
            auto len = parse_unsigned_short(raw, size, offset);
            length_check(size, offset, len);

            switch (key)
            {
            case dns_svcparam::alpn_e : 
                {
                    dns_svcparam p(dns_svcparam::alpn_e);

                    uint i = 0;

                    while (i < len)
                    {
                        auto alpn_label_len = parse_octet(raw, size, offset);
                        length_check(size, offset, alpn_label_len);
                        p.m_alpns.push_back(string(reinterpret_cast<const char *>(raw + offset), alpn_label_len));
                        offset += alpn_label_len;
                        i += alpn_label_len + 2;
                    }

                    res->m_params[dns_svcparam::alpn_e] = p;
                }
                break;

            case dns_svcparam::no_default_alpn_e :
                // this param type has no data
                res->m_params[dns_svcparam::no_default_alpn_e] = dns_svcparam(dns_svcparam::no_default_alpn_e);
                break;

            case dns_svcparam::ipv4hint_e :
                {
                    dns_svcparam p(dns_svcparam::ipv4hint_e);

                    uint i = 0;

                    while (i < len)
                    {
                        p.m_ipv4_addrs.push_back(parse_ip4_address(raw, size, offset));
                        i += 4;
                    }

                    res->m_params[dns_svcparam::ipv4hint_e] = p;
                }
                break;

            case dns_svcparam::ipv6hint_e :
                {
                    dns_svcparam p(dns_svcparam::ipv6hint_e);

                    uint i = 0;

                    while (i < len)
                    {
                        p.m_ipv6_addrs.push_back(parse_ip6_address(raw, size, offset));
                        i += 16;
                    }

                    res->m_params[dns_svcparam::ipv6hint_e] = p;
                }
                break;

            case dns_svcparam::port_e :
                {
                    dns_svcparam p(dns_svcparam::port_e);
                    p.m_port = parse_unsigned_short(raw, size, offset);
                    res->m_params[dns_svcparam::port_e] = p;
                }
                break;

            case dns_svcparam::ech_e :
                {
                    dns_svcparam p(dns_svcparam::ech_e);
                    buffer b(len);
                    parse_data(b.get_data(), raw, len, size, offset);
                    p.m_ech = b;
                    res->m_params[dns_svcparam::ech_e] = p;
                }
                break;

            case dns_svcparam::mandatory_e :
                {
                    dns_svcparam p(dns_svcparam::mandatory_e);

                    uint i = 0;

                    while (i < len)
                    {
                        p.m_mandatory.push_back(static_cast<dns_svcparam::svcparam_type_t>(parse_unsigned_short(raw, size, offset)));
                        i += 2;
                    }

                    res->m_params[dns_svcparam::mandatory_e] = p;
                }
                break;

            default:
                THROW(message_serial_format_exception, "unhandled SVCPARAM type", key);
            }
        }
        else
        {
            THROW(message_serial_format_exception, "SVCPARAMS contain invalid type", key);
        }
    }

    return res;
}

void dns_message_serial::unparse_txt(const shared_ptr<dns_txt> &txt, octet *target, size_t size, size_t &offset)
{
    for (auto s : txt->m_strings)
    {
        unparse_octet(target, &s->m_size, size, offset);
        unparse_data(target, s->m_data, s->m_size, size, offset);
    }
}

shared_ptr<dns_label> dns_message_serial::parse_string(const octet *raw, size_t size, size_t &offset)
{
    octet len = parse_octet(raw, size, offset);
    length_check(size, offset, len);
    auto res = make_shared<dns_label>(raw + offset, len);
    offset += len;
    return res;
}

void dns_message_serial::unparse_string(const shared_ptr<dns_label> &string, octet *target, size_t size, size_t &offset)
{
    unparse_octet(target, &string->m_size, size, offset);
    unparse_data(target, string->m_data, string->m_size, size, offset);
}

buffer dns_message_serial::parse_remaining_buffer(const octet *raw, size_t size, size_t &offset)
{
    if (offset < size)
    {
        buffer res(size - offset, raw + offset);
        offset = size;
        return res;
    }
    else
    {
        return buffer(0);
    }
}

string dns_message_serial::parse_remaining_string(const octet *raw, size_t size, size_t &offset)
{
    if (offset < size)
    {
        string res((char *)(raw + offset), size - offset);
        offset = size;
        return res;
    }
    else
    {
        return "";
    }
}

void dns_message_serial::unparse_remaining_string(const string &s, octet *target, size_t size, size_t &offset)
{
    length_check(size, offset, s.size());
    memcpy(target + offset, s.c_str(), s.size());
    offset += s.size();
}

void dns_message_serial::unparse_remaining_buffer(const buffer &b, octet *target, size_t size, size_t &offset)
{
    length_check(size, offset, b.get_size());
    memcpy(target + offset, b.get_data(), b.get_size());
    offset += b.get_size();
}

void dns_message_serial::unparse_datetime(const datetime &dt, octet *target, size_t size, size_t &offset)
{
    unparse_uint(dt.to_time_t(), target, size, offset);
}

datetime dns_message_serial::parse_datetime(const octet *raw, size_t size, size_t &offset)
{
    return datetime(static_cast<time_t>(parse_uint(raw, size, offset)));
}

shared_ptr<dns_type_bitmap> dns_message_serial::parse_type_bitmap(const octet *raw, size_t size, size_t &offset)
{
    auto res = make_shared<dns_type_bitmap>();

    while (offset < size)
    {
        octet page = parse_octet(raw, size, offset);
        octet page_length = parse_octet(raw, size, offset);

        if (page_length > 32)
        {
            THROW(message_unparser_exception, "type bitmap has page with illegal length (>32)", page_length);
        }

        octet buffer[32];
        parse_data(buffer, raw, page_length, size, offset);

        for (octet byte = 0; byte < page_length; byte++)
        {
            for (octet bit = 0; bit < 8; bit++)
            {
                if ((buffer[byte] & (0x80 >> bit)) != 0)
                {
                    res->add(static_cast<dns_rr::type_t>((page * 256) + (byte * 8 + bit)));
                }
            }
        }
    }

    return res;
}

void dns_message_serial::unparse_type_bitmap(const shared_ptr<dns_type_bitmap> &tb, octet *target, size_t size, size_t &offset)
{
    if (tb->get_types().size() == 0)
    {
        THROW(message_unparser_exception, "type bitmap has no entries");
    }

    octet bitmap_length = 32;
    octet bitmap[32];
    memset(bitmap, 0, 32);

    for (auto t : tb->get_types())
    {
        if (t > 255)
        {
            THROW(message_unparser_exception, "type bitmap has entry with value greater than 255", t);
        }

        int byte = t / 8;
        int bit = t % 8;

        bitmap[byte] |= 0x80 >> bit;
    }

    while (bitmap[bitmap_length - 1] == 0)
    {
        bitmap_length--;
    }

    // window - only the one since we don't (currently at least) handle any rr types > 255
    unparse_octet(0, target, size, offset);
    unparse_octet(bitmap_length, target, size, offset);
    unparse_data(target, bitmap, bitmap_length, size, offset);
}

void dns_message_serial::unparse_svcparams(const shared_ptr<dns_svcparams> &params, octet *target, size_t size, size_t &offset)
{
    for (auto i : params->m_params)
    {
        switch (i.first)
        {
        case dns_svcparam::alpn_e :
            {
                uint len = 0;
                for (auto a : i.second.m_alpns)
                {
                    if (a.size() > 255)
                    {
                        THROW(message_serial_format_exception, "SVCPARAM ALPN label has > 255 length", a);
                    }
                    len += 1 + a.size();
                }

                unparse_unsigned_short(dns_svcparam::alpn_e, target, size, offset);
                unparse_unsigned_short(len, target, size, offset);

                for (auto a : i.second.m_alpns)
                {
                    unparse_octet(a.size(), target, size, offset);
                    unparse_data(target, a.c_str(), a.size(), size, offset);
                }
            }
            break;

        case dns_svcparam::no_default_alpn_e :
            unparse_unsigned_short(dns_svcparam::no_default_alpn_e, target, size, offset);
            unparse_unsigned_short(0, target, size, offset);
            break;

        case dns_svcparam::mandatory_e :
            unparse_unsigned_short(dns_svcparam::mandatory_e, target, size, offset);
            unparse_unsigned_short(i.second.m_mandatory.size() * 2, target, size, offset);
            for (auto i : i.second.m_mandatory)
            {
                unparse_unsigned_short(i, target, size, offset);
            }
            break;

        case dns_svcparam::port_e :
            unparse_unsigned_short(dns_svcparam::port_e, target, size, offset);
            unparse_unsigned_short(2, target, size, offset);
            unparse_unsigned_short(i.second.m_port, target, size, offset);
            break;

        case dns_svcparam::ipv4hint_e :
            unparse_unsigned_short(dns_svcparam::ipv4hint_e, target, size, offset);
            unparse_unsigned_short(i.second.m_ipv4_addrs.size() * 4, target, size, offset);
            for (auto i : i.second.m_ipv4_addrs)
            {
                unparse_ip4_address(i, target, size, offset);
            }
            break;

        case dns_svcparam::ipv6hint_e :
            unparse_unsigned_short(dns_svcparam::ipv6hint_e, target, size, offset);
            unparse_unsigned_short(i.second.m_ipv6_addrs.size() * 16, target, size, offset);
            for (auto i : i.second.m_ipv6_addrs)
            {
                unparse_ip6_address(i, target, size, offset);
            }
            break;

        case dns_svcparam::ech_e :
            unparse_unsigned_short(dns_svcparam::ech_e, target, size, offset);
            unparse_unsigned_short(i.second.m_ech.get_size(), target, size, offset);
            unparse_data(target, i.second.m_ech.get_data(), i.second.m_ech.get_size(), size, offset);
            break;

        default:
            THROW(message_serial_format_exception, "unhandled SVCPARAM type", i.first);
        }
    }
}

eui48 dns_message_serial::parse_eui48(const octet *raw, size_t size, size_t &offset)
{
    eui48 res;
    parse_data(res.m_eui48, raw, 6, size, offset);
    return res;
}

eui64 dns_message_serial::parse_eui64(const octet *raw, size_t size, size_t &offset)
{
    eui64 res;
    parse_data(res.m_eui64, raw, 8, size, offset);
    return res;
}

void dns_message_serial::unparse_eui48(const eui48 &addr, octet *target, size_t size, size_t &offset)
{
    unparse_data(target, addr.m_eui48, 6, size, offset);
}

void dns_message_serial::unparse_eui64(const eui64 &addr, octet *target, size_t size, size_t &offset)
{
    unparse_data(target, addr.m_eui64, 8, size, offset);
}

shared_ptr<dns_ipseckey> dns_message_serial::parse_ipseckey(const octet *raw, size_t size, size_t &offset)
{
    auto res = make_shared<dns_ipseckey>();

    res->m_precedence = parse_octet(raw, size, offset);
    res->m_gateway_type = dns_ipseckey::gateway_type_t(parse_octet(raw, size, offset));
    res->m_algorithm = parse_octet(raw, size, offset);

    switch (res->m_gateway_type)
    {
    case dns_ipseckey::ipv4_e :
        res->m_ipv4_gateway = parse_ip4_address(raw, size, offset);
        break;
    case dns_ipseckey::ipv6_e :
        res->m_ipv6_gateway = parse_ip6_address(raw, size, offset);
        break;
    case dns_ipseckey::name_e :
        res->m_name_gateway = *parse_name(raw, size, offset);
        break;
    default:
    case dns_ipseckey::none_e :
        break;
    }

    res->m_public_key = parse_remaining_buffer(raw, size, offset);

    return res;
}

void dns_message_serial::unparse_ipseckey(const shared_ptr<dns_ipseckey> &key, octet *target, size_t size, size_t &offset)
{
    unparse_octet(key->m_precedence, target, size, offset);
    unparse_octet(key->m_gateway_type, target, size, offset);
    unparse_octet(key->m_algorithm, target, size, offset);

    switch (key->m_gateway_type)
    {
    case dns_ipseckey::ipv4_e :
        unparse_ip4_address(key->m_ipv4_gateway, target, size, offset);
        break;
    case dns_ipseckey::ipv6_e :
        unparse_ip6_address(key->m_ipv6_gateway, target, size, offset);
        break;
    case dns_ipseckey::name_e :
        {
            unordered_map<dns_name, unsigned short> offsets;
            unparse_name(key->m_name_gateway, target, offsets, size, offset, false);
        }
        break;
    default:
    case dns_ipseckey::none_e :
        break;
    }

    unparse_buffer(key->m_public_key, target, size, offset);
}
