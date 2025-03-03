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

#include <boost/property_tree/ptree.hpp>

#include "types.hpp"
#include "config_reader.hpp"
#include "server_config.hpp"
#include "nullable.hpp"

EXCEPTION_CLASS(config_exception, exception)
EXCEPTION_CLASS(config_addresses_exception, exception)

namespace adns
{
    /**
     * Configuration values for servers.
     */
    class config final
    {
    public:

        /**
         * Get a list of servers to be run.
         */
        static std::list<server_config> get_servers();

        /**
         * Add a new server - will be picked up on restart
         */
        static void add_server(const server_config &sc);

        /**
         * Change a server - changes will be picked up on restart
         */
        static void update_server(const server_config &sc);

        /**
         * Delete a server - changes will be picked up on restart
         */
        static void delete_server(const server_config &sc);

        /**
         * Get the recursive cache config.
         */
        static cache_config get_cache_config();

        /**
         * Name of the default from which to take the source of configuration values.
         */
        static const char *default_bootstrap_file;

        /**
         * Initialise logging
         */
        static void init_log(std::string bootstrap_config_filename);

        /**
         * Cache config values from the DB.
         */
        static void init_config();

        /**
         * Should the server be run as a dameon or not? Non-daemon is primarily for dev and debug.
         */
        static bool daemon(std::string bootstrap_config_filename);

        /**
         * user id to run as - null implies the current user
         */
        static nullable<int> run_as_user_id();

        /**
         * group id to run as - null implies the current user
         */
        static nullable<int> run_as_group_id();

        /**
         * Place to chroot to - null implies don't chroot
         */
        static nullable<std::string> chroot_dir();

        /**
         * file holding server SSL certificate chain file
         */
        static std::string ssl_server_chain_file();

        /**
         * file holding server SSL private key file
         */
        static std::string ssl_server_key_file();

        /**
         * username for server configuration
         */
        static std::string server_config_username();

        /**
         * password for server configuration
         */
        static std::string server_config_password();

        /**
         * do we support EDNS?
         */
        static bool support_edns();

        /**
         * if we support EDNS, what's the packet size
         */
        static short unsigned edns_size();

        /**
         * should we use ipv4 upstream for updating the root hints?
         */
        static uint use_ip4_root_hints();

        /**
         * should we use ipv6 upstream for updating the root hints?
         */
        static uint use_ip6_root_hints();

        /**
         * Initialise DB connection from json file ready to read actual server config
         */
        static void init_db_config();

        /**
         * Get the base config only as a JSON structure
         */
        static json base_config_to_json();

        /**
         * Update the base config only from a JSON structure
         */
        static void set_base_config_from_json(const json &j);

        /**
         * Get the cache config only as a JSON structure
         */
        static json cache_config_to_json();

        /**
         * Update the cache config only from a JSON structure
         */
        static void set_cache_config_from_json(const json &j);

    private:

        /**
         * No need to instantiate, use the static methods.
         */
        config();

        /**
         * translate a password - which may be plaintext or scrambled - into an actual password
         * that can be used.
         */
        static std::string translate_password(const std::string &password);

        /**
         * extract a config value from the property tree throwing an exception if the value
         * wasn't found.
         */
        static std::string get_tree_string(const std::string &name, const std::string &exception_text);

        /**
         * Turn a string representing a server type into the equivalent enum value.
         */
        static server_config::server_type_t string_to_server_type(const std::string &s);

        /**
         * Helper to read the actual config values from the defined source type
         */
        static config_reader o_config_reader;

        /**
         * boost property tree
         */
        static boost::property_tree::ptree o_tree;

        /**
         * check if two lists of socket addresses overlap
         */
        static bool socket_addresses_overlap(
                const std::list<server_config::socket_address_t> &sal1,
                const std::list<server_config::socket_address_t> &sal2);
    };
}
