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
 
#include "config_types.hpp"
#include "exception.hpp"

using namespace std;
using namespace adns;

config_types::server_type_t config_types::string_to_server_type(const std::string &protocol, const std::string &transport)
{
    string s = protocol + "." + transport;

    if (s == "dns.udp")
        return config_types::dns_udp_e;
    else if (s == "dns.tcp")
        return config_types::dns_tcp_e;
    else if (s == "control.tcp")
        return config_types::control_e;
    else if (s == "control.dot")
        return config_types::dns_dot_e;
    else if (s == "control.doh")
        return config_types::dns_doh_e;

    THROW(config_types_exception, "unrecognised server type", s);
}

