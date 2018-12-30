cmake_minimum_required(VERSION 3.5)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  message("No install prefix set; will install to ./dist")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/dist")
endif()

if (UNIX AND NOT APPLE)
  set(LINUX true)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

include(externalTinyXml2)

if (LINUX)
  include(externalGoogleTest)
endif()
