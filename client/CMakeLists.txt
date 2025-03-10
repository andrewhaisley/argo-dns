cmake_minimum_required(VERSION 3.5)

include_directories(
    .
    ../socket
    ../db
    ../zone
    ../acore
    ../message
    ../json)

add_library(client dns_ip_selector.cpp dns_parallel_client.cpp dns_serial_client.cpp)
