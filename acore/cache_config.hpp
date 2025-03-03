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

#include <string>
#include <list>

#include "types.hpp"
#include "exception.hpp"
#include "json_serializable.hpp"
#include "client_config.hpp"

EXCEPTION_CLASS(cache_config_exception, exception)

namespace adns
{
    class cache_config : public json_serializable
    {
    public:
    
        friend class config_reader;

        // default TTL answers with no records (e.g. servfail)
        uint default_ttl;

        // how many ms to wait between cache garbage collect operations
        uint cache_garbage_collect_ms;

        // maximum number of referals stored in the cache
        uint cache_max_referral_rrs;

        // maximum number of answers stored in the cache
        uint cache_max_answer_rrs;

        // base config for the DNS client used by the cache for updating root hints
        client_config client;

    protected:

        // ID of DB row, not shared outside here since there is only
        // ever one row
        uuid cache_config_id;

        /**
         * Turn into a json object
         */
        virtual void json_serialize() const;

        /**
         * Populated from json object
         */
        virtual void json_unserialize();

    };
}
