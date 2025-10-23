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
#include <boost/functional/hash.hpp>

#include "types.hpp"
#include "exception.hpp"
#include "connection.hpp"
#include "dns_zone.hpp"
#include "dns_rr.hpp"
#include "dns_rr_SOA.hpp"
#include "dns_rr_NS.hpp"
#include "dns_rr_CNAME.hpp"
#include "dns_rr_DNAME.hpp"

EXCEPTION_CLASS(rr_cache_exception, exception)

namespace adns
{
    /**
     * An incremental caching reader of resource records
     */
    class dns_rr_cache final
    {
    public:

        /**
         * new (empty) rr cache
         */
        dns_rr_cache();

        /**
         * destructor
         */
        virtual ~dns_rr_cache();

        /**
         * get all records for a zone (no exceptions)
         */
        std::list<std::shared_ptr<dns_rr>> get_resource_records(const dns_zone &zone, bool shuffle = false);

        /**
         * get a specific records from a zone 
         * return null if the rr doesn't exist
         */
        std::shared_ptr<dns_rr> get_resource_record(const dns_zone &zone, uuid id);
        
        /**
         * Get all records for the name except for DNSSEC related ones
         */
        std::list<std::shared_ptr<dns_rr>> get_resource_records(const dns_zone &zone, const dns_name &name, bool shuffle = false);

        /**
         * Get all records for the name of a given type
         */
        std::list<std::shared_ptr<dns_rr>> get_resource_records(const dns_zone &zone, const dns_name &name, dns_rr::type_t t, bool shuffle = false);
        /**
         * get all records filtered by name/type. DNSSEC included, no shuffling.
         * @param   IN  name - null -> all names
         * @param   IN  type - null -> all types
         */
        std::list<std::shared_ptr<dns_rr>> get_resource_records(
                                        const dns_zone                 &zone,
                                        const nullable<dns_name>       &name, 
                                        const nullable<dns_rr::type_t> type);

        /**
         * Get the SOA record for a zone
         */
        const std::shared_ptr<dns_rr_SOA> &get_SOA(const dns_zone &zone);

        /**
         * Get the NS records for a zone
         */
        const std::list<std::shared_ptr<dns_rr_NS>> &get_NS(const dns_zone &zone);

        /**
         * Get the delegated NS records for a name (if any)
         */
        std::list<std::shared_ptr<dns_rr_NS>> get_delegated_NS(const dns_zone &zone, const dns_name &name);

        /**
         * Get the matching CNAME record for a name (if any)
         */
        std::shared_ptr<dns_rr_CNAME> get_CNAME(const dns_zone &zone, const dns_name &name);

        /**
         * Get the matching DNAME record for a name (if any)
         */
        std::shared_ptr<dns_rr_DNAME> get_DNAME(const dns_zone &zone, const dns_name &name);

        /**
         * insert a single record into the DB
         */
        uuid insert_record_db(db::connection &conn, const std::shared_ptr<dns_rr> &rr);

        /**
         * insert a single record into the cache
         */
        void insert_record_cache(const std::shared_ptr<dns_rr> &rr);

        /**
         * update a single record in the DB
         */
        void update_record_db(db::connection &conn, const std::shared_ptr<dns_rr> &rr);

        /**
         * update a single record in the cache
         */
        void update_record_cache(const std::shared_ptr<dns_rr> &rr);

        /**
         * delete a single record from the DB
         */
        void delete_record_db(db::connection &conn, const std::shared_ptr<dns_rr> &rr);

        /**
         * delete a single record from the cache
         */
        void delete_record_cache(const std::shared_ptr<dns_rr> &rr);

        /**
         * delete all records for a zone from the DB
         */
        void delete_records_db(db::connection &conn, const dns_zone &zone);

        /**
         * delete all records for a zone from the cache
         */
        void delete_records_cache(const dns_zone &zone);

    private:

        // map of zone id to a map of resource record type id to a list of actual resource records
        // an empty list is a negative cache indicator 
        std::unordered_map<uuid, std::map<dns_rr::type_t, std::list<std::shared_ptr<dns_rr>>>, boost::hash<boost::uuids::uuid>> m_cache;

        // map of zone id to SOA
        std::unordered_map<uuid, std::shared_ptr<dns_rr_SOA>, boost::hash<boost::uuids::uuid>> m_SOA_cache;

        // map of zone id to NS records
        std::unordered_map<uuid, std::list<std::shared_ptr<dns_rr_NS>>, boost::hash<boost::uuids::uuid>> m_NS_cache;

        // delegated NS records
        std::unordered_map<uuid, std::map<dns_name, std::list<std::shared_ptr<dns_rr_NS>>>, boost::hash<boost::uuids::uuid>> m_delegated_NS_cache;

        // CNAME records
        std::unordered_map<uuid, std::map<dns_name, std::shared_ptr<dns_rr_CNAME>>, boost::hash<boost::uuids::uuid>> m_CNAME_cache;

        // DNAME records
        std::unordered_map<uuid, std::map<dns_name, std::shared_ptr<dns_rr_DNAME>>, boost::hash<boost::uuids::uuid>> m_DNAME_cache;
        
        // lock for whole cache
        std::mutex m_lock;

        /**
         * read records of given type for a zone from the DB if needed
         */
        void read_records(const dns_zone &zone, dns_rr::type_t t);

        /**
         * Shuffle a list of records - or not.
         */
        void shuffle_list(std::list<std::shared_ptr<dns_rr>> &rrs, bool shuffle);

    };
}
