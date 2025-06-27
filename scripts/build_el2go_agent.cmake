include("${CMAKE_CURRENT_LIST_DIR}/cmake_policies.cmake")

if(NOT DEFINED CMAKE_FIND_LIBRARY_PREFIXES)
   set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
endif()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "-DCMAKE_TOOLCHAIN_FILE=<toolchain file> missing")
endif()

if (NOT DEFINED el2go_agent_ROOT)
    message(FATAL_ERROR "el2go_agent_ROOT not defined")
endif()

if (DEFINED el2go_agent_ROOT AND NOT IS_ABSOLUTE ${el2go_agent_ROOT})
    set(el2go_agent_ROOT "${CMAKE_SOURCE_DIR}/${el2go_agent_ROOT}")
endif()

if (NOT DEFINED el2go_agent_BUILD_DIR)
    message(FATAL_ERROR "el2go_agent_BUILD_DIR not defined")
endif()

if (DEFINED el2go_agent_BUILD_DIR AND NOT IS_ABSOLUTE ${el2go_agent_BUILD_DIR})
    set(el2go_agent_BUILD_DIR "${CMAKE_SOURCE_DIR}/${el2go_agent_BUILD_DIR}")
endif()

include(${CMAKE_TOOLCHAIN_FILE})

list(APPEND CMAKE_MODULE_PATH PATHS ./cmake)
include(GNUInstallDirs)
find_package(el2go_agent)

if (el2go_agent_FOUND)
    message(STATUS "EL2GO Agent libraries already installed")
    return()
endif()

if(NOT DEFINED el2go_agent_SRC_PATH)
    message(FATAL_ERROR "-Del2go_agent_SRC_PATH=<EL2GO Agent source path> missing")
endif()

if(NOT IS_ABSOLUTE ${el2go_agent_SRC_PATH})
    set(el2go_agent_SRC_PATH "${CMAKE_SOURCE_DIR}/${el2go_agent_SRC_PATH}")
endif()

# Build EL2GO library
message(STATUS "Building EL2GO Agent in " ${el2go_agent_SRC_PATH})
set(ENV{CC} ${CMAKE_C_COMPILER})
set(ENV{AR} ${CMAKE_AR})

execute_process(COMMAND echo $ENV{LDFLAGS})
set(el2go_agent_BUILD_DIR ${el2go_agent_BUILD_DIR}/el2go_agent-build)

set(el2go_agent_CMAKE_ARGS
    -DSSS_HAVE_HOSTCRYPTO_MBEDTLS=ON
    -DNXP_IOT_AGENT_HAVE_PSA_IMPL_SMW=ON
    -DSSS_HAVE_MBEDTLS_ALT_PSA=ON
    -DSSS_HAVE_HOST_LINUX_LIKE=ON
    -DNXP_SMW_DIR=${NXP_SMW_DIR}
    -DMbedTLS_DIR=${MbedTLS_DIR}
    -DCMAKE_INSTALL_PREFIX=${el2go_agent_ROOT}/usr
    -DCMAKE_INSTALL_LIBDIR=lib
    -DCMAKE_C_FLAGS="-DMBEDTLS_USE_PSA_CRYPTO")

    message(STATUS "Executing cmake ${CMAKE_COMMAND} ${el2go_agent_CMAKE_ARGS} ${el2go_agent_SRC_PATH}")
    execute_process(COMMAND ${CMAKE_COMMAND} ${el2go_agent_CMAKE_ARGS} ${el2go_agent_SRC_PATH} -B  ${el2go_agent_BUILD_DIR}
                    WORKING_DIRECTORY ${el2go_agent_SRC_PATH}
                    RESULT_VARIABLE res)
    if(NOT ${res} EQUAL 0)
        message(FATAL_ERROR "Cannot execute cmake: ${res}")
    endif()

    message(STATUS "Building EL2GO Agent library")
    execute_process(COMMAND make install
                    WORKING_DIRECTORY ${el2go_agent_BUILD_DIR}
                    RESULT_VARIABLE res)
    if(NOT ${res} EQUAL 0)
        message(FATAL_ERROR "Cannot build EL2GO Agent: ${res}")
    endif()


