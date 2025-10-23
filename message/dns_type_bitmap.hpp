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
#include "dns_rr.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(dns_type_bitmap_exception, exception)

namespace adns
{
    /**
     * RRTYPE bitmap used by NSEC records
     */
    class dns_type_bitmap : public json_serializable
    {
    public:

        /**
         * empty bitmap
         */
        dns_type_bitmap();

        /**
         * constructor from a json string of form ["TYPE1", "TYPE2", etc]
         */
        dns_type_bitmap(const std::string &s);

        /**
         * constructor from a json array
         */
        dns_type_bitmap(const json &j);

        /** 
         * destructor
         */
        virtual ~dns_type_bitmap();

        /**
         * add a type
         */
        void add(dns_rr::type_t t);

        /**
         * string representation
         */
        std::string to_string() const;

        /**
         * get the types
         */
        const std::set<dns_rr::type_t> &get_types() const;

    private:

        std::set<dns_rr::type_t> m_types;

        /**
         * Turn the instance into JSON
         */
        virtual void json_serialize() const;

        /**
         * Re-initialize the instance from json.
         */
        virtual void json_unserialize();
    };

    std::ostream &operator<<(std::ostream& stream, const dns_type_bitmap &t);
}
