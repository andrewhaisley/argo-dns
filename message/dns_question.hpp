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

#include <string>
#include <list>

#include "dns_name.hpp"
#include "dns_rr.hpp"
#include "types.hpp"

namespace adns
{
    class dns_question final
    {
    public:

        friend class dns_message_parser;
        friend class dns_message_unparser;

        // query types extra to requests for specific resource record types
        typedef enum
        {
            QT_RR_e   =   0,    // Request for a specific resource record type
            QT_IXFR_e =   251,  // Diffs from prior zone transfer
            QT_AXFR_e =   252,  // A request for a transfer of an entire zone
            QT_ALL_e  =   255   // A request for all records
        }
        qtype_t;

        // query classes - we only support IN or ANY
        typedef enum
        {
            QC_IN_e  = 1,
            QC_ANY_e = 255
        }
        qclass_t;

        /**
         * Constructor invalid instance
         */
        dns_question();

        /**
         * Copy constructor - shallow copy
         */
        dns_question(const dns_question &other);

        /**
         * Constructor
         */
        dns_question(const dns_name &qname, qtype_t qtype = QT_RR_e, dns_rr::type_t rr_type = dns_rr::T_NONE_e, qclass_t qc = QC_IN_e);

        /**
         * Destructor
         */
        virtual ~dns_question();

        /**
         * assignment - shallow copy
         */
        const dns_question &operator=(const dns_question &other);

        /**
         * equal if all elements are equal
         */
        bool operator==(const dns_question &other) const;

        /**
         * not equal if any elements are not equal
         */
        bool operator!=(const dns_question &other) const;

        /**
         * < operator
         */
        bool operator<(const dns_question &other) const;

        /**
         * Get the list of names being queried.
         */
        const dns_name &get_qname() const;

        /**
         * Get the query type
         */
        qtype_t get_qtype() const;

        /**
         * Get the query class
         */
        qclass_t get_qclass() const;

        /**
         * Get the resource record type being queried
         */
        dns_rr::type_t get_rr_type() const ;

        /**
         * Set the query type
         */
        void set_qtype(qtype_t qt);

        /**
         * hash for use in unordered_map
         */
        size_t get_hash() const;

        /**
         * to string for logging
         */
        std::string to_string() const;

    private:

        dns_name m_qname;

        qtype_t         m_qtype;
        qclass_t        m_qclass;
        dns_rr::type_t  m_rr_type;

        // hash value
        mutable size_t m_hash;
    };

    std::ostream &operator<<(std::ostream& stream, const dns_question &q);
}

/**
 * add a template specialisation so that dns_question can be the key in a hash map
 * without the need to specify an explict hash class
 */
namespace std 
{
    template <> struct hash<adns::dns_question>
    {
        size_t operator()(const adns::dns_question &k) const
        {
            return k.get_hash();
        }
    };
}
