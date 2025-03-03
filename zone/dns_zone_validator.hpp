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
