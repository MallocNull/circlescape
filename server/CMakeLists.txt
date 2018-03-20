cmake_minimum_required(VERSION 2.6)
project(server)

file(GLOB_RECURSE server_src
    "src/*.hpp"
    "src/*.cpp"
)  

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static")
endif()
    
add_executable(server ${server_src})

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries(server pthread nsl resolv)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    target_link_libraries(server wsock32 ws2_32)
else()
    target_link_libraries(server pthread socket nsl resolv)
endif()

install(TARGETS server RUNTIME DESTINATION bin)
