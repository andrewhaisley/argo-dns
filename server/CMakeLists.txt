cmake_minimum_required(VERSION 3.5)

include_directories(
    .
    ../client 
    ../api 
    ../resolver 
    ../socket 
    ../acore 
    ../message 
    ../zone 
    ../db 
    ../http 
    ../cache
    ../json)

add_library(
    server 
    control_connection.cpp
    control_server.cpp
    dns_doh_connection.cpp
    dns_doh_server.cpp
    dns_dot_server.cpp
    dns_handler.cpp
    dns_tcp_connection.cpp
    dns_tcp_server.cpp
    dns_udp_server.cpp
    server_container.cpp
    server.cpp)
