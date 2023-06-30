cmake_minimum_required(VERSION 3.6)
add_library(network Network.cpp Update.cpp Strategy.cpp)
target_include_directories(network PUBLIC ${PROJECT_SOURCE_DIR}/lib_network/include)
message(TestIncludedir="${PROJECT_SOURCE_DIR}/lib_network/include")

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()
