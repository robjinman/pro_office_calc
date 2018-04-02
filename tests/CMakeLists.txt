cmake_minimum_required(VERSION 3.5)

include_directories(include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_BINARY_DIR}/dependencies/tinyxml2/include)
include_directories(${CMAKE_BINARY_DIR}/dependencies/googleTest/include)

link_directories(${CMAKE_BINARY_DIR}/dependencies/tinyxml2/lib)
link_directories(${CMAKE_BINARY_DIR}/dependencies/googleTest/lib)

add_compile_options(-std=c++14 -Wall -g)

file(GLOB_RECURSE srcs src/*.cpp)
add_executable(unitTests ${srcs})

add_dependencies(unitTests googleTest)

target_link_libraries(unitTests procalclib gtest gtest_main pthread)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/data" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
