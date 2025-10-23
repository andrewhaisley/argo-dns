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

#include <arpa/inet.h>
#include <string>

#include "exception.hpp"
#include "json_serializable.hpp"

EXCEPTION_CLASS(ip_address_exception, exception)

namespace adns
{
    /**
     * IP4 & IP6 addresses
     */
    class ip_address : public json_serializable
    {
    public:

        friend class network;
        friend class socket;
        friend class socket_address;
        friend class dns_message_serial;

        typedef enum
        {
            none_e,
            ip_v4_e,
            ip_v6_e
        }
        ip_type_t;

        /**
         * Invalid ip address
         */
        ip_address();

        /**
         * Any IP address of the given type.
         */
        ip_address(ip_type_t t);

        /**
         * IP address from v4 or v6 string.
         */
        ip_address(const std::string &s);

        /**
         * destructor
         */
        virtual ~ip_address();

        /**
         * get the IP version for this address
         */
        ip_type_t get_type() const;

        /**
         * string representation
         */
        std::string to_string() const;

        /**
         * equality test - false if the types are different
         */
        bool operator==(const ip_address &other) const;
        bool operator!=(const ip_address &other) const;

        /**
         * less than test - ip4 addresses are always less than ip6 addresses
         */
        bool operator<(const ip_address &other) const;

        /**
         * is the ip address localhost?
         */
        bool is_localhost() const;

        /**
         * is the address 'any' (either for ipv4 or ipv6
         */
        bool is_any() const;

        /**
         * generate a hash for use in STL containers
         */
        size_t get_hash() const;

    protected:

        /**
         * serialize into a json object
         */
        virtual void json_serialize() const;

        /**
         * unserialize from a json object
         */
        virtual void json_unserialize();

    private:

        ip_type_t m_type;

        struct in6_addr m_in6_addr;
        struct in_addr  m_in_addr;

        /**
         * constructor from a standard V4 address structure. Only
         * used by other members of the socket group of classes
         */
        ip_address(struct in_addr a);

        /**
         * constructor from a standard V6 address structure. Only
         * used by other members of the socket group of classes
         */
        ip_address(struct in6_addr a);

        /**
         * initialise from a string - can be an IP4 or IP6 string
         */
        void init(const std::string &s);
    };

    std::ostream &operator<<(std::ostream& stream, const ip_address &addr);
}

/**
 * add a template specialisation so that dns_name can be the key in a hash map
 * without the need to specify an explict hash class
 */
namespace std 
{
    template <> struct hash<adns::ip_address>
    {
        size_t operator()(const adns::ip_address &k) const
        {
            return k.get_hash();
        }
    };
}
