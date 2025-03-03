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
