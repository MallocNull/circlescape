cmake_minimum_required(VERSION 3.7)
project(sockscape)

string(COMPARE EQUAL ${CMAKE_BUILD_TYPE} Debug _CMP)
if(_CMP)
    set(CMAKE_VERBOSE_MAKEFILE ON)
    add_definitions("-DSOSC_DEBUG")
endif()

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
#if(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++11 -s USE_SDL=2")
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} --preload-file ../resources/client")
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} --shell-file ../src/client/shell.html")
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"bmp\"]'")
#endif()

set(CMAKE_EXECUTABLE_SUFFIX ".html")

## CLIENT BUILD ##

#find_package(OpenGL REQUIRED)
#find_package(GLEW REQUIRED)
#find_package(SDL2 REQUIRED)
#find_package(SDL2_image REQUIRED)

file(GLOB_RECURSE client_src
    #"src/common/*.hpp"
    #"src/common/*.cpp"
    "src/client/*.hpp"
    "src/client/*.cpp"
)

add_executable(client ${client_src})

target_include_directories(client
    #PRIVATE ${PROJECT_SOURCE_DIR}/src/common
    PRIVATE ${PROJECT_SOURCE_DIR}/src/client
    PRIVATE ${PROJECT_SOURCE_DIR}/include/client)
    #PRIVATE ${OPENGL_INCLUDE_DIR}
    #PRIVATE ${GLEW_INCLUDE_DIR}
    #PRIVATE ${SDL2_INCLUDE_DIR}
    #PRIVATE ${SDL2_IMAGE_INCLUDE_DIR})
#target_link_libraries(client
#    ${OPENGL_LIBRARIES}
#    ${GLEW_LIBRARY}
#    ${SDL2_LIBRARY}
#    ${SDL2_IMAGE_LIBRARIES})
install(TARGETS client RUNTIME DESTINATION bin/client)

## SERVER BUILD ##

#file(GLOB_RECURSE server_src
#    "src/common/*.hpp"
#    "src/common/*.cpp"
#    "src/server/*.hpp"
#    "src/server/*.cpp"
#    "src/server/*.c"
#    "src/server/*.h"
#)

#add_executable(server ${server_src})
#target_include_directories(server
#    PRIVATE ${PROJECT_SOURCE_DIR}/src/common
#    PRIVATE ${PROJECT_SOURCE_DIR}/src/server)
#install(TARGETS server RUNTIME DESTINATION bin/server)

## COMMON LIBRARIES ##

#if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
#    target_link_libraries(server wsock32 ws2_32)
#    target_link_libraries(client wsock32 ws2_32)
#elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#    target_link_libraries(server dl pthread nsl resolv)
#    target_link_libraries(client dl pthread nsl resolv)
#else()
#    target_link_libraries(server dl pthread socket nsl resolv)
#    target_link_libraries(client dl pthread socket nsl resolv)
#endif()
