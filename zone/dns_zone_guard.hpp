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
#include "read_write_lock.hpp"

namespace adns
{
    /**
     * guard access to the zone/rr cache structure
     */
    class dns_zone_guard final
    {
    public:

        typedef enum
        {
            read_e,
            write_e
        }
        type_t;

        /**
         * construction locks the cache in the mode requested
         */
        dns_zone_guard(type_t t);

        /**
         * destruction unlocks the cache
         */
        virtual ~dns_zone_guard();

    private:

        type_t m_type;
        static read_write_lock o_lock;
    };
}
