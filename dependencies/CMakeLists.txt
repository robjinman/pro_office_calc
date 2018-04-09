cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

include(externalTinyXml2)
if (UNIX)
  include(externalGoogleTest)
endif()
