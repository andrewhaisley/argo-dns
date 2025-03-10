cmake_minimum_required(VERSION 3.5)

add_library(
    json 
    fd_reader.cpp
    fd_writer.cpp
    file_reader.cpp
    file_writer.cpp
    json_array_index_range_exception.cpp
    json.cpp
    json_exception.cpp
    json_invalid_key_exception.cpp
    json_io_exception.cpp
    json_parser_exception.cpp
    json_pointer_exception.cpp
    json_utf8_exception.cpp
    lexer.cpp
    parser.cpp
    pointer.cpp
    reader.cpp
    stream_reader.cpp
    stream_writer.cpp
    token.cpp
    unparser.cpp
    utf8.cpp
    writer.cpp)
