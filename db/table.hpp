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
/**
 * autogenerated by dbgen.py at 03:09PM on March 22, 2025, do not hand edit
 */

#pragma once

#include <map>
#include <list>
#include <string>

#include "types.hpp"
#include "connection.hpp"

namespace adns
{
    namespace db
    {
        /**
         * db table metadata
         */
        class table
        {
        public:

            typedef enum
            {
                schema_version_e,
                address_list_e,
                address_list_address_e,
                address_list_network_e,
                basic_config_e,
                cache_config_e,
                control_server_e,
                ui_server_e,
                dns_client_e,
                dns_server_e,
                horizon_e,
                root_server_e,
                server_e,
                server_socket_address_e,
                zone_e,
                rr_history_e,
                rr_a_e,
                rr_aaaa_e,
                rr_afsdb_e,
                rr_caa_e,
                rr_cert_e,
                rr_cname_e,
                rr_csync_e,
                rr_dhcid_e,
                rr_dname_e,
                rr_eui48_e,
                rr_eui64_e,
                rr_generic_e,
                rr_hinfo_e,
                rr_https_e,
                rr_ipseckey_e,
                rr_kx_e,
                rr_loc_e,
                rr_mx_e,
                rr_naptr_e,
                rr_ns_e,
                rr_openpgpkey_e,
                rr_ptr_e,
                rr_soa_e,
                rr_srv_e,
                rr_sshfp_e,
                rr_svcb_e,
                rr_tlsa_e,
                rr_txt_e,
                rr_uri_e,
                rr_zonemd_e
            }
            table_t;

            // get a list of SQL strings to create DB tables for the relevant DB type (null op for some
            // DB types - e.g. MongoDB).
            static std::list<std::pair<table_t, std::string>> get_table_sql(type_t);

            // get a list of SQL strings to create DB indexes for the relevant DB type (null op for some
            // DB types - e.g. MongoDB).
            static std::list<std::pair<table_t, std::list<std::string>>> get_index_sql(type_t);

            // get a list of SQL strings to create DB foreign keys for the relevant DB type (null op for some
            // DB types - e.g. MongoDB).
            static std::list<std::pair<table_t, std::list<std::string>>> get_foreign_key_sql(type_t);

            // create static data needed for a base install 
            static void create_static(connection &conn);

        private:

            static std::map<type_t, std::list<std::pair<table_t, std::string>>> o_table_sql;
            static std::map<type_t, std::list<std::pair<table_t, std::list<std::string>>>> o_index_sql;
            static std::map<type_t, std::list<std::pair<table_t, std::list<std::string>>>> o_fk_sql;
        };
    }
}
