cmake_minimum_required(VERSION 3.12)
project(client)

file(GLOB_RECURSE client_str
    "src/*.hpp"
    "src/*.cpp"
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(client src/main.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${SDL2_INCLUDE_DIR}
                    ${SDL2_IMAGE_INCLUDE_DIR}
                    ${SDL2_TTF_INCLUDE_DIR})
target_link_libraries(client ${SDL2_LIBRARY}
                              ${SDL2_IMAGE_LIBRARIES}
                              ${SDL2_TTF_LIBRARIES})

install(TARGETS client RUNTIME DESTINATION bin)