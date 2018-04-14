cmake_minimum_required(VERSION 3.5)

set(projectName procalc)

project(${projectName})

if (WIN32)
  set(QT_DIR "C:/Qt/5.10.1/msvc2017_64")
  set(BUILD_SUBDIR "win64")

  add_compile_options(-DWIN32)
else()
  set(BUILD_SUBDIR "linux")
endif()

set(CMAKE_CXX_STANDARD 14)

set(PROCALC_DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/dependencies/build/${BUILD_SUBDIR}/dist")
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
include_directories("${TINYXML2_INCLUDE_DIRS}")

link_directories("${PROCALC_DEPENDENCIES_DIR}/bin")
link_directories("${QT_DIR}/bin")

add_compile_options(${OpenMP_CXX_FLAGS})

if (WIN32)
  add_compile_options(-O2)
else()
  add_compile_options(-O3)
endif()

if (CMAKE_BUILD_TYPE EQUAL "Release")
  message("Release mode")
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

if (NOT WIN32)
  target_link_libraries(procalclib "${OpenMP_CXX_FLAGS}")
endif()
target_link_libraries(procalclib ${TINYXML2_LIBRARIES})
target_link_libraries(procalclib Qt5::Core)
target_link_libraries(procalclib Qt5::Widgets)
target_link_libraries(procalclib Qt5::Gui)
target_link_libraries(procalclib Qt5::Multimedia)

add_executable(procalc "src/main.cpp")

target_link_libraries(procalc procalclib)

file(COPY "data" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "VERSION" DESTINATION "${CMAKE_BINARY_DIR}/data")

if (UNIX)
  file(COPY "run.sh" DESTINATION "${CMAKE_BINARY_DIR}")

  install(TARGETS ${projectName} DESTINATION "bin")

  install(DIRECTORY "data" DESTINATION "share/${projectName}")
  install(FILES "VERSION" DESTINATION "share/${projectName}")
  install(FILES "${projectName}.desktop" DESTINATION "share/applications")
  install(FILES "${projectName}.svg" DESTINATION "share/pixmaps")

  install(FILES "${projectName}.sh" DESTINATION "games" RENAME "${projectName}")
elseif (WIN32)
  install(TARGETS ${projectName} DESTINATION "bin")
  install(FILES "${PROCALC_DEPENDENCIES_DIR}/bin/tinyxml2.dll" DESTINATION "bin")
  install(DIRECTORY "data" DESTINATION "bin")
  install(FILES "VERSION" DESTINATION ".")
endif()

include(InstallRequiredSystemLibraries)
