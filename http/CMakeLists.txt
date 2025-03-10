cmake_minimum_required(VERSION 3.5)

include_directories(
    .
    ../socket
    ../acore
    ../db
    ../json)

add_library(
    http
    http1.cpp
    http2.cpp
    http.cpp
    http_impl.cpp
    http_request.cpp
    http_response.cpp
    url.cpp)
