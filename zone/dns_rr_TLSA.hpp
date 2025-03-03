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

/**
 * automatically generated by rrgen.py at 05:05PM on January 30, 2024 do not edit by hand.
 */

#include <boost/lexical_cast.hpp>

#include "dns_rr.hpp"

namespace adns
{
    /**
     * DNS resource record type TLSA (type number 52)
     */
    class dns_rr_TLSA final : public dns_rr
    {
    public:

        /**
         * Constructor 
         */
        dns_rr_TLSA();

        /**
         * Destructor 
         */
        virtual ~dns_rr_TLSA();

        /**
         * make a copy 
         */
        virtual dns_rr *clone();

        /**
         * set methods
         */
        void set_certifcate_usage(octet certifcate_usage);
        void set_selector(octet selector);
        void set_matching_type(octet matching_type);
        void set_certificate_association(const buffer &certificate_association);

        /**
         * get methods
         */
        octet get_certifcate_usage() const;
        octet get_selector() const;
        octet get_matching_type() const;
        const buffer &get_certificate_association() const;

        /**
         * dump out for debug
         */
        virtual void dump() const;

        /**
         * turn into a string for logging/debug/comparison
         */
        virtual std::string to_string() const;

        /**
         * turn into a string for logging/debug/comparison with no TTL
         */
        virtual std::string to_string_no_ttl() const;

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

        octet m_certifcate_usage;
        octet m_selector;
        octet m_matching_type;
        buffer m_certificate_association;

    };
}
