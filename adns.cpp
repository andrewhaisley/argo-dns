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
#include <unistd.h>
#include <iostream>
#include <string>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <cppconn/exception.h>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/exception/all.hpp>

#include "types.hpp"
#include "server_container.hpp"
#include "config.hpp"
#include "util.hpp"
#include "exception.hpp"
#include "dns_udp_server.hpp"
#include "dns_tcp_server.hpp"
#include "dns_dot_server.hpp"
#include "dns_doh_server.hpp"
#include "dns_recursive_cache.hpp"
#include "control_server.hpp"
#include "ui_server.hpp"
#include "dns_rr.hpp"
#include "address_list.hpp"
#include "dns_horizon.hpp"
#include "http_response.hpp"
#include "network.hpp"
#include "run_state.hpp"
#include "schema_manager.hpp"

#include "connection_pool.hpp"

using namespace std;
using namespace adns;
using namespace db;
using namespace boost::log::trivial;


EXCEPTION_CLASS(main_exception, exception)

void make_daemon()
{
    /* Our process ID and Session ID */
    pid_t pid, sid;
    
    /* Fork off the parent process */
    pid = fork();

    if (pid < 0) 
    {
        LOG(fatal) << "fork failed";
        exit(1);
    }

    if (pid > 0)
    {
        exit(0);
    }

    /* Change the file mode mask */
    umask(0);
            
    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) 
    {
        LOG(fatal) << "setsid failed";
        exit(1);
    }
}

void set_user()
{
    nullable<int> uid = config::run_as_user_id();

    if (!uid.is_null())
    {
        if (seteuid(uid.value()) != 0)
        {
            THROW(main_exception, "seteuid failed", util::strerror(), errno);
        }
    }
}

void set_group()
{
    nullable<int> gid = config::run_as_group_id();

    if (!gid.is_null())
    {
        if (setegid(gid.value()) != 0)
        {
            THROW(main_exception, "setegid failed", util::strerror(), errno);
        }
    }
}

void set_root_directory()
{
    nullable<string> root = config::chroot_dir();

    if (!root.is_null())
    {
        if (chroot(root.value().c_str()) != 0)
        {
            THROW(main_exception, "chroot failed", util::strerror(), errno);
        }
    }
}

void class_init()
{
    dns_horizon::load_cache();
    network::init();
    dns_recursive_cache::init(config::get_cache_config());
}

void run_servers()
{
    boost::asio::io_service io_service;
    server_container sc(io_service);

    list<server_config> servers = config::get_servers();

    for (auto iter : servers)
    {
        if (!iter.enabled)
        {
            LOG(info) 
                    << "skipping disabled server " 
                    << iter.server_id << " " 
                    << iter.protocol << " " 
                    << iter.transport;
            continue;
        }

        switch (iter.type)
        {
        case server_config::dns_udp_e :
            LOG(info) << "starting dns udp ipv4/ipv6";
            for (auto x : iter.socket_addresses)
            {
                LOG(info) << "binding to " << x.ip_address << " port " << x.port;
            }
            try
            {
                sc.add(new dns_udp_server(io_service, iter));
            }
            catch (adns::exception &e)
            {
                e.log(error, "failed to start DNS UDP server instance");
            }
            break;
        case server_config::dns_tcp_e :
            LOG(info) << "starting dns tcp ipv4/ipv6";
            for (auto x : iter.socket_addresses)
            {
                LOG(info) << "binding to " << x.ip_address << " port " << x.port;
            }
            try
            {
                sc.add(new dns_tcp_server(io_service, iter));
            }
            catch (adns::exception &e)
            {
                e.log(error, "failed to start DNS TCP server instance");
            }
            break;
        case server_config::dns_dot_e :
            LOG(info) << "starting dns DoT ipv4/ipv6";
            for (auto x : iter.socket_addresses)
            {
                LOG(info) << "binding to " << x.ip_address << " port " << x.port;
            }
            try
            {
                sc.add(new dns_dot_server(io_service, iter));
            }
            catch (adns::exception &e)
            {
                e.log(error, "failed to start DNS DoT server instance");
            }
            break;
        case server_config::dns_doh_e :
            LOG(info) << "starting dns DoH ipv4/ipv6";
            for (auto x : iter.socket_addresses)
            {
                LOG(info) << "binding to " << x.ip_address << " port " << x.port;
            }
            try
            {
                sc.add(new dns_doh_server(io_service, iter));
            }
            catch (adns::exception &e)
            {
                e.log(error, "failed to start DNS DoT server instance");
            }
            break;
        case server_config::control_e :
            LOG(info) << "starting control server (tcp ipv4/ipv6)";
            for (auto x : iter.socket_addresses)
            {
                LOG(info) << "binding to " << x.ip_address << " port " << x.port;
            }
            try
            {
                sc.add(new control_server(io_service, iter));
            }
            catch (adns::exception &e)
            {
                e.log(error, "failed to start control server instance");
            }
            break;
        case server_config::ui_e :
            LOG(info) << "starting UI server (tcp ipv4/ipv6)";
            for (auto x : iter.socket_addresses)
            {
                LOG(info) << "binding to " << x.ip_address << " port " << x.port;
            }
            try
            {
                sc.add(new ui_server(io_service, iter));
            }
            catch (adns::exception &e)
            {
                e.log(error, "failed to start UI server instance");
            }
            break;
        default:
            LOG(warning) << "Unknown server type";
            break;
        }
    }

    sc.run();
    sc.join();

    dns_horizon::empty_cache();
}

int main(int argc, char *argv[], char *envp[])
{
    SSL_library_init();
    SSL_load_error_strings();

    run_state::store_args(argv, envp);

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("config-file", boost::program_options::value<string>(), "boostrap configuration file name");

    try
    {
        boost::program_options::variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help") > 0) 
        {
            cout << "Usage: options_description [options]\n";
            cout << desc;
            return 0;
        }

        string bootstrap_config = config::default_bootstrap_file;

        if (vm.count("config-file") == 0)
        {
            LOG(info) << "no bootstrap configuration file specified on command line, defaulting to " << bootstrap_config;
        }
        else
        {
            bootstrap_config = vm["config-file"].as<string>();
        }

        config::init_log(bootstrap_config);
        LOG(info) << "starting " << VERSION;

        set_user();
        set_group();
        set_root_directory();

        if (config::daemon(bootstrap_config))
        {
            make_daemon();
        }

        config::init_db_config();
        schema_manager::update_or_create();
        address_list::load_cache();
        config::init_config();
        class_init();

        run_servers();

        if (run_state::o_restart)
        {
            LOG(info) << "all done, restarting";
            run_state::restart();
        }
        else
        {
            LOG(info) << "all done, exiting";
            exit(0);
        }
    }
    catch (adns::exception &e)
    {
        e.log(fatal, "unhandled exception encountered");
        exit(1);
    }
    catch (sql::SQLException &e)
    {
        LOG(fatal) << e.what();
        exit(1);
    }
    catch (boost::program_options::error &e)
    {
        cerr << "Usage: options_description [options]\n";
        cerr << desc;
        exit(1);
    }
    catch (boost::exception &e)
    {
        LOG(fatal) << diagnostic_information(e);
        exit(1);
    }
    catch (std::exception &e)
    {
        LOG(fatal) << e.what();
        exit(1);
    }
}
