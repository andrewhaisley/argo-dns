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

#include <iostream>
#include <string>
#include <stdlib.h>

#include "types.hpp"
#include "ip_address.hpp"
#include "dns_question.hpp"
#include "dns_name.hpp"
#include "dns_rr.hpp"
#include "dns_rr_SOA.hpp"
#include "dns_serial_client.hpp"

using namespace adns;
using namespace std;

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 5)
        {
            cerr << "Usage: " << argv[0] << " <host ip> <port> <domain> <type>" << endl;
            return 1;
        }

        dns_message r;

        r.set_type(dns_message::query_e);
        r.set_op_code(dns_message::op_query_e);
        r.set_response_code(dns_message::no_error_e);
        r.set_is_authoritative(false);
        r.set_is_truncated(false);
        r.set_is_recursion_desired(true);
        r.set_is_recursion_available(false);
        r.set_edns(true, 4096);

        auto name = make_shared<dns_name>(argv[3]);

        string type_string(argv[4]);

        dns_question::qtype_t   q_type;
        dns_rr::type_t          rr_type;

        if (type_string == "ALL")
        {
            q_type = dns_question::QT_ALL_e;
            rr_type = dns_rr::T_NONE_e;
        }
        else if (type_string == "IXFR")
        {
            LOG(error) << "IXFR not supported";
            return 1;
        }
        else if (type_string == "AXFR")
        {
            LOG(error) << "AXFR not supported";
            return 1;
        }
        else
        {
            q_type = dns_question::QT_RR_e;
            rr_type = dns_rr::string_to_type(type_string);
        }

        LOG(debug) << "QUERY TYPE IS " << q_type << " RR TYPE IS " << rr_type;
        auto q = make_shared<dns_question>(*name, q_type, rr_type);
        r.set_question(q);

        dns_serial_client client(socket_address(ip_address(argv[1]), atoi(argv[2])));

        auto res = client.send(r, true, false, 3, 5);
        if (res == nullptr)
        {
            LOG(info) << "no response from server";
        }
        else
        {
            res->dump();
        }
    }
    catch (adns::exception& e)
    {
        cerr << e.get_description() << ":" << e.get_details() << ":" << e.get_code() << ":" << endl;
    }

    return 0;
}
