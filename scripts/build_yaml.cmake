include("${CMAKE_CURRENT_LIST_DIR}/cmake_policies.cmake")

if(NOT DEFINED CMAKE_FIND_LIBRARY_PREFIXES)
   set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
endif()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "-DCMAKE_TOOLCHAIN_FILE=<toolchain file> missing")
endif()

if(NOT DEFINED Yaml_ROOT)
    message(FATAL_ERROR "-DYaml_ROOT=<libyaml export path> missing")
endif()

if(NOT IS_ABSOLUTE ${Yaml_ROOT})
    set(Yaml_ROOT "${CMAKE_SOURCE_DIR}/${Yaml_ROOT}")
endif()

include(${CMAKE_TOOLCHAIN_FILE})

list(APPEND CMAKE_MODULE_PATH PATHS ./cmake)
include(GNUInstallDirs)
find_package(Yaml)

if (YAML_FOUND)
    message(STATUS "Yaml library already installed")
    return()
endif()

if(NOT DEFINED YAML_SRC_PATH)
    message(FATAL_ERROR "-YAML_SRC_PATH=<libyaml source path> missing")
endif()

if(NOT IS_ABSOLUTE ${YAML_SRC_PATH})
    set(YAML_SRC_PATH "${CMAKE_SOURCE_DIR}/${YAML_SRC_PATH}")
endif()

if(NOT YAML_VERSION)
    set(YAML_VERSION "0.2.5" CACHE STRING "Default Yaml version")
    set(YAML_HASH "SHA256=c642ae9b75fee120b2d96c712538bd2cf283228d2337df2cf2988e3c02678ef4")
endif()
set(YAML_SRC "${YAML_SRC_PATH}/yaml-${YAML_VERSION}")

if(NOT EXISTS ${YAML_SRC})
    set(YAML_URL "http://pyyaml.org/download/libyaml")
    set(YAML_ARCHIVE "yaml-${YAML_VERSION}.tar.gz")

    find_file(YAML_ARCHIVE_PATH ${YAML_ARCHIVE} ${YAML_SRC_PATH})
    if (NOT ${YAML_ARCHIVE_PATH})
        message(STATUS "Downloading yaml library sources from " ${YAML_URL})
        file(DOWNLOAD
             "${YAML_URL}/${YAML_ARCHIVE}"
             "${YAML_SRC_PATH}/${YAML_ARCHIVE}"
              EXPECTED_HASH ${YAML_HASH}
              STATUS DL_STATUS)
        list(GET DL_STATUS 0 DL_RESULT)
        if(NOT ${DL_RESULT} EQUAL 0)
            message(FATAL_ERROR "Failed to download libyaml: ${DL_STATUS}")
        endif()
    endif()

    message(STATUS "Extracting ${YAML_ARCHIVE}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzvf ${YAML_ARCHIVE}
                    WORKING_DIRECTORY ${YAML_SRC_PATH}
                    RESULT_VARIABLE RES)
    if(NOT ${RES} EQUAL 0)
       message(FATAL_ERROR "Cannot extract libyaml:" ${RES})
     endif()
endif()

# Build YAML library
set(ENV{CC} ${CMAKE_C_COMPILER})
set(ENV{AR} ${CMAKE_AR})

if(NOT DEFINED TOOLCHAIN_NAME)
    message(FATAL_ERROR "TOOLCHAIN_NAME must be defined for cross-compilation")
endif()

set(YAML_CONFIGURE "./configure")
set(YAML_CONFIGURE_ARGS "--prefix=${Yaml_ROOT}/usr" "--with-sysroot=${Yaml_ROOT}/usr"
    "--host=${TOOLCHAIN_NAME}")

execute_process(COMMAND ${YAML_CONFIGURE} ${YAML_CONFIGURE_ARGS}
                WORKING_DIRECTORY ${YAML_SRC}
                RESULT_VARIABLE res)

if(NOT ${res} EQUAL 0)
    message(FATAL_ERROR "Cannot configure libyaml: ${res}")
endif()

message(STATUS "Building and installing ${YAML_ARCHIVE_PATH}")
set(YAML_MAKE_ARGS install)
execute_process(COMMAND make ${YAML_MAKE_ARGS}
                WORKING_DIRECTORY ${YAML_SRC}
                RESULT_VARIABLE res)

if(NOT ${res} EQUAL 0)
    message(FATAL_ERROR "Cannot build libyaml: ${res}")
endif()
