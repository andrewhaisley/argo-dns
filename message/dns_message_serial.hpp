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
 
#pragma once

#include <unordered_map>
#include <string>
#include <string.h>

#include "types.hpp"
#include "dns_name.hpp"
#include "ip_address.hpp"
#include "buffer.hpp"
#include "dns_txt.hpp"
#include "dns_message_parser_exceptions.hpp"
#include "datetime.hpp"
#include "dns_type_bitmap.hpp"
#include "dns_svcparams.hpp"
#include "dns_ipseckey.hpp"
#include "eui48.hpp"
#include "eui64.hpp"

namespace adns
{
    class dns_message_serial
    {
    public:

        struct header_part_1
        {
            octet rd     : 1;
            octet tc     : 1;
            octet aa     : 1;
            octet opcode : 4;
            octet qr     : 1;
        };

        struct header_part_2
        {
            octet rcode  : 4;
            octet z      : 3;
            octet ra     : 1;
        };
        
        dns_message_serial();

        virtual ~dns_message_serial();

    protected:

        /**
         * Check if there's enough space in a buffer to copy some bytes in.
         * Throws exception on overrun.
         */
        void length_check(size_t size, size_t offset, size_t count) const;

        /**
         * Parses an unsigned short int from a raw message applying big-endian conversion as required. 
         * length_check is called to ensure no length overruns. offset is incremented by 2.
         */
        unsigned short parse_unsigned_short(const octet *raw, size_t size, size_t &offset) const;

        /**
         * Parses a short int from a raw message applying big-endian conversion as required. 
         * length_check is called to ensure no length overruns. offset is incremented by 2.
         */
        short parse_short(const octet *raw, size_t size, size_t &offset) const;

        /**
         * Parses an uint from a raw message applying big-endian conversion as required. 
         * length_check is called to ensure no length overruns. offset is incremented by 4.
         */
        uint parse_uint(const octet *raw, size_t size, size_t &offset) const;

        /**
         * Parses an int from a raw message applying big-endian conversion as required. 
         * length_check is called to ensure no length overruns. offset is incremented by 4.
         */
        int parse_int(const octet *raw, size_t size, size_t &offset) const;

        /**
         * Copies an int from a raw message applying big-endian conversion as required. 
         * length_check is called to ensure no length overruns. offset is incremented by 4.
         */
        void parse_uint(uint &target, const octet *raw, size_t size, size_t &offset) const;

        /**
         * Parses a single 8 bit byte from a raw message. 
         * length_check is called to ensure no length overruns. offset is incremented by 1.
         */
        octet parse_octet(const octet *raw, size_t size, size_t &offset) const;

        /**
         * Copies a single 8 bit byte from a raw message.
         * length_check is called to ensure no length overruns. offset is incremented by 1.
         */
        void parse_octet(void *target, const octet *raw, size_t size, size_t &offset) const;

        /**
         * Copies something from a raw message into a supplied target.
         * length_check is called to ensure no length overruns. offset is incremented by the length of the buffer.
         */
        void parse_data(octet *target, const octet *raw, size_t length, size_t size, size_t &offset) const;

        /**
         * Extract a name.
         */
        std::shared_ptr<dns_name> parse_name(const octet *raw, size_t size, size_t &offset);

        /**
         * Extract an IP4 address.
         */
        ip_address parse_ip4_address(const octet *raw, size_t size, size_t &offset);

        /**
         * Extract an IP6 address.
         */
        ip_address parse_ip6_address(const octet *raw, size_t size, size_t &offset);

        /**
         * Extract an EUI48 address.
         */
        eui48 parse_eui48(const octet *raw, size_t size, size_t &offset);

        /**
         * Extract an EUI64 address.
         */
        eui64 parse_eui64(const octet *raw, size_t size, size_t &offset);

        /**
         * Parse a date time.
         */
        datetime parse_datetime(const octet *raw, size_t size, size_t &offset);

        /**
         * Parse an rr type bitmap .
         */
        std::shared_ptr<dns_type_bitmap> parse_type_bitmap(const octet *raw, size_t size, size_t &offset);

        /**
         * extract some data as a buffer object
         */
        buffer parse_buffer(const octet *raw, uint buffer_length, size_t size, size_t &offset);

        /**
         * parse a txt format record
         */
        std::shared_ptr<dns_txt> parse_txt(const octet *raw, size_t size, size_t &offset);

        /**
         * parse a single string (length + data)
         */
        std::shared_ptr<dns_label> parse_string(const octet *raw, size_t size, size_t &offset);

        /**
         * parse whetever is left of the raw data into a buffer
         */
        buffer parse_remaining_buffer(const octet *raw, size_t size, size_t &offset);

        /**
         * parse whetever is left of the raw data into as string
         */
        std::string parse_remaining_string(const octet *raw, size_t size, size_t &offset);

        /**
         * parse svcparams (HPPTS & SVCB records)
         */
        std::shared_ptr<dns_svcparams> parse_svcparams(const octet *raw, size_t size, size_t &offset);

        /**
         * parse ipseckey (IPSEC record type)
         */
        std::shared_ptr<dns_ipseckey> parse_ipseckey(const octet *raw, size_t size, size_t &offset);

        /**
         * Copies an unsigned short int into a raw message.
         * length_check is called to ensure no length overruns. offset is incremented by 2.
         */
        void unparse_unsigned_short(unsigned short s, octet *target, size_t size, size_t &offset) const;

        /**
         * Copies a short int into a raw message.
         * length_check is called to ensure no length overruns. offset is incremented by 2.
         */
        void unparse_short(short s, octet *target, size_t &offset) const;

        /**
         * Copies an uint into a raw message.
         * length_check is called to ensure no length overruns. offset is incremented by 4.
         */
        void unparse_uint(uint i, octet *target, size_t size, size_t &offset) const;

        /**
         * Copies an uint into a raw message.
         * length_check is called to ensure no length overruns. offset is incremented by 4.
         */
        void unparse_int(int i, octet *target, size_t size, size_t &offset) const;

        /**
         * Copies a single 8 bit byte into a raw message.
         * length_check is called to ensure no length overruns. offset is incremented by 1.
         */
        void unparse_octet(octet *target, const void *o, size_t size, size_t &offset) const;

        /**
         * Copies a single 8 bit byte into a raw message.
         * length_check is called to ensure no length overruns. offset is incremented by 1.
         */
        void unparse_octet(octet o, octet *target, size_t size, size_t &offset) const;

        /**
         * Copies a buffer into a raw message.
         * length_check is called to ensure no length overruns. offset is incremented by the length of the buffer.
         */
        void unparse_data(octet *target, const void *o, size_t length, size_t size, size_t &offset) const;

        /**
         * Write a name into the given raw buffer.
         */
        void unparse_name(
                const dns_name &n, 
                octet *target, 
                std::unordered_map<dns_name, unsigned short> &name_offsets, 
                size_t size, 
                size_t &offset,
                bool compress = true);

        /**
         * Unparse an IP4 address.
         */
        void unparse_ip4_address(const ip_address &addr, octet *target, size_t size, size_t &offset);

        /**
         * Unparse an IP6 address.
         */
        void unparse_ip6_address(const ip_address &addr, octet *target, size_t size, size_t &offset);

        /**
         * Unparse an EUI48 address.
         */
        void unparse_eui48(const eui48 &addr, octet *target, size_t size, size_t &offset);

        /**
         * Unparse an EUI64 address.
         */
        void unparse_eui64(const eui64 &addr, octet *target, size_t size, size_t &offset);

        /**
         * Unparse a datetime.
         */
        void unparse_datetime(const datetime &dt, octet *target, size_t size, size_t &offset);

        /**
         * Unparse a type_bitmap.
         */
        void unparse_type_bitmap(const std::shared_ptr<dns_type_bitmap> &tb, octet *target, size_t size, size_t &offset);

        /**
         * unparse a buffer object
         */
        void unparse_buffer(const buffer &data, octet *target, size_t size, size_t &offset);

        /**
         * unparse a txt format record
         */
        void unparse_txt(const std::shared_ptr<dns_txt> &txt, octet *target, size_t size, size_t &offset);

        /**
         * unparse a single string with a length octet
         */
        void unparse_string(const std::shared_ptr<dns_label> &string, octet *target, size_t size, size_t &offset);

        /**
         * unparse a buffer, without a length count, into the raw data
         */
        void unparse_remaining_buffer(const buffer &b, octet *target, size_t size, size_t &offset);

        /**
         * unparse a string, without a length count, into the raw data
         */
        void unparse_remaining_string(const std::string &b, octet *target, size_t size, size_t &offset);

        /**
         * unparse svcparams (HPPTS & SVCB records) into the raw data
         */
       void unparse_svcparams(const std::shared_ptr<dns_svcparams> &params, octet *target, size_t size, size_t &offset);

        /**
         * unparse ipseckey (IPSECKEY record) into the raw data
         */
       void unparse_ipseckey(const std::shared_ptr<dns_ipseckey> &params, octet *target, size_t size, size_t &offset);

    };
}
