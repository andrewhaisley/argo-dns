cmake_minimum_required(VERSION 3.5)

include_directories(
    .
    ../resolver 
    ../socket
    ../acore 
    ../message 
    ../zone
    ../db
    ../cache
    ../client 
    ../json)

add_library(
    resolver 
    dns_auth_resolver.cpp
    dns_forwarding_resolver.cpp
    dns_forwarding_slot.cpp
    dns_horizon.cpp
    dns_recursive_resolver.cpp
    dns_recursive_slot.cpp
    dns_recursive_slot_manager.cpp)
