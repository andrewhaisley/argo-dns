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
