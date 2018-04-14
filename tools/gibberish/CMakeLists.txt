cmake_minimum_required(VERSION 3.5)

add_compile_options(-O3 -Wall)

file(GLOB_RECURSE srcs src/*.cpp)
add_executable(gibberish ${srcs})
