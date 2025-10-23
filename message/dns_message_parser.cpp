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
 
#include <unordered_map>

#include "config.hpp"
#include "util.hpp"
#include "exception.hpp"
#include "dns_message_parser.hpp"
#include "dns_rr_parser.hpp"

using namespace adns;
using namespace std;

shared_ptr<dns_question> dns_message_parser::parse_question(const octet *raw, size_t size, size_t &offset)
{
    shared_ptr<dns_name> qname = parse_name(raw, size, offset);

    unsigned short raw_qtype = parse_short(raw, size, offset);
    unsigned short raw_qclass = parse_short(raw, size, offset);

    dns_question::qtype_t  qtype = static_cast<dns_question::qtype_t>(raw_qtype);
    dns_question::qclass_t qclass = static_cast<dns_question::qclass_t>(raw_qclass);
    dns_rr::type_t         rr_type;

    if ((qtype == dns_question::QT_IXFR_e) || (qtype == dns_question::QT_AXFR_e) || (qtype == dns_question::QT_ALL_e))
    {
        rr_type = dns_rr::T_NONE_e;
    }
    else
    {
        rr_type = static_cast<dns_rr::type_t>(raw_qtype);
        qtype = dns_question::QT_RR_e;
    }

    return make_shared<dns_question>(*qname, qtype, rr_type, qclass);
}

dns_message_parser::dns_message_parser()
{
}

dns_message_parser::~dns_message_parser()
{
}

void dns_message_parser::parse_edns_options(dns_message *m, dns_rr_OPT *rr)
{
    // can only have one OPT RR
    if (m->m_is_edns)
    {
        THROW(message_parser_format_exception, "more than one OPT resource record found");
    }

    m->m_is_edns = true;
    m->m_payload_size = rr->get_class();

    if (m->m_payload_size < STANDARD_MESSAGE_SIZE)
    {
        m->m_payload_size = STANDARD_MESSAGE_SIZE;
    }

    m->m_response_code = static_cast<dns_message::response_code_t>(static_cast<int>(m->m_response_code) | ((rr->get_ttl() & 0xff000000) >> 16));
    m->m_edns_version = (rr->get_ttl() & 0x00ff0000) >> 16;
    m->m_is_dnssec_ok = (rr->get_ttl() & 0x00008000) != 0;
}

dns_message *dns_message_parser::from_wire(const buffer &raw_buffer)
{
    dns_message *m = new dns_message;

    try
    {
        size_t i = 0;
        size_t size = raw_buffer.get_size();

        const octet *raw = raw_buffer.get_data();

        m->m_id = parse_short(raw, size, i);

        header_part_1 part_1;
        header_part_2 part_2;

        parse_octet(&part_1, raw, size, i);
        parse_octet(&part_2, raw, size, i);

        m->m_type = static_cast<dns_message::type_t>(part_1.qr);
        m->m_op_code = static_cast<dns_message::op_code_t>(part_1.opcode);
        m->m_is_authoritative = part_1.aa;
        m->m_is_truncated = part_1.tc;
        m->m_is_recursion_desired = part_1.rd;
        m->m_is_recursion_available = part_2.ra;
        m->m_response_code = static_cast<dns_message::response_code_t>(part_2.rcode);

        unsigned short qd_count = parse_short(raw, size, i);

        // RFC1035 allows more than one question but that makes no sense since the surrounding
        // flags in the message could then be ambiguous. Bind treats this as a format error
        // so we'll do the same. Zero questions makes no sense either.
        if (qd_count != 1)
        {
            THROW(message_parser_format_exception, "more than 1 or 0 questions found in dns query", qd_count);
        }

        m->m_an_count = parse_short(raw, size, i);
        m->m_ns_count = parse_short(raw, size, i);
        m->m_ar_count = parse_short(raw, size, i);

        shared_ptr<dns_rr> rr;

        m->m_question = parse_question(raw, size, i);

        for (unsigned short x = 0; x < m->m_an_count; x++)
        {
            rr = m_rr_parser.parse(raw, size, i);

            if (rr->get_type() == dns_rr::T_OPT_e)
            {
                parse_edns_options(m, dynamic_cast<dns_rr_OPT *>(rr.get()));
            }
            else
            {
                m->m_answers.push_back(rr);
            }
        }

        for (unsigned short x = 0; x < m->m_ns_count; x++)
        {
            rr = m_rr_parser.parse(raw, size, i);

            if (rr->get_type() == dns_rr::T_OPT_e)
            {
                parse_edns_options(m, dynamic_cast<dns_rr_OPT *>(rr.get()));
            }
            else
            {
                m->m_nameservers.push_back(rr);
            }
        }

        for (unsigned short x = 0; x < m->m_ar_count; x++)
        {
            rr = m_rr_parser.parse(raw, size, i);

            if (rr->get_type() == dns_rr::T_OPT_e)
            {
                parse_edns_options(m, dynamic_cast<dns_rr_OPT *>(rr.get()));
            }
            else
            {
                m->m_additional_records.push_back(rr);
            }
        }

        return m;
    }
    catch (...)
    {
        delete m;
        throw;
    }
}

void dns_message_parser::unparse_question(octet *raw, unordered_map<dns_name, unsigned short> &name_offsets, const dns_question &q, size_t size, size_t &offset, bool compress)
{
    unparse_name(q.m_qname, raw, name_offsets, size, offset, compress);

    if (q.m_qtype == dns_question::QT_RR_e)
    {
        unparse_unsigned_short(static_cast<unsigned short>(q.m_rr_type), raw, size, offset);
    }
    else
    {
        unparse_unsigned_short(static_cast<unsigned short>(q.m_qtype), raw, size, offset);
    }

    unparse_unsigned_short(static_cast<unsigned short>(dns_question::QC_IN_e), raw, size, offset);
}

buffer dns_message_parser::to_wire(const dns_message &m, size_t max_size)
{
    try
    {
        buffer raw = to_wire(m, false, max_size);

        // if the message goes over 512, then try again with compression
        if (raw.get_size() > STANDARD_MESSAGE_SIZE)
        {
            return to_wire(m, true, max_size);
        }
        else
        {
            return raw;
        }
    }
    catch (message_serial_overrun_exception &e)
    {
        // try again with compression to see if we can get it under the max size
        return to_wire(m, true, max_size);
    }
}

buffer dns_message_parser::to_wire(const dns_message &m, bool compress, size_t max_size)
{
    size_t i = 0;

    unordered_map<dns_name, unsigned short> name_offsets;

    octet *raw = new octet[max_size];

    try
    {
        unparse_unsigned_short(m.m_id, raw, max_size, i);

        header_part_1 part_1;
        header_part_2 part_2;

        part_1.qr = static_cast<octet>(m.m_type);
        part_1.opcode = static_cast<octet>(m.m_op_code);
        part_1.aa = m.m_is_authoritative ? 1 : 0;
        part_1.tc = m.m_is_truncated ? 1 : 0;
        part_1.rd = m.m_is_recursion_desired ? 1 : 0;
        part_2.ra = m.m_is_recursion_available ? 1 : 0;
        part_2.rcode = static_cast<octet>(m.m_response_code & 0x0f);
        part_2.z = 0;

        unparse_octet(raw, &part_1, max_size, i);
        unparse_octet(raw, &part_2, max_size, i);

        unparse_unsigned_short(m.m_question ? 1 : 0, raw, max_size, i);
        unparse_unsigned_short(m.m_an_count, raw, max_size, i);
        unparse_unsigned_short(m.m_ns_count, raw, max_size, i);

        if (m.m_is_edns)
        {
            unparse_unsigned_short(m.m_ar_count + 1, raw, max_size, i);
        }
        else
        {
            unparse_unsigned_short(m.m_ar_count, raw, max_size, i);
        }

        if (m.m_question)
        {
            unparse_question(raw, name_offsets, *m.m_question, max_size, i, compress);
        }

        for (auto iter : m.m_answers)
        {
            if (iter)
            {
                m_rr_parser.unparse(raw, name_offsets, *iter, max_size, i, compress);
            }
            else
            {
                LOG(error) << "null answer record found, ignored";
            }
        }

        for (auto iter : m.m_nameservers)
        {
            if (iter)
            {
                m_rr_parser.unparse(raw, name_offsets, *iter, max_size, i, compress);
            }
            else
            {
                LOG(error) << "null nameserver record found, ignored";
            }
        }

        for (auto iter : m.m_additional_records)
        {
            if (iter)
            {
                m_rr_parser.unparse(raw, name_offsets, *iter, max_size, i, compress);
            }
            else
            {
                LOG(error) << "null additional record found, ignored";
            }
        }

        if (m.m_is_edns)
        {
            // name
            octet zero = 0;
            unparse_octet(raw, &zero, max_size, i);
            // type
            unparse_unsigned_short(dns_rr::T_OPT_e, raw, max_size, i);
            // class 
            unparse_unsigned_short(m.m_payload_size, raw, max_size, i);

            uint ttl = m.m_edns_version << 16;

            ttl |= (m.m_response_code >> 4) << 24;
            if (m.m_is_dnssec_ok)
            {
                ttl |= 0x8000;
            }

            // TTL
            unparse_uint(ttl, raw, max_size, i);

            // data length
            unparse_unsigned_short(0, raw, max_size, i);
        }
    }
    catch (...)
    {
        delete [] raw;
        throw;
    }

    buffer b(i, raw);
    delete [] raw;
    return b;
}

uint dns_message_parser::extract_id(const buffer &raw) noexcept
{
    if (raw.get_size() >= 2)
    {
        uint res = 0;
        memcpy(&res, raw.get_data(), 2);
        return ntohs(res);
    }
    else
    {
        return 0;
    }
}

void dns_message_parser::set_id(const buffer &raw, uint id) noexcept
{
    if (raw.get_size() >= 2)
    {
        uint x = htons(id);
        memcpy(raw.get_data(), &x, 2);
    }
}

dns_message::op_code_t dns_message_parser::extract_opcode(const buffer &raw) noexcept
{
    if (raw.get_size() >= 3)
    {
        header_part_1 part_1;
        memcpy(&part_1, raw.get_data() + 3, 1);
        return static_cast<dns_message::op_code_t>(part_1.opcode);
    }
    else
    {
        return dns_message::op_query_e;
    }
}

void dns_message_parser::set_truncated(const buffer &raw, bool truncated)
{
    if (raw.get_size() >= 3)
    {
        header_part_1 part_1;
        memcpy(&part_1, raw.get_data() + 2, 1);
        part_1.tc = truncated ? 1 : 0;
        memcpy(raw.get_data() + 2, &part_1, 1);
    }
    else
    {
        THROW(message_parser_format_exception, "raw message is too small to set truncated flag");
    }
}
