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
