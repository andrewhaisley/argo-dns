cmake_minimum_required(VERSION 3.5)

include_directories(
    .
    ../socket
    ../acore
    ../db
    ../http
    ../zone
    ../message
    ../resolver
    ../json 
    ../cache)

add_library(ui ui.cpp)
