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
     * DNS resource record type SOA (type number 6)
     */
    class dns_rr_SOA final : public dns_rr
    {
    public:

        /**
         * Constructor 
         */
        dns_rr_SOA();

        /**
         * Destructor 
         */
        virtual ~dns_rr_SOA();

        /**
         * make a copy 
         */
        virtual dns_rr *clone();

        /**
         * set methods
         */
        void set_mname(const std::shared_ptr<dns_name> &mname);
        void set_rname(const std::shared_ptr<dns_name> &rname);
        void set_serial(uint serial);
        void set_refresh(int refresh);
        void set_retry(int retry);
        void set_expire(int expire);
        void set_minimum(uint minimum);

        /**
         * get methods
         */
        const std::shared_ptr<dns_name> &get_mname() const;
        const std::shared_ptr<dns_name> &get_rname() const;
        uint get_serial() const;
        int get_refresh() const;
        int get_retry() const;
        int get_expire() const;
        uint get_minimum() const;

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

        std::shared_ptr<dns_name> m_mname;
        std::shared_ptr<dns_name> m_rname;
        uint m_serial;
        int m_refresh;
        int m_retry;
        int m_expire;
        uint m_minimum;

    };
}
