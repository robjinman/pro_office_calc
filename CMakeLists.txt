cmake_minimum_required(VERSION 3.5)

set(PROJECT_SHORT_NAME "procalc")
set(PROJECT_LONG_NAME "Pro Office Calculator")
set(APP_TARGET_NAME "procalc")
set(LIB_TARGET_NAME "procalclib")
set(DATA_DIR "${CMAKE_SOURCE_DIR}/data")
set(VERSION_FILE "${CMAKE_SOURCE_DIR}/VERSION")

project(${PROJECT_SHORT_NAME})

file(STRINGS "${VERSION_FILE}" VERSION_STRINGS)
list(GET VERSION_STRINGS 0 PROJECT_VERSION)

message("============================================================")
message("  Configuring ${PROJECT_LONG_NAME} ${PROJECT_VERSION} - ${CMAKE_BUILD_TYPE} build")
message("============================================================")

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

set(PROCALC_DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/dependencies/build/${BUILD_SUBDIR}/dist")

set(CMAKE_CXX_STANDARD 14)

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

if (USE_LOCAL_DEPS)
  message("Using locally built dependencies")

  set(TINYXML2_INCLUDE_DIRS "${PROCALC_DEPENDENCIES_DIR}/include")
  file(
    GLOB_RECURSE
    TINYXML2_LIBRARIES
    "${PROCALC_DEPENDENCIES_DIR}/lib"
    "${CMAKE_SHARED_LIBRARY_PREFIX}tinyxml2${CMAKE_STATIC_LIBRARY_SUFFIX}"
  )
else()
  find_package(TinyXml2 REQUIRED)
endif()

find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)
find_package(Qt5Multimedia CONFIG REQUIRED)

if (PROFILING_ON)
  message("Profiling ON")

  find_package(GooglePerfTools REQUIRED)

  if (NOT DEFINED PROFILE_DURATION)
    set(PROFILE_DURATION -1)
  endif()

  add_compile_options(-DSINGLE_THREAD -DPROFILING_ON)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed -lprofiler -Wl,--as-needed")
endif()

file(GLOB_RECURSE srcs "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM srcs "${CMAKE_SOURCE_DIR}/src/main.cpp")

include_directories("${CMAKE_SOURCE_DIR}/src")
include_directories("${PROCALC_DEPENDENCIES_DIR}/include")
include_directories("${QT_DIR}/include")
include_directories("${TINYXML2_INCLUDE_DIRS}")

link_directories("${PROCALC_DEPENDENCIES_DIR}/bin")
link_directories("${QT_DIR}/bin")

if (WIN32)
  add_compile_options(-O2)
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
elseif (APPLE)
  add_compile_options(-O3 -Wall -Wextra)
  include_directories("/usr/local/opt/llvm/include")
  link_directories("/usr/local/opt/llvm/lib")
else()
  add_compile_options(-O3 -Wall -Wextra)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
endif()

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
  message("Release mode")
else()
  message("Debug mode")
  add_compile_options(-DDEBUG)

  if (NOT WIN32)
    add_compile_options(-g)
  endif()
endif()

add_library(${LIB_TARGET_NAME} ${srcs})

target_link_libraries(${LIB_TARGET_NAME} ${TINYXML2_LIBRARIES})
target_link_libraries(${LIB_TARGET_NAME} Qt5::Core)
target_link_libraries(${LIB_TARGET_NAME} Qt5::Widgets)
target_link_libraries(${LIB_TARGET_NAME} Qt5::Gui)
target_link_libraries(${LIB_TARGET_NAME} Qt5::Multimedia)

if (APPLE)
  set(ICON_FILE "${CMAKE_SOURCE_DIR}/icons/${PROJECT_SHORT_NAME}.icns")

  set(MACOSX_BUNDLE true)
  set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_LONG_NAME}")
  set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_LONG_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_ICON_FILE "procalc.icns")

  set_source_files_properties("${ICON_FILE}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  set_source_files_properties("${VERSION_FILE}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

  add_executable(${APP_TARGET_NAME} MACOSX_BUNDLE
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    ${DATA_FILES}
    "${ICON_FILE}"
    "${VERSION_FILE}")
else()
  add_executable(${APP_TARGET_NAME} "${CMAKE_SOURCE_DIR}/src/main.cpp")
endif()

target_link_libraries(${APP_TARGET_NAME} ${LIB_TARGET_NAME})

file(COPY "${DATA_DIR}" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "${VERSION_FILE}" DESTINATION "${CMAKE_BINARY_DIR}")

if (LINUX)
  install(TARGETS ${PROJECT_SHORT_NAME} DESTINATION "bin")

  install(DIRECTORY "${DATA_DIR}" DESTINATION "share/${PROJECT_SHORT_NAME}")
  install(FILES "${VERSION_FILE}" DESTINATION "share/${PROJECT_SHORT_NAME}")
  install(FILES "${CMAKE_SOURCE_DIR}/${PROJECT_SHORT_NAME}.desktop" DESTINATION "share/applications")
  install(FILES "${CMAKE_SOURCE_DIR}/icons/${PROJECT_SHORT_NAME}.svg" DESTINATION "share/icons/hicolor/scalable/apps")
  install(FILES "${CMAKE_SOURCE_DIR}/icons/16/${PROJECT_SHORT_NAME}.png" DESTINATION "share/icons/hicolor/16x16/apps")
  install(FILES "${CMAKE_SOURCE_DIR}/icons/22/${PROJECT_SHORT_NAME}.png" DESTINATION "share/icons/hicolor/22x22/apps")
  install(FILES "${CMAKE_SOURCE_DIR}/icons/32/${PROJECT_SHORT_NAME}.png" DESTINATION "share/icons/hicolor/32x32/apps")
  install(FILES "${CMAKE_SOURCE_DIR}/icons/48/${PROJECT_SHORT_NAME}.png" DESTINATION "share/icons/hicolor/48x48/apps")
  install(FILES "${CMAKE_SOURCE_DIR}/icons/64/${PROJECT_SHORT_NAME}.png" DESTINATION "share/icons/hicolor/64x64/apps")
  install(FILES "${CMAKE_SOURCE_DIR}/icons/128/${PROJECT_SHORT_NAME}.png" DESTINATION "share/icons/hicolor/128x128/apps")
  install(FILES "${CMAKE_SOURCE_DIR}/icons/256/${PROJECT_SHORT_NAME}.png" DESTINATION "share/icons/hicolor/256x256/apps")

  install(FILES "${CMAKE_SOURCE_DIR}/${PROJECT_SHORT_NAME}.sh" DESTINATION "games" RENAME "${PROJECT_SHORT_NAME}")
elseif (WIN32)
  set(MANIFEST_PATH "${CMAKE_SOURCE_DIR}/windows/dpi_unaware.manifest")

	ADD_CUSTOM_COMMAND(
		TARGET procalc
		POST_BUILD
    COMMAND "mt.exe" -manifest \"${MANIFEST_PATH}\" -inputresource:\"$<TARGET_FILE:procalc>\"\;\#1 -outputresource:\"$<TARGET_FILE:procalc>\"\;\#1)

  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")

  install(TARGETS ${PROJECT_SHORT_NAME} DESTINATION ".")
  install(FILES "${PROCALC_DEPENDENCIES_DIR}/bin/tinyxml2.dll" DESTINATION ".")
  install(DIRECTORY "${DATA_DIR}" DESTINATION ".")
  install(FILES "${VERSION_FILE}" DESTINATION ".")
  install(FILES "${CMAKE_SOURCE_DIR}/icons/${PROJECT_SHORT_NAME}.ico" DESTINATION ".")
elseif (APPLE)
  install(CODE "set(QT_DIR \"${QT_DIR}\")")
  install(CODE "set(DATA_DIR \"${DATA_DIR}\")")

  install(SCRIPT "${CMAKE_MODULE_PATH}/osx_bundle.cmake")
endif()

include(InstallRequiredSystemLibraries)
