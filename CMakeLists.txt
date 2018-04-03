cmake_minimum_required(VERSION 3.5)

set(projectName procalc)

project(${projectName})

add_subdirectory(dependencies)
add_subdirectory(tools)
add_subdirectory(tests)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(OpenMP REQUIRED)
if (PROFILING_ON)
  find_package(GooglePerfTools REQUIRED)
endif()

file(GLOB_RECURSE srcs src/*.cpp)
list(REMOVE_ITEM srcs "src/main.cpp")

include_directories(src)
include_directories(${CMAKE_BINARY_DIR}/out/usr/local/include)

link_directories(${CMAKE_BINARY_DIR}/out/usr/local/lib)

add_compile_options(-std=c++14 -Wall ${OpenMP_CXX_FLAGS})

if (CMAKE_BUILD_TYPE EQUAL "Release")
  add_compile_options(-O3)
else()
  message("Debug mode")
  add_compile_options(-DDEBUG -O3 -g)

  if (PROFILING_ON)
    message("Profiling ON")

    add_compile_options(-O3)
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed -lprofiler -Wl,--as-needed")
  endif()
endif()

add_library(procalclib ${srcs})

add_dependencies(procalclib libTinyXml2)

target_link_libraries(procalclib "${OpenMP_CXX_FLAGS}")
target_link_libraries(procalclib Qt5::Core)
target_link_libraries(procalclib Qt5::Widgets)
target_link_libraries(procalclib Qt5::Gui)
target_link_libraries(procalclib Qt5::Multimedia)
target_link_libraries(procalclib tinyxml2)

add_executable(procalc "src/main.cpp")

target_link_libraries(procalc procalclib)

file(COPY "data" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "VERSION" DESTINATION "${CMAKE_BINARY_DIR}/data")

install(TARGETS ${projectName} DESTINATION "bin")
install(DIRECTORY "data" DESTINATION "share/${projectName}")
install(FILES "${projectName}.sh" DESTINATION "games" RENAME "${projectName}")
install(FILES "VERSION" DESTINATION "share/${projectName}/data")
install(FILES "${projectName}.desktop" DESTINATION "share/applications")
install(FILES "${projectName}.svg" DESTINATION "share/pixmaps")

include(InstallRequiredSystemLibraries)
