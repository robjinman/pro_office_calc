cmake_minimum_required(VERSION 3.5)

include_directories(include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_compile_options(-std=c++14 -Wall -g)

file(GLOB_RECURSE srcs src/*.cpp)
add_executable(unitTests ${srcs})

target_link_libraries(unitTests procalclib gtest gtest_main pthread)
