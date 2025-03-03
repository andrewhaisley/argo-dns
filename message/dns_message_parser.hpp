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
#pragma once

#include <string.h>

#include <unordered_map>
#include <list>
#include <string>
#include <memory>

#include "dns_message.hpp"
#include "dns_message_serial.hpp"
#include "dns_rr.hpp"
#include "dns_rr_OPT.hpp"
#include "dns_rr_parser.hpp"
#include "dns_name.hpp"
#include "types.hpp"
#include "buffer.hpp"
#include "exception.hpp"
#include "dns_message_parser_exceptions.hpp"

namespace adns
{
    /**
     * A class for parsing and unparsing DNS messages. That is, for converting them from the
     * on-the-wire format into a instance of dns_message and back again.
     */
    class dns_message_parser final : public dns_message_serial
    {
    public:

        /**
         * create a parser ready to convert raw messages to and from the on the wire format into
         * dns_message instances.
         */
        dns_message_parser();

        /**
         * destructor - clean up any cached data
         */
        virtual ~dns_message_parser();
    
        /**
         * Convert a single message from the on the wire format. Throws dns_message_parser::exception in the case
         * of trouble.
         */
        dns_message *from_wire(const buffer &raw);

        /**
         * Convert a single message into the wire format. Throws exception in the case
         * of trouble. 
         */
        buffer to_wire(const dns_message &m, size_t size = MAX_EXTENDED_MESSAGE_SIZE);

        /**
         * Convert a single message into the wire format. Throws exception in the case
         * of trouble. 
         */
        buffer to_wire(const dns_message &m, bool compress, size_t size);

        /**
         * extract just the message ID - return 0 if the message is too short.
         */
        uint extract_id(const buffer &raw) noexcept;

        /**
         * set just the message ID - do nothing if the message is too short
         */
        void set_id(const buffer &raw, uint id) noexcept;

        /**
         * extract just the message opcode - return op_query_e if the message is too short.
         */
        dns_message::op_code_t extract_opcode(const buffer &raw) noexcept;

        /**
         * set the truncated flag in the header part of a message without
         * parsing/unparsing.
         */
        void set_truncated(const buffer &raw, bool truncated);

    private:

        // resource record parser instance
        dns_rr_parser m_rr_parser;

        /**
         * Parse a single question from raw data. offset is incremented by the length of
         * the question.
         */
        std::shared_ptr<dns_question> parse_question(const octet *raw, size_t size, size_t &offset);

        /**
         * update the message to reflect the EDNS options contained in an OPT record
         */
        void parse_edns_options(dns_message *m, dns_rr_OPT *rr);

        /**
         * Write a question into the given raw buffer.
         */
        void unparse_question(
                octet *raw, 
                std::unordered_map<dns_name, unsigned short> &name_offsets, 
                const dns_question &q, 
                size_t size, 
                size_t &offset,
                bool   compress = true);

    };
}
