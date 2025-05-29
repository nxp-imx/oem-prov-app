include("${CMAKE_CURRENT_LIST_DIR}/cmake_policies.cmake")
include(FetchContent)

if(NOT DEFINED CMAKE_FIND_LIBRARY_PREFIXES)
   set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
endif()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "-DCMAKE_TOOLCHAIN_FILE=<toolchain file> missing")
endif()

if(NOT DEFINED Cyaml_ROOT)
    message(FATAL_ERROR "-DCyaml_ROOT=<cyaml export path> missing")
endif()

if(NOT IS_ABSOLUTE ${Cyaml_ROOT})
    set(Cyaml_ROOT "${CMAKE_SOURCE_DIR}/${Cyaml_ROOT}")
endif()

include(${CMAKE_TOOLCHAIN_FILE})

list(APPEND CMAKE_MODULE_PATH PATHS ./cmake)
include(GNUInstallDirs)
find_package(Cyaml)

if (Cyaml_FOUND)
    message(STATUS "Cyaml library already installed")
    return()
endif()

if(NOT DEFINED CYAML_SRC_PATH)
    message(FATAL_ERROR "-DCYAML_SRC_PATH=<cyaml source path> missing")
endif()

if(NOT IS_ABSOLUTE ${CYAML_SRC_PATH})
    set(CYAML_SRC_PATH "${CMAKE_SOURCE_DIR}/${CYAML_SRC_PATH}")
endif()

if(NOT EXISTS CYAML_REVISION)
    set(CYAML_BRANCH "main")
    set(CYAML_REVISION "07ff8654a270ec9b410acd3152b60de9e9f941af")
endif()

# download source
if(NOT EXISTS ${CYAML_SRC_PATH})
    message(STATUS "Download source for ${CYAML_SRC_PATH}")

    # Use SUBBUILD_DIR to control where temporary files go
    get_filename_component(CYAML_PARENT_DIR ${CYAML_SRC_PATH} DIRECTORY)
    set(TEMP_BUILD_DIR ${CYAML_PARENT_DIR}/cyaml-fetchcontent-temp)

    FetchContent_Populate(
        cyaml
        GIT_REPOSITORY https://github.com/tlsa/libcyaml.git
        GIT_TAG ${CYAML_REVISION}
        SOURCE_DIR ${CYAML_SRC_PATH}
        SUBBUILD_DIR ${TEMP_BUILD_DIR}/subbuild
        BINARY_DIR ${TEMP_BUILD_DIR}/binary
    )

    # Clean up after successful download
    if(EXISTS ${TEMP_BUILD_DIR})
        file(REMOVE_RECURSE ${TEMP_BUILD_DIR})
    endif()
endif()

# Build CYAML library
message(STATUS "Building cyaml in " ${CYAML_SRC_PATH})
set(ENV{CC} ${CMAKE_C_COMPILER})
set(ENV{AR} ${CMAKE_AR})

find_package(Yaml REQUIRED)
get_filename_component(YAML_LIBRARY_DIR ${YAML_LIBRARY} DIRECTORY)

set(ENV{CFLAGS} "-I${YAML_INCLUDE_DIRS}")
set(ENV{LDFLAGS} "-L${YAML_LIBRARY_DIR}")
set(ENV{PREFIX} ${Cyaml_ROOT}/usr)

execute_process(COMMAND make install
                WORKING_DIRECTORY ${CYAML_SRC_PATH}
                RESULT_VARIABLE res)
if(NOT ${res} EQUAL 0)
    message(FATAL_ERROR "Cannot build cyaml: ${res}")
endif()
