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
#include "server_config.hpp"
#include "exception.hpp"
#include "socket_address.hpp"
#include "handler_pool.hpp"

#include "dns_message.hpp"
#include "dns_zone.hpp"
#include "dns_horizon.hpp"

EXCEPTION_CLASS(auth_resolver_exception, exception)
EXCEPTION_CLASS(auth_resolver_not_impl_exception, auth_resolver_exception)
EXCEPTION_CLASS(auth_resolver_fail_exception, auth_resolver_exception)

namespace adns
{
    /**
     * authoritative DNS resolver. Provides synchronous responses to requests. 
     */
    class dns_auth_resolver final
    {
    public:

        /**
         * get the one and only instance
         */
        static std::shared_ptr<dns_auth_resolver> get_instance(const server_config &server_config);

        /**
         * destructor
         */
        virtual ~dns_auth_resolver();

        /**
         * synchronously resolve a single request
         */
        dns_message *resolve(
                        const dns_horizon               &h,
                        dns_message                     *request, 
                        const std::shared_ptr<dns_zone> &zone);

    private:

        /**
         * default constructor
         */
        dns_auth_resolver(const server_config &server_config);

        /**
         * get some answers to a question 
         *  zone - where to get the resource records from
         *  q - question to answer
         */
        std::list<std::shared_ptr<dns_rr>> get_answers(const dns_zone &zone, const dns_question &q);

        /**
         * overwrite the names in a list of resource records.
         *  records - resource records
         *  name - name to place in the record
         */
        std::list<std::shared_ptr<dns_rr>> overwrite_names(const std::list<std::shared_ptr<dns_rr>> &records, const dns_name &name);

        /**
         * get glue records for the list of nameservers, if any. Don't include records already in
         * the answer list.
         */
        template<class RR> std::list<std::shared_ptr<dns_rr>> get_glue(
                    const dns_horizon                        &h,
                    const std::list<std::shared_ptr<dns_rr>> &answers,
                    const std::list<std::shared_ptr<RR>>     &ns_records);

        /**
         * Find the list of records that don't also appear in the list of answers
         */
        template<class RR> std::list<std::shared_ptr<dns_rr>> filter_records(
                    const std::list<std::shared_ptr<dns_rr>> &answers,
                    const std::list<std::shared_ptr<RR>>     &records);

        /**
         * figure out if the name is part of a delegated subdomain in the given
         * zone (which has an SOA record) and return the namserver records for it.
         */
        std::list<std::shared_ptr<dns_rr>> get_delegated_domain_nameservers(
                    const dns_horizon  &h,
                    const dns_zone     &zone, 
                    const dns_name     &name, 
                    const dns_rr       &soa);

        /**
         * create a basic response message based on the request being handled.
         */
        dns_message *create_response(dns_message *request);

        /**
         * resolve a request for the SOA record - something of a special case
         */
        dns_message *resolve_soa(const dns_horizon &h, dns_message *request);

        /**
         * resolve a request for NS records - another special case 
         */
        dns_message *resolve_ns(const dns_horizon &h, dns_message *request);

        /**
         * resolve a request for all records - something of a special case
         */
        dns_message *resolve_all(const dns_horizon &h, dns_message *request);

        /**
         * Given a query name, traverse any CNAME/DNAME pointer chain that's there
         * to find the name that actually needs to be resolved.
         * @param   h
         * @param   qname               - the name being queried
         * @param   loop_found          - set to true if a loop of CNAMES/DNAMES was found
         * @param   pointer_records     - set to the list of DNAMES and CNAMES (including those
         *                                derived from DNAMES) found.
         * @return  the actual name to be resolved (same as qname if no CNAMES/DNAMES found).
         */
        dns_name traverse_pointer_chain(
                            const dns_horizon                  &h, 
                            const dns_name                     &qname, 
                            bool                               &loop_found, 
                            std::list<std::shared_ptr<dns_rr>> &pointer_records);

        /**
         * apply a DNAME to another name
         */
        std::shared_ptr<dns_name> apply_dname(
                            const dns_name &name, 
                            const dns_name &dname_from, 
                            const dns_name &dname_to);
    };
}
