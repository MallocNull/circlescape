cmake_minimum_required(VERSION 3.7)
project(sockscape)

string(COMPARE EQUAL ${CMAKE_BUILD_TYPE} Debug _CMP)
if(_CMP)
    set(CMAKE_VERBOSE_MAKEFILE ON)
    add_definitions("-DSOSC_DEBUG")
endif()

string(COMPARE EQUAL ${SOSC_BUILD_TARGET} CLIENT _CMP)
if(_CMP)

    ##################
    ## CLIENT BUILD ##
    ##################

    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -s USE_SDL=2 --preload-file ../resources/client")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} --shell-file ../src/client/shell.html")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"bmp\"]'")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -s EXPORTED_FUNCTIONS='[\"_main\", \"_resize_context\"]'")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -s EXTRA_EXPORTED_RUNTIME_METHODS='[\"ccall\"]'")

    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    file(GLOB_RECURSE client_src
        #"src/common/*.hpp"
        #"src/common/*.cpp"
        "src/client/*.hpp"
        "src/client/*.cpp"
    )

    add_executable(client ${client_src})

    add_custom_command(
        TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
    )

    target_include_directories(client
        #PRIVATE ${PROJECT_SOURCE_DIR}/src/common
        PRIVATE ${PROJECT_SOURCE_DIR}/src/client
        PRIVATE ${PROJECT_SOURCE_DIR}/include/client)

    install(TARGETS client RUNTIME DESTINATION bin/client)

else()

    ##################
    ## SERVER BUILD ##
    ##################

    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_STANDARD 11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()

    file(GLOB_RECURSE server_src
        "src/common/*.hpp"
        "src/common/*.cpp"
        "src/server/*.hpp"
        "src/server/*.cpp"
        "src/server/*.c"
        "src/server/*.h"
    )

    find_package(OpenSSL)

    add_executable(server ${server_src})
    target_include_directories(server
        PRIVATE ${PROJECT_SOURCE_DIR}/src/common
        PRIVATE ${PROJECT_SOURCE_DIR}/src/server
        PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(server ${OPENSSL_LIBRARIES})
    install(TARGETS server RUNTIME DESTINATION bin/server)

    if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        target_link_libraries(server wsock32 ws2_32)
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        target_link_libraries(server dl pthread nsl resolv)
    else()
        target_link_libraries(server dl pthread socket nsl resolv)
    endif()
endif()
