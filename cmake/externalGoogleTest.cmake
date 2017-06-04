include(ExternalProject)

set(GOOGLE_TEST_CMAKE_ARGS
  -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
  -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
  -DBUILD_SHARED_LIBS=FALSE
)

ExternalProject_Add(googleTest
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/srcs/googleTest

  URL https://github.com/google/googletest/archive/release-1.7.0.zip
  DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/downloads

  INSTALL_COMMAND ""
  CMAKE_ARGS ${GOOGLE_TEST_CMAKE_ARGS}
)

set(LIBS_DIR ${CMAKE_CURRENT_BINARY_DIR}/srcs/googleTest/src/googleTest-build)

add_custom_command(
  TARGET googleTest POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${LIBS_DIR}/libgtest.a ${CMAKE_CURRENT_BINARY_DIR}/googleTest/lib/libgtest.a
  COMMAND ${CMAKE_COMMAND} -E copy ${LIBS_DIR}/libgtest_main.a ${CMAKE_CURRENT_BINARY_DIR}/googleTest/lib/libgtest_main.a
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/srcs/googleTest/src/googleTest/include ${CMAKE_CURRENT_BINARY_DIR}/googleTest/include
)
