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
