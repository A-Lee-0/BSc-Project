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

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib_network/include)
include_directories(${PROJECT_BINARY_DIR})

add_executable(SGCB main.cpp)

add_subdirectory(src)
add_subdirectory(lib_network)

target_link_libraries(SGCB core)
target_link_libraries(SGCB network)



