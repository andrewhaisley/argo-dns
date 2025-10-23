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
#include <map>
#include <boost/functional/hash.hpp>

#include "types.hpp"
#include "exception.hpp"
#include "dns_name.hpp"
#include "ip_address.hpp"
#include "nullable.hpp"
#include "json.hpp"
#include "dns_horizon.hpp"
#include "dns_rr.hpp"
#include "dns_rr_SOA.hpp"
#include "dns_rr_NS.hpp"
#include "dns_rr_CNAME.hpp"
#include "dns_rr_DNAME.hpp"
#include "dns_question.hpp"
#include "handler_pool.hpp"
#include "connection.hpp"
#include "connection.hpp"

EXCEPTION_CLASS(zone_cache_exception, exception)
EXCEPTION_CLASS(dns_zone_exception, exception)

namespace adns
{
    class dns_zone final
    {
    public:

        // static methods to get zones from the cache

        /**
         * find a zone for which we're authoritative that matches to the greatest extent.
         * e.g. if we're authoritative for blah.com and y.blah.com then the result for
         * for x.y.blah.com would be y.blah.com.
         * returns nullptr when no match could be found
         */
        static std::shared_ptr<dns_zone> find(const uuid &horizon_id, const dns_name &name);

        /**
         * find a zone which matches the name exactly.
         * returns nullptr when no match could be found
         */
        static std::shared_ptr<dns_zone> find_exact(const uuid &horizon_id, const dns_name &name);

        /**
         * find all zones for the horizon.
         */
        static std::list<std::shared_ptr<dns_zone>> find_by_horizon(const uuid &horizon_id);

        /**
         * get all zones
         */
        static std::list<std::shared_ptr<dns_zone>> find();

        /**
         * get zone from id - nullptr if no match
         */
        static std::shared_ptr<dns_zone> find(uuid id);

        /**
         * default constructor - invalid empty zone
         */
        dns_zone();

        /**
         * make a copy of the zone that can be safely inserted into the cache
         */
        std::shared_ptr<dns_zone> clone() const;

        /**
         * destructor
         */
        virtual ~dns_zone();

        /**
         * get methods
         */
        uuid get_zone_id() const;
        uuid get_horizon_id() const;
        const std::shared_ptr<dns_name> &get_name() const;

        bool get_is_forwarded() const;
        const ip_address &get_forward_ip_address() const;
        int get_forward_port() const;

        /**
         * Get the SOA for the zone.
         */
        const std::shared_ptr<dns_rr_SOA> &get_SOA() const;

        /**
         * Get the first CNAME that matches the given name. More than one CNAME is
         * an error condition and will open happen if someone has inserted stuff
         * directly into the DB or there's a transactional integrity issue.
         */
        const std::shared_ptr<dns_rr_CNAME> get_CNAME(const dns_name &name) const;

        /**
         * Get the first CNAME that matches the given name. This is the longest match
         * available. 
         */
        const std::shared_ptr<dns_rr_DNAME> get_DNAME(const dns_name &name) const;

        /**
         * Get the nameservers for the zone (not delegated ones)
         */
        const std::list<std::shared_ptr<dns_rr_NS>> &get_NS() const;

        /**
         * Get any delegated nameservers for a name. E.g. if the name is x.y.z.blah.com and
         * we have a nameserver z.blah.com return that.
         */
        std::list<std::shared_ptr<dns_rr_NS>> get_delegated_NS(const dns_name &name) const;

        /**
         * get a specific records from a zone 
         * null if the record wasn't found in the zone
         */
        std::shared_ptr<dns_rr> get_resource_record(uuid id) const;

        /**
         * get all records for a zone (no exceptions thrown)
         */
        std::list<std::shared_ptr<dns_rr>> get_resource_records(bool shuffle = false) const;
        
        /**
         * Get all records for a specific name except for DNSSEC related ones
         */
        std::list<std::shared_ptr<dns_rr>> get_resource_records(const dns_name &name, bool shuffle = false) const;

        /**
         * Get all records for a specific name of a given type (DNSSEC included)
         */
        std::list<std::shared_ptr<dns_rr>> get_resource_records(
                                        const dns_name &name, 
                                        dns_rr::type_t t, 
                                        bool           shuffle = false) const;

        /**
         * get all records filtered by name/type. DNSSEC included, no shuffling.
         * @param   IN  name - null -> all names
         * @param   IN  type - null -> all types
         */
        std::list<std::shared_ptr<dns_rr>> get_resource_records(
                                        const nullable<dns_name>       &name, 
                                        const nullable<dns_rr::type_t> type) const;

        /**
         * Get all records as per the question with filtering as needed - wrapper around the previous two methods.
         */
        std::list<std::shared_ptr<dns_rr>> get_resource_records(const dns_question &q, bool shuffle = false) const;

        /**
         * turn the zone into a json structure
         */
        json to_json() const;

        /**
         * populate a zone from a json structure
         */
        void from_json(const json &j);

        /**
         * insert a new zone (deserialized from JSON in the API) into the DB
         * @return UUID of newly created zone
         */
        uuid insert_zone_db(db::connection &conn);

        /**
         * insert a new zone (deserialized from JSON in the API) into the cache
         */
        void insert_zone_cache();

        /**
         * update an existing zone (deserialized from JSON in the API) into the DB
         */
        void update_zone_db(db::connection &conn);

        /**
         * update a zone (deserialized from JSON in the API) in the cache
         */
        void update_zone_cache();

        /** 
         * delete the zone and any records from the DB
         */
        void delete_zone_db(db::connection &conn);

        /** 
         * delete the zone and any records from the cache
         */
        void delete_zone_cache();

        /** 
         * delete a list of resource records from the DB
         */
        void delete_records_db(db::connection &conn, const std::list<std::shared_ptr<dns_rr>> &rrs);

        /** 
         * delete a list of resource records from the cache
         */
        void delete_records_cache(const std::list<std::shared_ptr<dns_rr>> &rrs);

        /** 
         * delete a resource record from the DB
         */
        void delete_record_db(db::connection &conn, const std::shared_ptr<dns_rr> &rr);

       /** 
         * delete a resource record from the cache
         */
        void delete_record_cache(const std::shared_ptr<dns_rr> &rr);

        /** 
         * insert a resource record into the DB
         * @return UUID of newly inserted record
         */
        uuid insert_record_db(db::connection &conn, const std::shared_ptr<dns_rr> &rr);

       /** 
         * insert a resource record into the cache
         */
        void insert_record_cache(const std::shared_ptr<dns_rr> &rr);

        /** 
         * update a resource record in the DB
         */
        void update_record_db(db::connection &conn, const std::shared_ptr<dns_rr> &rr);

       /** 
         * update a resource record in the cache
         */
        void update_record_cache(const std::shared_ptr<dns_rr> &rr);

        /**
         * initialise caches if they're not already - must be called before the server goes multi-threaded
         */
        static void init();

        /**
         * get the list of pending (i.e. not inserted into the DB or cache) resource
         * records for the zone.
         */
        const std::list<std::shared_ptr<dns_rr>> &get_pending_resource_records() const;

        /**
         * dump for debug use
         */
        void dump() const;

    private:

        /**
         * a cache of dns_zone objects read from the DB
         */
        class dns_zone_cache final
        {
        public:

            /**
             * destructor
             */
            virtual ~dns_zone_cache();

            /**
             * initialised cache
             */
            dns_zone_cache(const uuid &horizon_id);

            /**
             * find a zone from its UUID
             *
             * returns nullptr when no match could be found
             */
            std::shared_ptr<dns_zone> find(uuid id);

            /**
             * find a zone. e.g. 'blah.com'. The longest match is returned. E.g.
             * we have zones:
             *    blah.com
             *    y.blah.com
             *    x.y.blah.com
             * find("blah.com") return blah.com
             * find("q.w.e.r.t.blah.com") return blah.com
             * find("y.blah.com") return y.blah.com
             * find("a.b.c.y.blah.com") return y.blah.com
             * find("a.b.x.y.blah.com") return x.y.blah.com
             *
             * returns nullptr when no match could be found
             */
            std::shared_ptr<dns_zone> find(const dns_name &name);

            /**
             * find a zone. Only an exact match is returned.
             *
             * returns nullptr when no match could be found
             */
            std::shared_ptr<dns_zone> find_exact(const dns_name &name);

            /**
             * find all zones.
             */
            std::list<std::shared_ptr<dns_zone>> find();

            /**
             * insert a zone into the cache
             */
            void insert_zone(const std::shared_ptr<dns_zone> &z);

            /**
             * delete a zone from the cache 
             */
            void delete_zone(const dns_zone &z);

        private:

            // map of all the zones by id
            std::unordered_map<uuid, std::shared_ptr<dns_zone>, boost::hash<boost::uuids::uuid>> m_zones_by_id;

            // map of all the zones by name
            std::unordered_map<dns_name, std::shared_ptr<dns_zone>> m_zones_by_name;

            // id of the horizon we're in
            uuid m_horizon_id;

            /**
             * load the zone data (not resource records though, they are read and cached as needed)
             */
            void read_data(const uuid &horizon_id);
        };

        // map of horizons -> dns zone caches. A zone is uniquely part of one horizon.
        static std::map<uuid, std::shared_ptr<dns_zone::dns_zone_cache>> o_zone_cache;

        // internal ID
        uuid m_zone_id;

        // horizon to which the zone belongs
        uuid m_horizon_id;

        // see get_ methods for descriptions
        bool m_is_forwarded;

        // name
        std::shared_ptr<dns_name> m_name;

        // forwarding details
        ip_address m_forward_ip_address;
        uint       m_forward_port;

        // temporary list of resource records pending update or insert of a new zone
        // instance into the DB and cache
        std::list<std::shared_ptr<dns_rr>> m_pending_resource_records;

        /**
         * constructor
         */
        dns_zone(
            uuid                             zone_id, 
            const std::shared_ptr<dns_name>  &name,
            uuid                             horizon_id, 
            bool                             is_forwarded,
            const nullable<ip_address>       &forward_ip_address,
            nullable<uint>                   forward_port);

    };
}
