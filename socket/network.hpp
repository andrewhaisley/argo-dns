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

#include "types.hpp"
#include "exception.hpp"
#include "ip_address.hpp"
#include "json_serializable.hpp"

EXCEPTION_CLASS(network_exception, exception)

namespace adns
{
    /**
     * IP networks
     */
    class network : public json_serializable
    {
    public:

        /**
         * invalid network
         */
        network();
    
        /**
         * new network from an IP address and a count. e.g. 128.1.1.1/8
         */
        network(const ip_address &ip, uint count);

        /**
         * new network from a string in CIDR format
         */
        network(const std::string &cidr_string);
    
        /**
         * destructor
         */
        virtual ~network();

        /**
         * is an address inside a network?
         */
        bool contains(const ip_address &ip) const;

        /**
         * network type - ip4 or ip6
         */
        ip_address::ip_type_t get_type() const;

        /**
         * initliase the bitmask array
         */
        static void init();

        /**
         * string representation
         */
        std::string to_string() const;

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

        ip_address  m_ip;
        uint        m_count;
        static uint o_bitmask[32];

        /**
         * validate that the count matches the ip address type
         */
        void validate();

        /**
         * figure out if a network defined by an ip and a bit count contains
         * another ip (ipv4)
         */
        bool contains(const struct in_addr &network, uint count, const struct in_addr &ip) const;

        /**
         * figure out if a network defined by an ip and a bit count contains
         * another ip (ipv6)
         */
        bool contains(const struct in6_addr &network, uint count, const struct in6_addr &ip) const;

        /**
         * initialise from a CIDR string
         */
        void init(const std::string &cidr_string);

    };

    std::ostream &operator<<(std::ostream& stream, const network &n);
}
