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

#include <unordered_map>
#include <vector>

#include "types.hpp"
#include "json.hpp"
#include "dns_rr.hpp"
#include "exception.hpp"
#include "dns_message_serial.hpp"
#include "dns_message_parser_exceptions.hpp"

namespace adns
{
    /**
     * parser for resource records
     */
    class dns_rr_parser final : public dns_message_serial
    {
    public:

        /**
         * constructor
         */
        dns_rr_parser();

        /**
         * destructor
         */
        virtual ~dns_rr_parser();

        /**
         * Parse a single resource record from raw data. offset is incremented by the length of
         * the record.
         */
        std::shared_ptr<dns_rr> parse(const octet *raw, size_t size, size_t &offset);

        /**
         * unparse a single resource record
         */
        void unparse(octet *raw, std::unordered_map<dns_name, unsigned short> &name_offsets, const dns_rr &r, size_t size, size_t &offset, bool compress = true);

        /**
         * turn a JSON representation into a resource record
         */
        std::shared_ptr<dns_rr> from_json(json j);

    private:

        /**
         * create a new instance of the appropriate dns_rr derived class for a given record type
         */
        std::shared_ptr<dns_rr> instantiate(dns_rr::type_t t);

        /**
         * parse the contents of the resource record type specific data into the
         * relevant member variables
         */
        void parse(const std::shared_ptr<dns_rr> &rr, unsigned short rr_length, const octet *raw, size_t size, size_t &offset);

        /**
         * unparse the contents of the resource record type specific data into the
         * relevant member variables
         */
        void unparse(const dns_rr &rr, octet *raw, std::unordered_map<dns_name, unsigned short> &name_offsets, size_t size, size_t &offset, bool compress = true);

    };
}
