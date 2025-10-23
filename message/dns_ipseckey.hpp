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
