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
#include <time.h>
#include <iostream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include "types.hpp"
#include "exception.hpp"
#include "socket_test.hpp"
#include "network.hpp"
#include "eui48.hpp"
#include "eui64.hpp"
//#include "json.hpp"
//#include "dns_name.hpp"
//#include "frequency_meter.hpp"

using namespace std;
using namespace adns;

int main(int argc, char* argv[])
{
    eui48 e("11-22-5e-00-53-2a");
    LOG(debug) << e.to_string();
    eui64 f("11-22-5e-00-53-2a-44-55");
    LOG(debug) << f.to_string();
#if 0
    try
    {
        if (strcmp(argv[1], "client") == 0)
        {
            socket_test::test_udp_client();
        }
        else if (strcmp(argv[1], "server") == 0)
        {
            socket_test::test_udp_server();
        }
        else if (strcmp(argv[1], "tcpclient") == 0)
        {
            socket_test::test_tcp_client();
        }
        else if (strcmp(argv[1], "tcpserver") == 0)
        {
            socket_test::test_tcp_server();
        }
    }
    catch (adns::exception &e)
    {
        e.log(boost::log::trivial::fatal, "unhandled exception encountered");
        exit(1);
    }
    dns_name name("");
    dns_name other("yonsei.ac.kr");

    if (name.is_subdomain_of(other))
    {
        cout << "'" << name << "' is a subdomain of '" << other << "'" << endl;
    }

    if (other.is_subdomain_of(name))
    {
        cout << "'" << other << "' is a subdomain of '" << name << "'" << endl;
    }

    boost::uuids::uuid u = boost::uuids::random_generator()();
    cout << to_string(u) << " " << to_string(u).size() << endl;

    frequency_meter m(5);

    while (true)
    {
        for (int i = 0; i < 10; i++)
        {
            m.event();
        }

        time_t now = time(NULL);

        cout << "frequency is " << m.frequency() << endl;

        sleep(1);
    }


    dns_name a("a.blah.com");
    dns_name b("b.blah.com");
    dns_name c("a.blah.com");
    dns_name d("x.a.blah.com");
    dns_name e("blah.com");

    if (a.wildcard_equals(b))
    {
        LOG(info) << "FAIL 1: equal";
    }

    if (!a.wildcard_equals(c))
    {
        LOG(info) << "FAIL 2: not equal";
    }

    if (a.wildcard_equals(d))
    {
        LOG(info) << "FAIL 3: equal";
    }

    if (a.wildcard_equals(e))
    {
        LOG(info) << "FAIL 4: equal";
    }

    if (d.wildcard_equals(a))
    {
        LOG(info) << "FAIL 5: equal";
    }

    if (e.wildcard_equals(a))
    {
        LOG(info) << "FAIL 6: equal";
    }

    dns_name wild("*.fred.com");
    dns_name w1("fred.com");
    dns_name w2("x.fred.com");
    dns_name w3("x.y.fred.com");

    if (wild.wildcard_equals(w1))
    {
        LOG(info) << "FAIL 7: equal";
    }

    if (!wild.wildcard_equals(w2))
    {
        LOG(info) << "FAIL 8: not equal";
    }

    if (!wild.wildcard_equals(w3))
    {
        LOG(info) << "FAIL 9: not equal";
    }
#endif
}
