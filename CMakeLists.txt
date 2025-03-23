cmake_minimum_required(VERSION 3.5)

project(ADNS VERSION 0.1 DESCRIPTION "Argo DNS Server" LANGUAGES CXX)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

add_compile_options(-g -Wall -Ofast -DBOOST_LOG_DYN_LINK -std=c++20 -pthread)

find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(Boost REQUIRED)

include_directories(
        /usr/include/libmongoc-1.0 
        /usr/include/libbson-1.0
        ${PostgreSQL_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/json
        ${CMAKE_CURRENT_SOURCE_DIR}/zone 
        ${CMAKE_CURRENT_SOURCE_DIR}/socket 
        ${CMAKE_CURRENT_SOURCE_DIR}/acore 
        ${CMAKE_CURRENT_SOURCE_DIR}/cache
        ${CMAKE_CURRENT_SOURCE_DIR}/api
        ${CMAKE_CURRENT_SOURCE_DIR}/ui
        ${CMAKE_CURRENT_SOURCE_DIR}/db
        ${CMAKE_CURRENT_SOURCE_DIR}/server 
        ${CMAKE_CURRENT_SOURCE_DIR}/message 
        ${CMAKE_CURRENT_SOURCE_DIR}/resolver 
        ${CMAKE_CURRENT_SOURCE_DIR}/http
        ${CMAKE_CURRENT_SOURCE_DIR}/client)

link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/zone 
        ${CMAKE_CURRENT_SOURCE_DIR}/json
        ${CMAKE_CURRENT_SOURCE_DIR}/acore 
        ${CMAKE_CURRENT_SOURCE_DIR}/cache
        ${CMAKE_CURRENT_SOURCE_DIR}/api
        ${CMAKE_CURRENT_SOURCE_DIR}/ui
        ${CMAKE_CURRENT_SOURCE_DIR}/db
        ${CMAKE_CURRENT_SOURCE_DIR}/server 
        ${CMAKE_CURRENT_SOURCE_DIR}/message 
        ${CMAKE_CURRENT_SOURCE_DIR}/socket
        ${CMAKE_CURRENT_SOURCE_DIR}/resolver
        ${CMAKE_CURRENT_SOURCE_DIR}/client
        ${CMAKE_CURRENT_SOURCE_DIR}/http)

add_subdirectory(zone)
add_subdirectory(cache)
add_subdirectory(socket)
add_subdirectory(acore)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(resolver)
add_subdirectory(http)
add_subdirectory(message)
add_subdirectory(db)
add_subdirectory(api)
add_subdirectory(ui)
add_subdirectory(json)

add_executable(adns adns.cpp)
target_link_libraries(adns server resolver api ui http message zone resolver message socket db client cache acore db acore message socket json db -lpthread -lmysqlcppconn -lssl -lcrypto -lsqlite3 -lmongoc-1.0 -lbson-1.0 -lpq -lnghttp2 -lboost_thread -lboost_program_options -lboost_log_setup -lboost_log)

add_executable(aquery aquery.cpp)
target_link_libraries(aquery server resolver api ui http message zone resolver message socket db client cache acore db acore message socket json db zone message socket -lpthread -lmysqlcppconn -lssl -lcrypto -lsqlite3 -lmongoc-1.0 -lbson-1.0 -lpq -lnghttp2 -lboost_thread -lboost_program_options -lboost_log_setup -lboost_log)

add_executable(atest atest.cpp)
target_link_libraries(atest socket acore json -lpthread -lmysqlcppconn -lssl -lcrypto -lsqlite3 -lmongoc-1.0 -lbson-1.0 -lpq -lnghttp2 -lboost_thread -lboost_program_options -lboost_log_setup -lboost_log)
