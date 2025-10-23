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

#include "types.hpp"
#include "dns_zone.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(dns_zone_validator_exception, exception)

namespace adns
{
    /**
     * validator for zone structures. Makes sure flags are set correctly and match resource
     * records (where present).
     */
    class dns_zone_validator final
    {
    public:

        /**
         * no instantiation
         */
        dns_zone_validator() = delete;

        /**
         * validate a full zone insert
         */
        static void validate_insert(const dns_zone &z);

        /**
         * validate a zone update
         */
        static void validate_update(const dns_zone &z);

        /**
         * validate the insert of a resource record into a zone
         */
        static void validate_insert_record(const dns_zone &z, const std::shared_ptr<dns_rr> &rr);

        /**
         * validate the update of a resource record in a zone
         */
        static void validate_update_record(const dns_zone &z, const std::shared_ptr<dns_rr> &rr);

        /**
         * validate the deletion of a resource record from a zone
         */
        static void validate_delete_record(const dns_zone &z, const std::shared_ptr<dns_rr> &rr);

    };
}
