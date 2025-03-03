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
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <atomic>

#define BOOST_LOG_USE_NATIVE_SYSLOG
#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/syslog_backend.hpp>

#include "types.hpp"
#include "config.hpp"
#include "exception.hpp"
#include "connection_pool.hpp"
#include "transaction.hpp"
#include "ip_address.hpp"

using namespace adns;
using namespace db;
using namespace std;
using namespace boost::log;

const char *config::default_bootstrap_file = "/etc/adns.conf.json";
config_reader config::o_config_reader;
boost::property_tree::ptree config::o_tree;

atomic<unsigned short> o_edns_size = 4096;
atomic<bool> o_support_edns = true;
atomic<bool> o_use_ip4_root_hints = true;
atomic<bool> o_use_ip6_root_hints = true;

string config::get_tree_string(const string &name, const string &exception_text)
{
    string res = o_tree.get<string>(name, "");

    if (res == "")
    {
        THROW(config_exception, exception_text);
    }
    else
    {
        return res;
    }
}

string config::ssl_server_chain_file()
{
    return get_tree_string(
                "static-config.ssl-server-chain-file",
                "ssl-server-chain-file not set in configuration");
}

string config::ssl_server_key_file()
{
    return get_tree_string(
                "static-config.ssl-server-key-file",
                "ssl-server-key-file not set in configuration");
}

string config::server_config_username()
{
    return get_tree_string(
                    "static-config.server-config-username", 
                    "server config username (static-config.server-config-username) not set in config file");
}

string config::server_config_password()
{
    return get_tree_string(
                    "static-config.server-config-password", 
                    "server config password (static-config.server-config-password) not set in config file");
}

void config::init_log(string bootstrap_config_filename)
{
    LOG(info) << "initialising bootstrap configuration from " << bootstrap_config_filename;

    boost::property_tree::json_parser::read_json(bootstrap_config_filename, o_tree);

    string log_destination = o_tree.get<string>("static-config.log.destination", "console");

    if (log_destination == "console")
    {
        // do nothing, all the defaults are fine
        LOG(info) << "log output to console";
    }
    else if (log_destination == "file")
    {
        add_common_attributes();

        string filename = o_tree.get<string>("static-config.log.file.filename", "adns_%N.log");
        uint rotation_size = o_tree.get<uint>("static-config.log.file.rotation_size", 1024 * 1024 * 10);
        string format = o_tree.get<string>("static-config.log.file.format", "[%TimeStamp%] [%ThreadID%] : %Message%");

        add_file_log
        (
            keywords::file_name = filename,
            keywords::rotation_size = rotation_size,
            keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
            keywords::format = format
        );

        LOG(info) << "redirected log output to file " << filename;

        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
    else if (log_destination == "syslog-native")
    {
        typedef sinks::synchronous_sink<sinks::syslog_backend> sink_t;

        auto core = core::get();

        boost::shared_ptr<sinks::syslog_backend> backend(new sinks::syslog_backend(
            keywords::facility = sinks::syslog::local0,
            keywords::use_impl = sinks::syslog::native));

        backend->set_severity_mapper(sinks::syslog::direct_severity_mapping<int>("Severity"));

        core->add_sink(boost::make_shared<sink_t>(backend));

        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
    else if (log_destination == "syslog-udp")
    {
        string address = o_tree.get<string>("static-config.log.syslog-udp.address", "127.0.0.1");
        uint port = o_tree.get<uint>("static-config.log.syslog-udp.port", 514);

        typedef sinks::synchronous_sink<sinks::syslog_backend> sink_t;

        auto core = core::get();

        boost::shared_ptr<sinks::syslog_backend> backend(new sinks::syslog_backend(
            keywords::facility = sinks::syslog::local0,
            keywords::use_impl = sinks::syslog::udp_socket_based));

        backend->set_target_address(address, port);
        backend->set_severity_mapper(sinks::syslog::direct_severity_mapping<int>("Severity"));

        core->add_sink(boost::make_shared<sink_t>(backend));

        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
    else
    {
        THROW(config_exception, "invalid log destination - must be one of stdout, file, syslog", log_destination);
    }

    string log_level = o_tree.get<string>("static-config.log.level", "info");

    if (log_level == "trace")
    {
        core::get()->set_filter(trivial::severity >= trivial::trace);
    }
    else if (log_level == "debug")
    {
        core::get()->set_filter(trivial::severity >= trivial::debug);
    }
    else if (log_level == "info")
    {
        core::get()->set_filter(trivial::severity >= trivial::info);
    }
    else if (log_level == "warning")
    {
        core::get()->set_filter(trivial::severity >= trivial::warning);
    }
    else if (log_level == "error")
    {
        core::get()->set_filter(trivial::severity >= trivial::error);
    }
    else if (log_level == "fatal")
    {
        core::get()->set_filter(trivial::severity >= trivial::fatal);
    }
}

void config::init_db_config()
{
    string type = o_tree.get<string>("static-config.dbtype", "sqlite");
    
    if (type == "sqlite")
    {
        LOG(info) << "using SQLite DB";
        string filename = o_tree.get<string>("static-config.sqlite.filename", "");

        connection_pool::init(sqlite_e, filename, "", "", "", true);
    }
    else if (type == "mysql")
    {
        LOG(info) << "using MySQL DB";
        string url = o_tree.get<string>("static-config.mysql.url", "");

        connection_pool::init(
                mysql_e,
                url,
                o_tree.get<string>("static-config.mysql.username"),
                o_tree.get<string>("static-config.mysql.password"),
                o_tree.get<string>("static-config.mysql.database"),
                true);
    }
    else if (type == "mongodb")
    {
        LOG(warning) << "using MongoDB DB experimental support";
        string url = o_tree.get<string>("static-config.mongodb.url", "");

        connection_pool::init(
                mongodb_e,
                url,
                "",
                "",
                o_tree.get<string>("static-config.mongodb.database"),
                o_tree.get<bool>("static-config.mongodb.use_transactions"));
    }
    else if (type == "postgres")
    {
        LOG(info) << "using Postgres DB";
        string info = o_tree.get<string>("static-config.postgres.info", "");

        connection_pool::init(
                postgres_e,
                info,
                "",
                "",
                "",
                true);
    }
    else
    {
        THROW(config_exception, "unknown database type", type);
    }
}

void config::init_config()
{
    o_config_reader.read();

    o_edns_size = static_cast<unsigned short>(o_config_reader.get_uint("all.edns-size", o_edns_size));
    o_support_edns = o_config_reader.get_bool("all.support-edns", o_support_edns);
    o_use_ip4_root_hints = o_config_reader.get_bool("all.use-ip4-root-hints", o_use_ip4_root_hints);
    o_use_ip6_root_hints = o_config_reader.get_bool("all.use-ip6-root-hints", o_use_ip6_root_hints);
}

bool config::daemon(string bootstrap_config_filename)
{
    boost::property_tree::ptree p_tree;
    boost::property_tree::json_parser::read_json(bootstrap_config_filename, p_tree);
    return p_tree.get<bool>("static-config.run-as-daemon", false);
}

nullable<int> config::run_as_user_id()
{
    string name = o_config_reader.get_string("all.seteuid", "");
    if (name == "")
    {
        return nullable<int>();
    }
    else
    {
        struct passwd *pw = getpwnam(name.c_str());

        if (pw == nullptr)
        {
            THROW(config_exception, "user name does not exist for all.seteuid", name);
        }
        else
        {
            return nullable<int>(pw->pw_uid);
        }
    }
}

nullable<int> config::run_as_group_id()
{
    string name = o_config_reader.get_string("all.setegid", "");
    if (name == "")
    {
        return nullable<int>();
    }
    else
    {
        struct group *gr = getgrnam(name.c_str());

        if (gr == nullptr)
        {
            THROW(config_exception, "group name does not exist for all.setegid", name);
        }
        else
        {
            return nullable<int>(gr->gr_gid);
        }
    }
}

nullable<string> config::chroot_dir()
{
    string res = o_config_reader.get_string("all.chroot", "");
    if (res == "")
    {
        return nullable<string>();
    }
    else
    {
        return nullable<string>(res);
    }
}

list<server_config> config::get_servers()
{
    return o_config_reader.servers();
}

cache_config config::get_cache_config()
{
    return o_config_reader.get_cache_config();
}

config::config()
{
}

bool config::socket_addresses_overlap(
            const list<server_config::socket_address_t> &sal1,
            const list<server_config::socket_address_t> &sal2)
{
    for (auto i1 : sal1)
    {
        for (auto i2 : sal2)
        {
            if (i1.port == i2.port)
            {
                ip_address ip1(i1.ip_address);
                ip_address ip2(i2.ip_address);

                if (ip1.get_type() == ip2.get_type())
                {
                    if ((ip1 == ip2) || ip1.is_any() || ip2.is_any())
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void config::add_server(const server_config &sc)
{
    if (sc.type == server_config::control_e)
    {
        THROW(config_exception, "can't add extra control servers, only one is allowed");
    }

    for (auto es : o_config_reader.servers())
    {
        if ((sc.server_id != es.server_id) && socket_addresses_overlap(sc.socket_addresses, es.socket_addresses))
        {
            THROW(config_addresses_exception, "socket addresses for new server overlap with existing one");
        }
    }

    o_config_reader.add_server(sc);
}

void config::update_server(const server_config &sc)
{
    o_config_reader.update_server(sc);
}

void config::delete_server(const server_config &sc)
{
    if (sc.type == server_config::control_e)
    {
        THROW(config_exception, "can't delete the control server");
    }
    o_config_reader.delete_server(sc);
}

short unsigned config::edns_size()
{
    return o_edns_size;
}

bool config::support_edns()
{
    return o_support_edns;
}

uint config::use_ip4_root_hints()
{
    return o_use_ip4_root_hints;
}

uint config::use_ip6_root_hints()
{
    return o_use_ip6_root_hints;
}

json config::base_config_to_json()
{
    json m(json::object_e);

    m["edns_size"] = int(o_edns_size);
    m["support_edns"] = o_support_edns;
    m["use_ip4_root_hints"] = o_use_ip4_root_hints;
    m["use_ip6_root_hints"] = o_use_ip6_root_hints;

    return m;
}

void config::set_base_config_from_json(const json &j)
{
    bool temp_use_ip4 = j["use_ip4_root_hints"];
    bool temp_use_ip6 = j["use_ip6_root_hints"];

    if (!temp_use_ip4 && !temp_use_ip6)
    {
        THROW(config_exception, "must use one of ip4 or ip6 root hints");
    }

    auto conn = connection_pool::get_connection();

    transaction t(*conn);

    o_config_reader.set_uint_db(*conn, "all.edns-size", uint(int(j["edns_size"])));
    o_config_reader.set_bool_db(*conn, "all.support-edns", j["support_edns"]);
    o_config_reader.set_bool_db(*conn, "all.use-ip4-root-hints", j["use_ip4_root_hints"]);
    o_config_reader.set_bool_db(*conn, "all.use-ip6-root-hints", j["use_ip6_root_hints"]);

    t.commit();

    o_config_reader.set_uint_cache("all.edns-size", uint(int(j["edns_size"])));
    o_config_reader.set_bool_cache("all.support-edns", j["support_edns"]);
    o_config_reader.set_bool_cache("all.use-ip4-root-hints", j["use_ip4_root_hints"]);
    o_config_reader.set_bool_cache("all.use-ip6-root-hints", j["use_ip6_root_hints"]);

    o_edns_size = uint(int(j["edns_size"]));
    o_support_edns = j["support_edns"];
    o_use_ip4_root_hints = j["use_ip4_root_hints"];
    o_use_ip6_root_hints = j["use_ip6_root_hints"];
}

json config::cache_config_to_json()
{
    return o_config_reader.get_cache_config().to_json();
}

void config::set_cache_config_from_json(const json &j)
{
    cache_config c;
    c.from_json(j);
    o_config_reader.update_cache_config(c);
}
