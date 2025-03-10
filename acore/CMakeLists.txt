cmake_minimum_required(VERSION 3.5)

include_directories(
    .
    ../db 
    ../socket 
    ../json)

add_library(acore
    buffer.cpp
    cache_config.cpp
    client_config.cpp
    config.cpp
    config_reader.cpp
    config_types.cpp
    datetime.cpp
    exception.cpp
    frequency_meter.cpp
    json_serializable.cpp
    monitor.cpp
    read_write_lock.cpp
    run_state.cpp
    server_config.cpp
    util.cpp)
