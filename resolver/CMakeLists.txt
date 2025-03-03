project(ADNS)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++17 -O3 -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++20 -Ofast")

include_directories("/usr/include/libmongoc-1.0")
include_directories("/usr/include/libbson-1.0")
include_directories("/usr/include/postgresql")
include_directories(../resolver ../ ../socket/ ../acore/ ../message/ ../zone/ ../db/ ../cache/ ../client/ ../json)

file(GLOB sources *.cpp)

add_library(resolver ${sources})
