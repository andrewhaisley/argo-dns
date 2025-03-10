cmake_minimum_required(VERSION 3.5)

include_directories(
    .
    ../db
    ../acore
    ../zone
    ../socket
    ../json) 

add_library(
    message 
    dns_ipseckey.cpp
    dns_label.cpp
    dns_message.cpp
    dns_message_envelope.cpp
    dns_message_parser.cpp
    dns_message_serial.cpp
    dns_name.cpp
    dns_question.cpp
    dns_svcparam.cpp
    dns_svcparams.cpp
    dns_txt.cpp
    dns_type_bitmap.cpp)
