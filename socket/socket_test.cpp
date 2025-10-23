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
 
#include "socket_test.hpp"
#include "udp_socket.hpp"
#include "tcp_client_socket.hpp"
#include "tcp_server_socket.hpp"

using namespace std;
using namespace adns;

socket_test::socket_test()
{
}

socket_test::~socket_test()
{
}

void socket_test::test_udp_client()
{
    udp_socket s(ip_address::ip_v6_e);
    //s.send(buffer("test"), socket_address(ip_address("127.0.0.1"), 6666));
    s.send(buffer("test"), socket_address(ip_address("::1"), 6666));
}

void socket_test::test_udp_server()
{
    //udp_socket s(socket_address(ip_address("127.0.0.1"), 6666));
    udp_socket s(socket_address(ip_address("::1"), 6666));
    socket_address remote_address;
    buffer b = s.receive(remote_address, 10000);
    LOG(info) << "received " << b.to_string();
}

void socket_test::test_tcp_client()
{
    tcp_client_socket s;
    s.connect(socket_address(ip_address("127.0.0.1"), 6666));
    s.write(buffer("test"), 10000);
}

void socket_test::test_tcp_server()
{
    tcp_server_socket s(socket_address(ip_address("127.0.0.1"), 6666));
    s.listen(5);
    auto ss = s.accept();
    buffer b = ss->read(4, 10000);
    LOG(info) << "read " << b.to_string();
}
