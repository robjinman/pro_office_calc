cmake_minimum_required(VERSION 3.5)

set(projectName procalc)

project(${projectName})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

add_compile_options(-std=c++14 -Wall)

if (CMAKE_BUILD_TYPE EQUAL "Release")
  add_compile_options(-O3)
else()
  message("Debug mode")
  add_compile_options(-DDEBUG -O0 -g)
endif()

add_subdirectory(tools)
add_subdirectory(tests)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(TinyXML2 REQUIRED)

file(GLOB_RECURSE srcs src/*.cpp)
list(REMOVE_ITEM srcs "src/main.cpp")

add_library(procalclib ${srcs})
add_executable(procalc "src/main.cpp")

target_link_libraries(procalc procalclib)

include_directories(src)
include_directories(${TINYXML2_INCLUDE_DIR})

target_link_libraries(procalc Qt5::Core)
target_link_libraries(procalc Qt5::Widgets)
target_link_libraries(procalc Qt5::Gui)
target_link_libraries(procalc ${TINYXML2_LIBRARIES})

target_link_libraries(procalclib Qt5::Core)
target_link_libraries(procalclib Qt5::Widgets)
target_link_libraries(procalclib Qt5::Gui)
target_link_libraries(procalclib ${TINYXML2_LIBRARIES})

file(COPY "data" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "VERSION" DESTINATION "${CMAKE_BINARY_DIR}/data")

install(TARGETS ${projectName} DESTINATION "bin")
install(DIRECTORY "data" DESTINATION "share/${projectName}")
install(FILES "${projectName}.sh" DESTINATION "games" RENAME "${projectName}")
install(FILES "VERSION" DESTINATION "share/${projectName}/data")
install(FILES "${projectName}.desktop" DESTINATION "share/applications")
install(FILES "${projectName}.svg" DESTINATION "share/pixmaps")

include(InstallRequiredSystemLibraries)
