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
