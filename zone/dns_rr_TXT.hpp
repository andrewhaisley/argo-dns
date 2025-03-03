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
     * DNS resource record type TXT (type number 16)
     */
    class dns_rr_TXT final : public dns_rr
    {
    public:

        /**
         * Constructor 
         */
        dns_rr_TXT();

        /**
         * Destructor 
         */
        virtual ~dns_rr_TXT();

        /**
         * make a copy 
         */
        virtual dns_rr *clone();

        /**
         * set methods
         */
        void set_txt(const std::shared_ptr<dns_txt> &txt);

        /**
         * get methods
         */
        const std::shared_ptr<dns_txt> &get_txt() const;

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

        std::shared_ptr<dns_txt> m_txt;

    };
}
