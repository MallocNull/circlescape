cmake_minimum_required(VERSION 2.6)
project(server)

add_executable(server main.cpp)

install(TARGETS server RUNTIME DESTINATION bin)
