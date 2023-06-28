cmake_minimum_required(VERSION 3.6)
project(SGCB)

set(CMAKE_CXX_STANDARD 14)
#Version number
set(SGCB_VERSION_MAJOR 0)
set(SGCB_VERSION_MINOR 1)

#Configuration for a header file to allow CMake variables to be passed to the source code
configure_file(
        ${PROJECT_SOURCE_DIR}/include/SGCB_Config.h.in
        ${PROJECT_BINARY_DIR}/SGCB_Config.h
)

message(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
message(PROJECT_BINARY_DIR="${PROJECT_BINARY_DIR}")
message(CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
message(CMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_BINARY_DIR})

add_subdirectory(src)
