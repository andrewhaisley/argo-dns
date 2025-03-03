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

#include <chrono>
#include <ctime>
#include <mutex>
#include <atomic>
#include <set>

#include "dns_rr.hpp"
#include "types.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(dns_recursive_cache_answer_exception, exception)

namespace adns
{
    class dns_recursive_cache_answer final
    {
    public:

        typedef enum 
        {
            data_e,     // got a positive result (one or more records)
            no_data_e,  // positive response with no records
            nxdomain_e, // got a negative result - i.e. record known not to exist rather than we've not yet
                        // looked for it
            fail_e,     // server fail

            referral_e // no result cached, you'll need to go recurse starting with
                        // the supplied nameserver(s) to get one
        }
        type_t;

        /**
         * constructor
         */
        dns_recursive_cache_answer(
            type_t t, 
            uint   default_ttl,
            const std::list<std::shared_ptr<dns_rr>> &answer_records     = std::list<std::shared_ptr<dns_rr>>(),
            const std::list<std::shared_ptr<dns_rr>> &ns_records         = std::list<std::shared_ptr<dns_rr>>(),
            const std::list<std::shared_ptr<dns_rr>> &additional_records = std::list<std::shared_ptr<dns_rr>>());

        /**
         * copy constructor
         */
        dns_recursive_cache_answer(const dns_recursive_cache_answer &other);

        // get methods
        type_t get_type() const;
        std::list<std::shared_ptr<dns_rr>> get_answer_records() const;
        std::list<std::shared_ptr<dns_rr>> get_nameserver_records() const;
        std::list<std::shared_ptr<dns_rr>> get_additional_records() const;

        std::list<std::shared_ptr<dns_rr>> get_answer_records(dns_rr::type_t t) const;

        // add extra answers
        void add_answer_records(const std::list<std::shared_ptr<dns_rr>> &rrs);

        /** 
         * get the first record of the given type from the answer section of the answer
         */
        std::shared_ptr<dns_rr> get_first_answer_record(dns_rr::type_t t) const;

        /** 
         * Does the answer have a record of the given type?
         */
        bool has_answer_record(dns_rr::type_t t) const;

        /** 
         * get the SOA record from the nameserver section of the answer
         */
        std::shared_ptr<dns_rr> get_soa_record() const;

        /**
         * get the NS records from the nameservers section
         */
        std::list<std::shared_ptr<dns_rr>> get_nameserver_NS_records() const;

        /**
         * update the TTLs for all of the records in the answer based on the time now and
         * when it was first cached.
         */
        void update_ttls() const;

        /**
         * set the TTL of all records to a given value. Only needed when setting root
         * hints.
         */
        void set_ttls(uint ttl) const;

        /**
         * has the answer timed out? (i.e. TTLs <= 0)
         */
        bool expired() const;

        /**
         * get the minimum ttl of all the records in the answer
         */
        uint get_min_ttl() const;

        /**
         * get the type as a string - debug or log
         */
        std::string get_type_as_string() const;

        /**
         * get the domain that's being referred to in the case of a referral answer
         */
        const dns_name &get_referral_name() const;

        /**
         * type to string - debug or log
         */
        static std::string get_type_as_string(type_t t);

        /**     
         * get the creation sequence number - used during garbage collection to find the oldest entries
         */
        size_t get_sequence() const;

        /**     
         * how many records in total in the answer? - used during garbage collection as a proxy for memory use
         */
        uint get_num_records() const;

        /**
         * dump - for debug
         */
        void dump() const;

    private:

        static std::atomic<size_t> o_sequence;

        size_t             m_sequence;
        uint               m_num_records;
        mutable std::mutex m_lock;
        type_t             m_type;
        dns_name           m_referral_name;

        // string representation of existing answer RRs (no TTL)
        std::set<std::string> m_answer_rr_strings;

        mutable std::chrono::time_point<std::chrono::steady_clock> m_create_time;

        // current minimum TTL value for all of the records in the answer
        mutable std::chrono::duration<double> m_min_ttl;

        // records for the answer
        std::list<std::shared_ptr<dns_rr>> m_answer_records;
        std::list<std::shared_ptr<dns_rr>> m_ns_records;
        std::list<std::shared_ptr<dns_rr>> m_additional_records;

        /**
         * go through a set of records updating the minimum TTL
         */
        void update_min_ttl(bool &min_set, const std::list<std::shared_ptr<dns_rr>> &rrs) const;
    };
}
