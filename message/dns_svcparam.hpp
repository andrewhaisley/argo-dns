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
#include "buffer.hpp"
#include "ip_address.hpp"
#include "json_serializable.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(dns_svcparam_exception, exception)

namespace adns
{
    class dns_svcparam : public json_serializable
    {
    public:

        friend class dns_message_serial;

        /**
         * Service param types
         */
        typedef enum
        {
            mandatory_e = 0,
            alpn_e = 1,
            no_default_alpn_e = 2,
            port_e = 3,
            ipv4hint_e = 4,
            ech_e = 5,
            ipv6hint_e = 6,
            invalid_e = 1000
        }
        svcparam_type_t;

        /**
         * invalid param
         */
        dns_svcparam();

        /**
         * param of given type - initialized as appropriate 
         */
        dns_svcparam(svcparam_type_t t);

        /**
         * destructor
         */
        virtual ~dns_svcparam();

        /**
         * Does the param value represent a valid svcparam type?
         */
        static bool valid_type(unsigned short s);

        /**
         * string name of the type
         */
        static std::string type_to_string(svcparam_type_t t);

        /**
         * type from name 
         */
        static svcparam_type_t string_to_type(const std::string &name);

        /**
         * get the type of the instance 
         */
        svcparam_type_t get_type() const;

        /**
         * for debug
         */
        std::string to_string() const;

    protected:

        svcparam_type_t m_type;

        std::list<std::string> m_alpns;
        std::list<ip_address> m_ipv4_addrs;
        std::list<ip_address> m_ipv6_addrs;
        std::list<svcparam_type_t> m_mandatory;
        unsigned short m_port;
        buffer m_ech;

        /**
         * Turn the instance into JSON
         */
        virtual void json_serialize() const;

        /**
         * Re-initialize the instance from json.
         */
        virtual void json_unserialize();

    };
}
