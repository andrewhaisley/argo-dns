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

#include <map>
#include <regex>

#include "types.hpp"
#include "nullable.hpp"
#include "url.hpp"
#include "dns_rr.hpp"

namespace adns
{
    /**
     * URLS as used by the API v1. All are of the following two forms:
     * 
     * 1/resource
     * 1/resource/$resource_id
     *
     * With the following optional query parameters:
     *    ?horizon=$horizon_id
     *    ?zone=$zone_id
     *    ?name=$name_string_regex      (DNS name that is)
     *    ?type=$rr_type                (A, AAAA etc)
     *
     */
    class api_url_v1 : public url
    {
    public:

        typedef enum
        {
            monitor_e,
            run_e,
            server_e,
            horizon_e,
            zone_e,
            rr_e,
            address_list_e,
            base_configuration_e,
            cache_configuration_e,
            none_e
        }
        resource_type_t;

        /**
         * default - empty 
         */
        api_url_v1();

        /**
         * construct from a raw api_url_v1 string from a URL
         */
        api_url_v1(const std::string &p);

        /**
         * destructor
         */
        virtual ~api_url_v1();

        /**
         * get the API version from the api_url_v1
         */
        uint get_version() const;

        /**
         * get the type of the resource being referenced
         */
        resource_type_t get_resource_type() const;
    
        /**
         * get the resource ID 
         * @return null if one wasn't given
         */
        nullable<uuid> get_resource_id() const;

        /**
         * get the ID of a resource filter (if any)
         */
        nullable<uuid> get_resource_filter_id(resource_type_t t) const;

        /**
         * get the DNS name filter regex (if any)
         */
        nullable<std::regex> get_name_filter() const;

        /**
         * get the record type filter (if any)
         */
        nullable<dns_rr::type_t> get_record_type_filter() const;

    private:

        uint            m_version;
        resource_type_t m_type;
        nullable<uuid>  m_resource_id;

        std::map<resource_type_t, uuid> m_filter_uuids;

        nullable<std::regex>     m_name_filter;
        nullable<dns_rr::type_t> m_record_type_filter;

        /**
         * extract the API version, resource type and resources from the api_url_v1 
         */
        void extract_version_type_and_resources();

        /**
         * extra all filters that represent resource IDs
         */
        void extract_resource_filters();

        /**
         * turn a resource type string into a type
         */
        resource_type_t string_to_type(std::string type) const;

    };
}
