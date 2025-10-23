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
 
#include "cache_config.hpp"
#include "exception.hpp"

using namespace std;
using namespace adns;

void cache_config::json_serialize() const
{
    set_json("default_ttl", int(default_ttl));
    set_json("cache_garbage_collect_ms", int(cache_garbage_collect_ms));
    set_json("cache_max_referral_rrs", int(cache_max_referral_rrs));
    set_json("cache_max_answer_rrs", int(cache_max_answer_rrs));

    set_json("client", client.to_json());
}

void cache_config::json_unserialize()
{
    default_ttl = int((*m_json_object)["default_ttl"]);
    cache_garbage_collect_ms = int((*m_json_object)["cache_garbage_collect_ms"]);
    cache_max_referral_rrs = int((*m_json_object)["cache_max_referral_rrs"]);
    cache_max_answer_rrs = int((*m_json_object)["cache_max_answer_rrs"]);

    client.from_json((*m_json_object)["client"]);
}
