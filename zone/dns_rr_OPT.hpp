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

#include "dns_rr.hpp"
#include "buffer.hpp"

namespace adns
{
    /**
     * OPT records aren't real in the sense that they contain protocol related information only
     * and are handled as a special case by the parsing code.
     */
    class dns_rr_OPT final : public dns_rr
    {
    public:

        /**
         * Constructor 
         */
        dns_rr_OPT();

        /**
         * Destructor 
         */
        virtual ~dns_rr_OPT();

        /**
         * set methods
         */
        void set_data(const buffer &data);

        /**
         * make a copy 
         */
        virtual dns_rr *clone();

        /**
         * get methods
         */
        const buffer &get_data() const;
        unsigned short get_payload_size() const;

        /**
         * dump out for debug
         */
        virtual void dump() const;

    private:

        /**
         * turn into JSON
         */
        void json_serialize() const;

        /**
         * build from JSON - does nothing in practise.
         */
        void json_unserialize();

        buffer m_data;

    };
}
