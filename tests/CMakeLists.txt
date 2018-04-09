cmake_minimum_required(VERSION 3.5)

include_directories(include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories("${PROCALC_DEPENDENCIES_DIR}/include")

link_directories("${PROCALC_DEPENDENCIES_DIR}/lib")

add_compile_options(-Wall -g)

file(GLOB_RECURSE srcs src/*.cpp)
add_executable(unitTests ${srcs})

target_compile_features(unitTests PUBLIC cxx_std_14)

target_link_libraries(unitTests procalclib gtest gtest_main pthread)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/data" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
