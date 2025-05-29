find_path(CYAML_INCLUDE_DIRS
          NAMES cyaml/cyaml.h
          PATH_SUFFIXES ${CMAKE_INSTALL_INCLUDEDIR} usr/${CMAKE_INSTALL_INCLUDEDIR}
          CMAKE_FIND_ROOT_PATH_BOTH)

find_library(CYAML_LIBRARY cyaml
             PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR} usr/${CMAKE_INSTALL_LIBDIR}
             CMAKE_FIND_ROOT_PATH_BOTH)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(${CMAKE_FIND_PACKAGE_NAME} REQUIRED_VARS
                                  CYAML_LIBRARY CYAML_INCLUDE_DIRS)

mark_as_advanced(CYAML_LIBRARY CYAML_INCLUDE_DIRS)
