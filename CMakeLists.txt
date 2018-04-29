cmake_minimum_required(VERSION 3.5)

set(projectName procalc)

project(${projectName})

message("***** Building ${CMAKE_BUILD_TYPE} build *****")

if (UNIX AND NOT APPLE)
  set(LINUX true)
endif()

if (WIN32)
  set(QT_DIR "C:/Qt/5.10.1/msvc2017_64")
  set(BUILD_SUBDIR "win64")

  add_compile_options(-DWIN32)
elseif (APPLE)
  set(QT_DIR "/usr/local/Cellar/qt/5.10.1")
  set(BUILD_SUBDIR "osx")
else()
  set(BUILD_SUBDIR "linux")
endif()

set(CMAKE_CXX_STANDARD 14)

set(PROCALC_DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/dependencies/build/${BUILD_SUBDIR}/dist")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${PROCALC_DEPENDENCIES_DIR}/lib/cmake")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${QT_DIR}/lib/cmake")

if (LINUX AND CMAKE_BUILD_TYPE MATCHES Debug)
  add_subdirectory(tools)
  add_subdirectory(tests)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if (WIN32)
  set(TINYXML2_INCLUDE_DIRS "${PROCALC_DEPENDENCIES_DIR}/include")
  set(TINYXML2_LIBRARIES "${PROCALC_DEPENDENCIES_DIR}/lib/tinyxml2.lib")
else()
  find_package(TinyXml2 REQUIRED)
endif()
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)
find_package(Qt5Multimedia CONFIG REQUIRED)
find_package(OpenMP)
if (OPENMP_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

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

if (WIN32)
  add_compile_options(-O2)
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
else()
  add_compile_options(-O3)
endif()

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
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
  file(COPY "run.bat" DESTINATION "${CMAKE_BINARY_DIR}")

  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "libs")

  install(TARGETS ${projectName} DESTINATION ".")
  install(FILES "${PROCALC_DEPENDENCIES_DIR}/bin/tinyxml2.dll" DESTINATION "libs")
  install(DIRECTORY "data" DESTINATION ".")
  install(FILES "VERSION" DESTINATION ".")
  install(FILES "procalc.ico" DESTINATION ".")
  install(FILES "run.bat" DESTINATION ".")
endif()

include(InstallRequiredSystemLibraries)
