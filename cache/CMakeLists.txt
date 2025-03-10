cmake_minimum_required(VERSION 3.5)

include_directories(
    .
    ../client 
    ../acore 
    ../message 
    ../zone 
    ../socket 
    ../db
    ../json)

add_library(cache dns_recursive_cache_answer.cpp dns_recursive_cache.cpp)
