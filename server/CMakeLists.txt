set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)
cmake_minimum_required(VERSION 2.6)
project(server)

file(GLOB_RECURSE server_src
    "src/*.hpp"
    "src/*.cpp"
)

add_executable(server ${server_src})

install(TARGETS server RUNTIME DESTINATION bin)
