# - Try to find TinyXml2
# Once done this will define
#  TINYXML2_FOUND - System has TinyXml2
#  TINYXML2_INCLUDE_DIRS - The TinyXml2 include directories
#  TINYXML2_LIBRARIES - The libraries needed to use TinyXml2
#  TINYXML2_DEFINITIONS - Compiler switches required for using TinyXml2

find_package(PkgConfig)
pkg_check_modules(PC_LIBXML QUIET libtinyxml2)
set(TINYXML2_DEFINITIONS ${PC_TINYXML_CFLAGS_OTHER})

find_path(TINYXML2_INCLUDE_DIR tinyxml2.h
          HINTS ${PC_TINYXML_INCLUDEDIR} ${PC_TINYXML_INCLUDE_DIRS}
          PATH_SUFFIXES tinyxml2)

find_library(TINYXML2_LIBRARY NAMES libtinyxml2 tinyxml2
             HINTS ${PC_TINYXML_LIBDIR} ${PC_TINYXML_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set TINYXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(TinyXml2 DEFAULT_MSG
                                  TINYXML2_LIBRARY TINYXML2_INCLUDE_DIR)

mark_as_advanced(TINYXML2_INCLUDE_DIR TINYXML2_LIBRARY)

set(TINYXML2_LIBRARIES ${TINYXML2_LIBRARY})
set(TINYXML2_INCLUDE_DIRS ${TINYXML2_INCLUDE_DIR})
