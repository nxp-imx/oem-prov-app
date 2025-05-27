cmake_minimum_required(VERSION 3.13)

project(oem-prov-app VERSION 1.1 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)
set(CMAKE_INSTALL_PREFIX /usr)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

message(STATUS "Build type ${CMAKE_BUILD_TYPE}.")

if(VERBOSE)
    if(NOT VERBOSE MATCHES "^[0-9]$")
        message(FATAL_ERROR, "VERBOSE must be an integer")
    endif()

    if (VERBOSE GREATER 0)
        add_definitions(-DENABLE_VERBOSE)
    endif()

    if (VERBOSE GREATER 4)
        message(WARNING, "Verbose values should be less than 5, setting the VERBOSE to 4")
        set(VERBOSE 4)
    endif()

    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(${VERBOSE} GREATER 2)
            set(VERBOSE 2)
        endif()
    endif()

    add_definitions(-DVERBOSE_LEVEL=${VERBOSE})
else()
    set(VERBOSE 0)
endif()

# used for testing purposes only
# Commit storage and close operations have irreversible side effects
# This define can be use for interface testing, but omitting the irreversible
# operations.
if (CLOSE_COMMIT_DRY_RUN)
	add_definitions(-DOEM_PROV_DRY_RUN)
endif()

# Create the version file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/oem_prov_version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/oem_prov_version.h)

include_directories(include
                    ${CMAKE_CURRENT_BINARY_DIR}/include)
add_subdirectory(src)

