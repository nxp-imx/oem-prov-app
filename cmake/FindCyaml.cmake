if (NOT DEFINED CYAML_ROOT)
    message("CYAML_ROOT not defined")
endif()

if (DEFINED CYAML_ROOT AND NOT IS_ABSOLUTE ${CYAML_ROOT})
    set(CYAML_ROOT "${CMAKE_SOURCE_DIR}/${CYAML_ROOT}")
endif()

find_path(CYAML_INCLUDE_DIRS
          NAMES cyaml/cyaml.h
          PATHS ${CYAML_ROOT} ${CYAML_ROOT}/usr
          PATH_SUFFIXES ${CMAKE_INSTALL_INCLUDEDIR} ${CMAKE_INSTALL_INCLUDEDIR}
          CMAKE_FIND_ROOT_PATH_BOTH)

find_library(CYAML_LIBRARY cyaml
             PATHS ${CYAML_ROOT} ${CYAML_ROOT}/usr
             PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR}
             CMAKE_FIND_ROOT_PATH_BOTH)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(${CMAKE_FIND_PACKAGE_NAME} REQUIRED_VARS
                                  CYAML_LIBRARY CYAML_INCLUDE_DIRS)

mark_as_advanced(CYAML_LIBRARY CYAML_INCLUDE_DIRS)

if (CYAML_FOUND AND NOT TARGET LIBCYAML::cyaml)
    add_library(LIBCYAML::cyaml SHARED IMPORTED)
    set_target_properties(LIBCYAML::cyaml
                          PROPERTIES
                          INTERFACE_INCLUDE_DIRECTORIES ${CYAML_INCLUDE_DIRS}
                          IMPORTED_LOCATION ${CYAML_LIBRARY})
endif()