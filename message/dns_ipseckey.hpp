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

#include <set>

#include "types.hpp"
#include "json_serializable.hpp"
#include "exception.hpp"
#include "ip_address.hpp"
#include "buffer.hpp"
#include "dns_name.hpp"

EXCEPTION_CLASS(dns_ipseckey_exception, exception)

namespace adns
{
    /**
     * RRTYPE bitmap used by NSEC records
     */
    class dns_ipseckey : public json_serializable
    {
    public:

        friend class dns_message_serial;

        typedef enum
        {
            none_e = 0,
            ipv4_e = 1,
            ipv6_e = 2,
            name_e = 3
        }
        gateway_type_t;

        /**
         * empty bitmap
         */
        dns_ipseckey();

        /**
         * constructor from a json string
         */
        dns_ipseckey(const std::string &s);

        /**
         * constructor from a json object
         */
        dns_ipseckey(const json &j);

        /** 
         * destructor
         */
        virtual ~dns_ipseckey();

        /**
         * string representation
         */
        std::string to_string() const;

    private:

        gateway_type_t m_gateway_type;
        octet m_precedence;
        octet m_algorithm;
        ip_address m_ipv4_gateway;
        ip_address m_ipv6_gateway;
        dns_name m_name_gateway;
        buffer m_public_key;

        /**
         * Turn the instance into JSON
         */
        virtual void json_serialize() const;

        /**
         * Re-initialize the instance from json.
         */
        virtual void json_unserialize();
    };

    std::ostream &operator<<(std::ostream& stream, const dns_ipseckey &t);
}
