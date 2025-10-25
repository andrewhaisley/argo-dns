cmake_minimum_required(VERSION 3.5)

include_directories(
    .
    ../acore
    ../db
    ../json)

add_library(
    socket 
    address_list.cpp
    eui48.cpp
    eui64.cpp
    ip_address.cpp
    network.cpp
    socket_address.cpp
    socket.cpp
    socket_set.cpp
    ssl_server_socket.cpp
    tcp_client_socket.cpp
    tcp_server_socket.cpp
    tcp_socket.cpp
    udp_socket.cpp)
