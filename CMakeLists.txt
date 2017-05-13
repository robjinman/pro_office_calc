cmake_minimum_required(VERSION 3.5)

set(projectName procalc)

add_compile_options(-std=c++11 -Wall)

if (CMAKE_BUILD_TYPE EQUAL "Release")
  add_compile_options(-O3)
else()
  message("Debug mode")
  add_compile_options(-DDEBUG -O0 -g)
endif()

project(${projectName})
add_subdirectory(tools)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

file(GLOB_RECURSE srcs src/*.cpp)
add_executable(procalc ${srcs})

target_link_libraries(${projectName} Qt5::Core)
target_link_libraries(${projectName} Qt5::Widgets)
target_link_libraries(${projectName} Qt5::Gui)

file(COPY "data" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "VERSION" DESTINATION "${CMAKE_BINARY_DIR}/data")

install(TARGETS ${projectName} DESTINATION "bin")
install(DIRECTORY "data" DESTINATION "share/${projectName}")
install(FILES "${projectName}.sh" DESTINATION "games" RENAME "${projectName}")
install(FILES "VERSION" DESTINATION "share/${projectName}/data")
install(FILES "${projectName}.desktop" DESTINATION "share/applications")
install(FILES "${projectName}.svg" DESTINATION "share/pixmaps")

include(InstallRequiredSystemLibraries)
