cmake_minimum_required(VERSION 3.5)

if (UNIX AND NOT APPLE)
  set(LINUX true)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

# For Apple, install dependency via homebrew
if (NOT APPLE)
  include(externalTinyXml2)
endif()

if (LINUX)
  include(externalGoogleTest)
endif()
