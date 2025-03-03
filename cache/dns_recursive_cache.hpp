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
#include <set>
#include <map>
#include <thread>

#include "types.hpp"
#include "dns_rr.hpp"
#include "dns_message.hpp"
#include "exception.hpp"
#include "cache_config.hpp"
#include "dns_recursive_cache_answer.hpp"
#include "read_write_lock.hpp"

EXCEPTION_CLASS(dns_recursive_cache_exception, exception)

namespace adns
{
    /**
     * A cache of resource records and answers to specific queries.
     */
    class dns_recursive_cache
    {
    public:

        /**
         * Initialise at startup - create the singleton etc.
         */
        static void init(const cache_config &c);

        /**
         * get the one and only instance
         */
        static std::shared_ptr<dns_recursive_cache> get_instance();

        /**
         * destruction
         */
        virtual ~dns_recursive_cache();

        /**
         * given a question, get the answer
         */
        std::shared_ptr<const dns_recursive_cache_answer> get_answer(
                            const std::shared_ptr<dns_question> &question, 
                            bool                                referral_requested);

        /**
         * get the nearest nameserver referal for a question that can't be answered from the cache.
         */
        std::shared_ptr<const dns_recursive_cache_answer> get_referral(const std::shared_ptr<dns_question> &question) const;

        /**
         * add an answer to the cache
         */
        void add_answer(
                    const std::shared_ptr<dns_question>                     &question, 
                    const std::shared_ptr<const dns_recursive_cache_answer> &answer);

        /**
         * add a referral to the cache
         */
        void add_referral(const dns_name &qname, const std::shared_ptr<const dns_recursive_cache_answer> &answer);

        /**
         * get the default ttl for answers without records
         */
        uint get_default_ttl() const;

        /**
         * Join running threads.
         */
        void join();

        /**
         * dump the cache contents for debug
         */
        void dump() const;

        /**
         * get a list of referral names with a record count - used for monitoring
         */
        std::map<dns_name, uint> get_referral_name_record_counts();

    private:

        // lock for the answer cache
        mutable read_write_lock m_answer_lock;

        // query/answer data
        std::unordered_map<dns_question, std::shared_ptr<const dns_recursive_cache_answer>> m_answers;

        // lock for the referral cache
        mutable read_write_lock m_referral_lock;

        // referral to elsewhere if the answer wasn't found
        std::unordered_map<dns_name, std::shared_ptr<const dns_recursive_cache_answer>> m_referrals;

        // list of referral entries - used for FIFO garbage collection
        std::list<std::pair<dns_name, std::shared_ptr<const dns_recursive_cache_answer>>> m_referral_list;

        // list of answer entries - used for FIFO garbage collection
        std::list<std::pair<dns_question, std::shared_ptr<const dns_recursive_cache_answer>>> m_answer_list;

        // garbage collection thread
        std::shared_ptr<std::thread> m_garbage_collector_thread;

        // root hint updater thread
        std::shared_ptr<std::thread> m_root_hint_updater_thread;

        // cache config (included client)
        cache_config m_config;

        // number of cached answer rrs - for monitoring
        uint m_num_answer_rrs;

        // number of cached referal rrs  - for monitoring
        uint m_num_referral_rrs;

        // ID of the monitor for number of cached answers
        uint m_monitor_answers_id;

        // ID of the monitor for number of cached referrals 
        uint m_monitor_referrals_id;

        // ID of the monitor for time of last garbage collect
        uint m_monitor_last_garbage_collect_id;

        /**
         * load root hints from DB
         */
        void load_root_hints();

        /**
         * new, empty cache
         */
        dns_recursive_cache(const cache_config &c);

        /**
         * figure out if an answer to a question is in the cache and, if so, has it expired.
         */
        bool answer_expired(const dns_question &q);

        /**
         * garbage collect cache entries
         */
        void garbage_collect();

        /**
         * update root hints if they're getting a bit old
         */
        void update_root_hints();

        /**
         * Using the ip addresses supplied, contact whichever root servers are
         * responding and update the list.
         */
        void update_root_hints_now(const std::set<ip_address> &ip_addresses);

        /**
         * Sanity check the response back from the root servers. Important so as to avoid
         * breaking the root hints completely.
         */
        bool validate_root_response(const dns_message *res);

        /**
         * Get relevant glue records from a cache answer for the root servers.
         */
        void get_glue_ips(
                std::set<ip_address>                                    &ip_addresses,
                const std::shared_ptr<const dns_recursive_cache_answer> &ca);

        /**
         * Garbage collect cached answers - delete anything with 0 TTL
         */
        void garbage_collect_answers();

        /**
         * Garbage collect cached referrals - delete anything with 0 TTL
         */
        void garbage_collect_referrals();
    };
}
