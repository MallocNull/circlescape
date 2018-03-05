cmake_minimum_required(VERSION 2.6)
project(server)

file(GLOB_RECURSE server_src
    "src/*.hpp"
    "src/*.cpp"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static")
add_executable(server ${server_src})

install(TARGETS server RUNTIME DESTINATION bin) 
