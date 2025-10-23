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

#include <list>
#include <algorithm>

#include "dns_question.hpp"
#include "dns_rr.hpp"
#include "types.hpp"
#include "buffer.hpp"

#define STANDARD_MESSAGE_SIZE 512
#define MAX_EXTENDED_MESSAGE_SIZE 65536
#define MIN_POSSIBLE_MESSAGE_SIZE 12
#define HEADER_SIZE 12
#define MAX_DOMAIN_NAME_LENGTH 253
#define MAX_DOMAIN_NAME_COMPONENTS 128
#define EDNS_VERSION 0

EXCEPTION_CLASS(message_exception, exception)

namespace adns
{
    /**
     * A class to hold DNS messages. The messages can be queries or responses.
     * In the case of messages received by the server, the friend class
     * dns_parser is used to convert the on the wire format into a dns_message instance.
     */
    class dns_message final
    {
    public:

        friend class dns_message_parser;
        friend class dns_message_unparser;

        /**
         * Whether the message is a query or response. This could have been
         * implemented by creating derived classes from a base message class
         * but there doesn't so far seem to be much to be gained by that.
         */
        typedef enum
        {
            query_e     = 0,
            response_e  = 1
        }
        type_t;

        /**
         * OPCODE - see RFC1035 4.1.1
         */
        typedef enum
        {
            op_query_e  = 0,
            op_inverse_query_e = 1,
            op_status_e = 2,
            op_notify_e = 4,
            op_update_e = 5
        }
        op_code_t;

        /**
         * RCODE - see RFC1035 4.1.1
         */
        typedef enum
        {
            no_error_e          = 0,
            format_error_e      = 1,
            server_failure_e    = 2,
            name_error_e        = 3,
            not_implemented_e   = 4,
            refused_e           = 5,
            yxdomain_e          = 6, 
            yxrrset_e           = 7,
            nxrrset_e           = 8, 
            notauth_e           = 9, 
            notzone_e           = 10, 
            reserved11_e        = 11,
            reserved12_e        = 12, 
            reserved13_e        = 13, 
            reserved14_e        = 14, 
            reserved15_e        = 15,
            badvers_e           = 16,
            badsig_e            = 16,
            badkey_e            = 17,
            badtime_e           = 18,
            badmode_e           = 19,
            badname_e           = 20,
            badalg_e            = 21,
            badtrunc_e          = 22
        }
        response_code_t;

        /**
         * Constructs and empty (and therefore invalid) message. 
         */
        dns_message();

        /**
         * Virtual destructor to avoid later confusion in the case of
         * derived classes.
         */
        virtual ~dns_message();

        /**
         * request or response? It might be necessary to rework this with derived
         * classes at some point, should this one grow too much.
         */
        type_t get_type() const;

        /**
         * OPCODE for request messages - see RFC1035 4.1.1
         */
        op_code_t get_op_code() const;

        /**
         * RCODE for response messages - see RFC1035 4.1.1
         */
        response_code_t get_response_code() const;

        /**
         * ID- see RFC1035 4.1.1
         */
        unsigned short int get_id() const;

        /**
         * AA - see RFC1035 4.1.1
         */
        bool get_is_authoritative() const;

        /**
         * TC - see RFC1035 4.1.1
         */
        bool get_is_truncated() const;

        /**
         * RD - see RFC1035 4.1.1
         */
        bool get_is_recursion_desired() const;

        /**
         * RA - see RFC1035 4.1.1
         */
        bool get_is_recursion_available() const;

        /**
         * EDNS?
         */
        bool get_is_edns() const;

        /**
         * EDNS version
         */
        octet get_edns_version() const;

        /**
         * EDNS payload size
         */
        unsigned short get_edns_payload_size() const;

        /**
         * DNSSEC OK?
         */
        bool get_is_dnssec_ok() const;

        /**
         * ANCOUNT - see RFC1035 4.1.1
         */
        unsigned short get_an_count() const;

        /**
         * NSCOUNT - see RFC1035 4.1.1
         */
        unsigned short get_ns_count() const;

        /**
         * ARCOUNT - see RFC1035 4.1.1
         */
        unsigned short get_ar_count() const;

        /**
         * Get the question from a request message.
         */
        const std::shared_ptr<dns_question> &get_question() const;

        /**
         * setter methods
         */
        void set_id(unsigned short int id);
        void set_type(type_t t);
        void set_op_code(op_code_t oc);
        void set_response_code(response_code_t rc);
        void set_is_authoritative(bool b);
        void set_is_truncated(bool b);
        void set_is_recursion_desired(bool b);
        void set_is_recursion_available(bool b);
        void set_edns(bool b, unsigned short payload_size);
        void set_question(const std::shared_ptr<dns_question> &q);

        template<class RR> void add_answer(const std::shared_ptr<RR> &a)
        {
            m_answers.push_back(a);
            m_an_count++;
            update_min_ttl(a);
        }

        template<class RR> void add_first_answer(const std::shared_ptr<RR> &a)
        {
            m_answers.push_front(a);
            m_an_count++;
            update_min_ttl(a);
        }

        template<class RR> void add_last_answer(const std::shared_ptr<RR> &a)
        {
            m_answers.push_back(a);
            m_an_count++;
            update_min_ttl(a);
        }

        template<class RR> void add_answers(const std::list<std::shared_ptr<RR>> &a)
        {
            m_answers.insert(m_answers.end(), a.begin(), a.end());
            m_an_count += a.size();
            update_min_ttl(a);
        }

        template<class RR> void add_nameserver(const std::shared_ptr<RR> &ns)
        {
            m_nameservers.push_back(ns);
            m_ns_count++;
            update_min_ttl(ns);
        }

        template<class RR> void add_nameservers(const std::list<std::shared_ptr<RR>> &ns)
        {
            m_nameservers.insert(m_nameservers.end(), ns.begin(), ns.end());
            m_ns_count += ns.size();
            update_min_ttl(ns);
        }

        template<class RR> void add_additional_record(const std::shared_ptr<RR> &a)
        {
            m_additional_records.push_back(a);
            m_ar_count++;
            update_min_ttl(a);
        }

        template<class RR> void add_additional_records(const std::list<std::shared_ptr<RR>> &a)
        {
            m_additional_records.insert(m_additional_records.end(), a.begin(), a.end());
            m_ar_count += a.size();
            update_min_ttl(a);
        }

        /**
         * get methods
         */
        const std::list<std::shared_ptr<dns_rr>> &get_answers() const;
        const std::list<std::shared_ptr<dns_rr>> &get_nameservers() const;
        const std::list<std::shared_ptr<dns_rr>> &get_additional_records() const;

        /**
         * number of answers etc.
         */
        size_t get_num_answers() const;
        size_t get_num_nameservers() const;
        size_t get_num_additional_records() const;

        /**
         * get first record of given type
         */
        std::shared_ptr<dns_rr> get_first_answer(dns_rr::type_t t) const;

        /**
         * get all records of given type
         */
        std::list<std::shared_ptr<dns_rr>> get_answers(dns_rr::type_t t) const;

        /**
         * get count of given record type
         */
        size_t get_num_answers(dns_rr::type_t t) const;
        size_t get_num_nameservers(dns_rr::type_t t) const;
        size_t get_num_additional_records(dns_rr::type_t t) const;

        /**
         * true on existance of at least one of the given record type
         */
        bool answer_exists(dns_rr::type_t t) const;
        bool nameserver_exists(dns_rr::type_t t) const;
        bool additional_record_exists(dns_rr::type_t t) const;

        /**
         * get the minimum TTL for any of the contained records
         */
        int get_min_ttl() const;

        /**
         * Debug dump via boost info log messages.
         */
        void dump() const;

    private:

        unsigned short m_id;
        unsigned short m_payload_size;
        type_t m_type;
        op_code_t m_op_code;
        bool m_is_authoritative;
        bool m_is_truncated;
        bool m_is_recursion_desired;
        bool m_is_recursion_available;
        bool m_is_edns;
        bool m_is_dnssec_ok;
        octet m_edns_version;
        response_code_t m_response_code;
        unsigned short m_an_count;
        unsigned short m_ns_count;
        unsigned short m_ar_count;
        int m_min_ttl;

        std::shared_ptr<dns_question> m_question;

        std::list<std::shared_ptr<dns_rr>> m_answers;
        std::list<std::shared_ptr<dns_rr>> m_nameservers;
        std::list<std::shared_ptr<dns_rr>> m_additional_records;

        /*
         * update the min ttl value for the message based on the contents of the supplied record
         */
        template<class RR> void update_min_ttl(const std::shared_ptr<RR> &r)
        {
            if (m_min_ttl == -1)
            {
                m_min_ttl = r->get_ttl();
            }
            else
            {
                m_min_ttl = std::min(m_min_ttl, (int)r->get_ttl());
            }
        }

        /*
         * update the min ttl value for the message based on the contents of the supplied record
         */
        template<class RR> void update_min_ttl(const std::list<std::shared_ptr<RR>> &rl)
        {
            for (auto r : rl)
            {
                update_min_ttl(r);
            }
        }

        /**
         * get the number of records of a given type in a list
         */
        size_t get_num_records(dns_rr::type_t t, const std::list<std::shared_ptr<dns_rr>> &rrs) const;

        /**
         * true if at least one record of the given type exists in the list, false otherwise
         */
        bool record_exists(dns_rr::type_t t, const std::list<std::shared_ptr<dns_rr>> &rrs) const;
    };
}
