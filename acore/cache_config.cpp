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
