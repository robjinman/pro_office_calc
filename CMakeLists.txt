cmake_minimum_required(VERSION 3.5)

set(projectName procalc)

project(${projectName})

if (WIN32)
  set(QT_DIR "C:/Qt/5.5/custom_dist")
  add_compile_options(-DWIN32)
endif()

set(PROCALC_DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/dependencies/build/dist")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${PROCALC_DEPENDENCIES_DIR}/lib/cmake")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${QT_DIR}/lib/cmake")

if (UNIX)
  add_subdirectory(tools)
  add_subdirectory(tests)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(TinyXml2 REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)
find_package(Qt5Multimedia CONFIG REQUIRED)
find_package(Qt5MultimediaWidgets CONFIG REQUIRED)
find_package(Qt5OpenGL CONFIG REQUIRED)
find_package(OpenMP REQUIRED)
if (PROFILING_ON)
  find_package(GooglePerfTools REQUIRED)
endif()

file(GLOB_RECURSE srcs src/*.cpp)
list(REMOVE_ITEM srcs "src/main.cpp")

include_directories(src)
include_directories("${PROCALC_DEPENDENCIES_DIR}/include")
include_directories("${QT_DIR}/include")

link_directories("${PROCALC_DEPENDENCIES_DIR}/bin")
#link_directories("${PROCALC_DEPENDENCIES_DIR}/lib")
link_directories("${QT_DIR}/bin")
#link_directories("${QT_DIR}/lib")

add_compile_options(${OpenMP_CXX_FLAGS})

if (CMAKE_BUILD_TYPE EQUAL "Release")
  message("Release mode")

  if (WIN32)
    add_compile_options(-O2)
  else()
    add_compile_options(-O3)
  endif()
else()
  message("Debug mode")
  add_compile_options(-DDEBUG)

  if (NOT WIN32)
    add_compile_options(-g)

    if (PROFILING_ON)
      message("Profiling ON")
      set(CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed -lprofiler -Wl,--as-needed")
    endif()
  endif()
endif()

add_library(procalclib ${srcs})

target_compile_features(procalclib PUBLIC cxx_std_14)

if (NOT WIN32)
  target_link_libraries(procalclib "${OpenMP_CXX_FLAGS}")
endif()
target_link_libraries(procalclib tinyxml2)
target_link_libraries(procalclib Qt5::Core)
target_link_libraries(procalclib Qt5::Widgets)
target_link_libraries(procalclib Qt5::Gui)
target_link_libraries(procalclib Qt5::Multimedia)
target_link_libraries(procalclib Qt5::MultimediaWidgets)
target_link_libraries(procalclib Qt5::OpenGL)

add_executable(procalc "src/main.cpp")

target_compile_features(procalc PUBLIC cxx_std_14)
target_link_libraries(procalc procalclib)

file(COPY "data" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "VERSION" DESTINATION "${CMAKE_BINARY_DIR}/data")

if (UNIX)
  install(TARGETS ${projectName} DESTINATION "bin")
  install(DIRECTORY "data" DESTINATION "share/${projectName}")
  install(FILES "${projectName}.sh" DESTINATION "games" RENAME "${projectName}")
  install(FILES "VERSION" DESTINATION "share/${projectName}/data")
  install(FILES "${projectName}.desktop" DESTINATION "share/applications")
  install(FILES "${projectName}.svg" DESTINATION "share/pixmaps")
elseif (WIN32)
  install(TARGETS ${projectName} DESTINATION "bin")
  install(FILES "${PROCALC_DEPENDENCIES_DIR}/bin/tinyxml2.dll" DESTINATION "bin")
  install(DIRECTORY "data" DESTINATION "bin")
  install(FILES "VERSION" DESTINATION ".")
endif()

include(InstallRequiredSystemLibraries)
