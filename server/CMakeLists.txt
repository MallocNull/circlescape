cmake_minimum_required(VERSION 2.6)
project(server)  

file(GLOB_RECURSE server_src
    "src/*.hpp"
    "src/*.cpp"
    "src/*.c"
    "src/*.h"
)  

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static")
endif()
    
add_executable(server ${server_src} src/db/_init_sql.cpp)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    target_link_libraries(server wsock32 ws2_32)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries(server dl pthread nsl resolv)
else()
    target_link_libraries(server dl pthread socket nsl resolv)
endif()

install(TARGETS server RUNTIME DESTINATION bin) 
 
