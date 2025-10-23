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

#include <thread>
#include <chrono>

#include "types.hpp"

#include "server_config.hpp"
#include "message_queue.hpp"
#include "dns_message_envelope.hpp"
#include "dns_recursive_cache.hpp"
#include "dns_recursive_slot_manager.hpp"
#include "dns_parallel_client.hpp"

EXCEPTION_CLASS(recursive_slot_exception, exception)

namespace adns
{
    class dns_recursive_slot
    {
    public:

        struct params_t
        {
            params_t(const server_config &config, dns_recursive_slot_manager &rsm);

            server_config              m_config;
            dns_recursive_slot_manager &m_slot_manager;
        };

        struct query_t
        {
            query_t(const dns_question &q, const ip_address &ip);

            bool operator<(const query_t &other) const;

            dns_question m_question;
            ip_address   m_ip;
        };

        /**
         * constructor - processes updates as part of a handler pool
         */
        dns_recursive_slot(const params_t &params, message_queue<dns_message_envelope *> &queue);

        /** 
         * destructor
         */
        virtual ~dns_recursive_slot();

        /**
         * wait for any running threads to join
         */
        void join();

        /**
         * resolve a single query
         */
        void process(dns_message_envelope *m);

        /**
         * send a response to any waiters on the slot
         */
        virtual void send_response(dns_message_envelope *e);


    private:

        typedef enum
        {
            data_e,
            additional_data_e,
            no_data_e,
            nxdomain_e,
            fail_e,
            referral_e,
            cname_e,
            dname_e
        }
        answer_type_t;

        // parameters - e.g. the slot manager
        params_t m_params;

        // outgoing message queue 
        message_queue<dns_message_envelope *> &m_out_queue;

        // time the resolution attempt started
        std::chrono::steady_clock::time_point m_start_time;

        // top level query for this slot
        dns_message_envelope *m_top_level_query;

        // places we've already asked the question - used for loop checking
        std::set<query_t> m_query_trail;

        // client connection
        dns_parallel_client m_client;

        // shared cache
        std::shared_ptr<dns_recursive_cache> m_cache;

        /**
         * recursively resolve a query
         */
        std::shared_ptr<const dns_recursive_cache_answer> resolve(
                        dns_message_envelope *m,
                        uint                 depth = 0);

        /**
         * check to see if we've exceded the max time allowed for resolution and
         * throw an exception if so
         */
        void check_timeout(const dns_message_envelope *m);

        /**
         * check to see if we're about to go around a loop
         */
        void check_loop(const dns_question &q, std::set<ip_address> &ip_addresses);

        /**
         * query a nameserver for an answer - automatically caching as we go
         */
        std::shared_ptr<const dns_recursive_cache_answer> query_nameserver(
                            dns_message_envelope                                    *m,
                            const std::shared_ptr<const dns_recursive_cache_answer> &cs,
                            uint                                                    depth);

        // does an answer have glue records?
        bool has_glue_records(
                            const std::shared_ptr<dns_rr>                           &ns, 
                            const std::shared_ptr<const dns_recursive_cache_answer> &cs);

        /**
         * Get a list of nameservers ordered by likely fastest response. The ones
         * with glue will always be first as the others may require a full resolution
         * in their own right.
         */
        void get_ordered_nameserver_list(
                            std::list<std::shared_ptr<dns_rr>>                      &ordered_ns_with_glue,
                            std::list<std::shared_ptr<dns_rr>>                      &ordered_ns_without_glue,
                            const std::shared_ptr<const dns_recursive_cache_answer> &cs);

        // get a list of glue IP addresses from an answer
        void get_glue_ips(
                    std::set<ip_address>                                    &ip_addresses,
                    std::shared_ptr<dns_rr>                                 &ns, 
                    const std::shared_ptr<const dns_recursive_cache_answer> &cs);

        // recursively resolve A & AAAA records for a nameserver
        void resolve_nameserver_ips(
                    std::set<ip_address>            &ip_addresses, 
                    const std::shared_ptr<dns_name> &n,
                    uint                            depth);

        // recursively resolve a CNAME
        std::shared_ptr<const dns_recursive_cache_answer> query_nameserver_cname(
                            dns_message_envelope                                    *m,
                            const std::shared_ptr<const dns_recursive_cache_answer> &ans,
                            uint                                                    depth);

        // recursively resolve a DNAME
        std::shared_ptr<const dns_recursive_cache_answer> query_nameserver_dname(
                            dns_message_envelope                                    *m,
                            const std::shared_ptr<const dns_recursive_cache_answer> &ans,
                            uint                                                    depth);

        // query a set of IP addresses of nameservers in parallel
        std::shared_ptr<const dns_recursive_cache_answer> query_nameserver_ips(
                            const std::shared_ptr<const dns_recursive_cache_answer> &referred_by,
                            dns_message_envelope       *m,
                            const std::set<ip_address> &ip_addresses,
                            uint                       depth);

    
        /**
         * respond to a request with an answer containing resource records
         */
        void respond_with_answer(
                    dns_message_envelope                                    *e,
                    const std::shared_ptr<const dns_recursive_cache_answer> &answer);

        /**
         * respond to a request with an error response 
         */
        void respond_with_error(dns_message_envelope *m, dns_message::response_code_t rc);

        /**
         * respond to a request with a no data response 
         */
        void respond_with_no_data(
                    dns_message_envelope                                    *e, 
                    const std::shared_ptr<const dns_recursive_cache_answer> &answer);

        /**
         * respond to a request with an nxdomain response 
         */
        void respond_with_nxdomain(
                    dns_message_envelope                                    *e,
                    const std::shared_ptr<const dns_recursive_cache_answer> &answer);

        /**
         * figure out what a response means wrt to a given question
         */
        answer_type_t classify_response(
                                const std::shared_ptr<const dns_recursive_cache_answer> &referred_by,
                                const std::shared_ptr<dns_question> &q,
                                const dns_message                   *r) const;

    
        /**
         * construct a basic response with the main header fields filled in
         */
        dns_message *construct_base_response(dns_message_envelope *e);

        /**
         * does a message have any answers matching a geiven question?
         */
        bool has_answers_matching_question(
                        const std::shared_ptr<dns_question> &q,
                        const dns_message                   *r) const;

        /**
         * does a message have any additional records matching a given question?
         */
        bool has_additional_records_matching_question(
                        const std::shared_ptr<dns_question> &q,
                        const dns_message                   *r) const;

        /**
         * Map the type of a response to the type of something that can be cached
         */
        dns_recursive_cache_answer::type_t map_answer_type(answer_type_t t) const;
    };
}
