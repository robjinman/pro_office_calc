cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

if (LINUX AND CMAKE_BUILD_TYPE MATCHES Debug)
  include(externalGoogleTest)
elseif (WIN32)
  include(externalTinyXml2)
endif()
