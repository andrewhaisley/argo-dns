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

#include <mutex>
#include <unordered_map>
#include <list>

#include "types.hpp"
#include "exception.hpp"
#include "dns_zone.hpp"
#include "dns_rr.hpp"
#include "dns_question.hpp"
#include "connection.hpp"

EXCEPTION_CLASS(rr_dal_exception, exception)

namespace adns
{
    /**
     * A database access layer to read/write resource records
     */
    class dns_rr_dal final
    {
    public:

        // number of resource record types we know about
        static int NUM_RESOURCE_RECORD_TYPES;

        // array of resource record types
        static dns_rr::type_t rr_types[];

        /**
         * no instances
         */
        dns_rr_dal() = delete;

        /**
         * read resource records of the given type for a zone from the DB
         */
        static void read_resource_records(uuid zid, dns_rr::type_t t, std::list<std::shared_ptr<dns_rr>> &res);

        /**
         * delete resource records of the given type for a zone from the DB
         */
        static void delete_resource_records(db::connection &conn, uuid zid, dns_rr::type_t t);

        /**
         * delete a single resource record from the DB
         */
        static void delete_resource_record(db::connection &conn, const std::shared_ptr<dns_rr> &rr);

        /**
         * insert a DNS record into the DB
         */
        static void insert_resource_record(db::connection &conn, const std::shared_ptr<dns_rr> &rr);

        /**
         * update a DNS record in the DB
         */
        static void update_resource_record(db::connection &conn, const std::shared_ptr<dns_rr> &rr);
    };
}
