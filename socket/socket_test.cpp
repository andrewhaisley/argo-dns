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
