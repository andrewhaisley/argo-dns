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

add_library(
    api 
    api_auth_token_v1.cpp
    api_address_list_v1.cpp
    api_base_configuration_v1.cpp
    api_cache_configuration_v1.cpp
    api.cpp
    api_guard.cpp
    api_horizon_v1.cpp
    api_monitor_v1.cpp
    api_resource_record_v1.cpp
    api_run_state_v1.cpp
    api_server_v1.cpp
    api_url_v1.cpp
    api_v1.cpp
    api_zone_v1.cpp)
