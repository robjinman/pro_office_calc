cmake_minimum_required(VERSION 3.5)

if (UNIX AND NOT APPLE)
  set(LINUX true)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

include(externalTinyXml2)

if (LINUX)
  include(externalGoogleTest)
endif()
