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
