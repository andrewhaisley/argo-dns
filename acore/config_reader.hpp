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

#include <boost/property_tree/ptree.hpp>

#include <string>
#include <list>
#include <vector>
#include <map>

#include "types.hpp"
#include "connection.hpp"
#include "server_config.hpp"
#include "cache_config.hpp"
#include "row_server.hpp"
#include "row_dns_client.hpp"
#include "row_dns_server.hpp"
#include "row_control_server.hpp"
#include "row_ui_server.hpp"
#include "row_server_socket_address.hpp"

EXCEPTION_CLASS(config_reader_exception, exception)

namespace adns
{
    /**
     * Class to read server configuration from a variety of sources. A bootstrap configuration
     * structure is passed in that defines the source for the main configuration which is stored
     * in the DB.
     */
    class config_reader final
    {
    public:

        typedef enum
        {
            server_e = 1
        }
        table_t;

        /**
         * Constructor 
         */
        config_reader();

        /**
         * Destructor.
         */
        virtual ~config_reader();

        /**
         * read the config from the DB
         */
        void read();

        /**
         * Get a named scalar boolean config value. If the value isn't found in the config, then
         * default_value is returned.
         */
        bool get_bool(const std::string &name, bool default_value);

        /**
         * Get a named scalar integer config value. If the value isn't found in the config, then
         * default_value is returned.
         */
        int get_int(const std::string &name, int default_value);

        /**
         * Get a named scalar uinteger config value. If the value isn't found in the config, then
         * default_value is returned.
         */
        uint get_uint(const std::string &name, uint default_value);

        /**
         * Get a named scalar string config value. If the value isn't found in the config, then
         * default_value is returned.
         */
        std::string get_string(const std::string &name, const std::string &default_value);

        /**
         * Set a named string config value in the DB
         */
        void set_string_db(db::connection &conn, const std::string &name, const std::string &value);

        /**
         * Set a named uint config value in the DB
         */
        void set_uint_db(db::connection &conn, const std::string &name, uint value);

        /**
         * Set a named bool config value in the DB
         */
        void set_bool_db(db::connection &conn, const std::string &name, bool value);

        /**
         * Set a named string config value in the cache
         */
        void set_string_cache(const std::string &name, const std::string &value);

        /**
         * Set a named uint config value in the DB
         */
        void set_uint_cache(const std::string &name, uint value);

        /**
         * Set a named bool config value in the DB
         */
        void set_bool_cache(const std::string &name, bool value);

        /**
         * Get the list of server configurations.
         */
        std::list<server_config> servers();

        /**
         * Add a new server - will be picked up on restart
         */
        void add_server(server_config sc);

        /**
         * Change a server - changes will be picked up on restart
         */
        void update_server(const server_config &sc);

        /**
         * Delete a server - changes will be picked up on restart
         */
        void delete_server(const server_config &sc);

        /**
         * Get the (single) cache configuration
         */
        cache_config get_cache_config();

        /**
         * Update the (single) cache configuration - changes will be picked up on restart
         */
        void update_cache_config(const cache_config &c);

    private:

        // lock for all operations on any member vars that aren't atomic - only relevant 
        // for the rest APIs as running servers all take their own copies of their 
        // configs at startup
        std::mutex m_lock;

        /**
         * Scalar config values read from the configured source at instantiation time.
         */
        std::map<std::string, std::string> m_raw_values;

        /**
         * IDs for the scalar config values.
         */
        std::map<std::string, uuid> m_raw_value_ids;

        /**
         * pending IDs for new scalar config values.
         */
        std::map<std::string, uuid> m_pending_raw_value_ids;

        /**
         * Server configs.
         */
        std::list<server_config> m_server_configs;

        /**
         * Cache config.
         */
        cache_config m_cache_config;

        /**
         * create a server row from a server_config instance
         */
        void build_row(const server_config &sc, db::row_server &rs);

        /**
         * create a dns server row from a server_config instance
         */
        void build_row(const server_config &sc, db::row_dns_server &rs);

        /**
         * create a control server row from a server_config instance
         */
        void build_row(const server_config &sc, db::row_control_server &rs);

        /**
         * create a UI server row from a server_config instance
         */
        void build_row(const server_config &sc, db::row_ui_server &rs);

        /**
         * create a client row from a client_config instance
         */
        void build_row(const client_config &sc, db::row_dns_client &rc);

        /**
         * create a vector of socket addresses (ip + port) from a server_config instance
         */
        void build_socket_address_rows(const server_config &sc, std::vector<std::shared_ptr<db::row_server_socket_address>> &sar);

        /**
         * Translate a client config database row into a client config structure
         */
        void translate_client_row(client_config &c, const db::row_dns_client &r);
    };
}
