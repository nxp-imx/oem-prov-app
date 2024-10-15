if (NOT DEFINED EL2GOAGENT_ROOT)
    message("EL2GOAGENT_ROOT not defined")
endif()

if (DEFINED EL2GOAGENT_ROOT AND NOT IS_ABSOLUTE ${EL2GOAGENT_ROOT})
    set(EL2GOAGENT_ROOT "${CMAKE_SOURCE_DIR}/${EL2GOAGENT_ROOT}")
endif()

find_path(EL2GOAgent_AGENT_INCLUDE_DIRS
          NAMES nxp_iot_agent_config.h
          PATHS ${EL2GOAGENT_ROOT} ${EL2GOAGENT_ROOT}/usr
          PATH_SUFFIXES ${CMAKE_INSTALL_INCLUDEDIR} ${CMAKE_INSTALL_INCLUDEDIR}/el2go-agent/nxp-iot-agent
          CMAKE_FIND_ROOT_PATH_BOTH)

find_path(EL2GOAgent_PROTOBUF_INCLUDE_DIRS
          NAMES Agent.pb.h
          PATHS ${EL2GOAGENT_ROOT} ${EL2GOAGENT_ROOT}/usr
          PATH_SUFFIXES ${CMAKE_INSTALL_INCLUDEDIR} ${CMAKE_INSTALL_INCLUDEDIR}/el2go-agent/protobuf
          CMAKE_FIND_ROOT_PATH_BOTH)

list(APPEND EL2GOAgent_INCLUDE_DIRS ${EL2GOAgent_AGENT_INCLUDE_DIRS} ${EL2GOAgent_PROTOBUF_INCLUDE_DIRS})

find_library(EL2GOAGENT_COMMON_LIBRARY nxp_iot_agent_common
             PATHS ${EL2GOAGENT_ROOT} ${EL2GOAGENT_ROOT}/usr
             PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR}
             CMAKE_FIND_ROOT_PATH_BOTH)

find_library(EL2GOAGENT_LIBRARY nxp_iot_agent
             PATHS ${EL2GOAGENT_ROOT} ${EL2GOAGENT_ROOT}/usr
             PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR}
             CMAKE_FIND_ROOT_PATH_BOTH)

list(APPEND EL2GOAgent_LIBRARIES ${EL2GOAGENT_LIBRARY} ${EL2GOAGENT_COMMON_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(${CMAKE_FIND_PACKAGE_NAME}
                                  REQUIRED_VARS
                                  EL2GOAgent_AGENT_INCLUDE_DIRS
                                  EL2GOAgent_PROTOBUF_INCLUDE_DIRS
                                  EL2GOAGENT_COMMON_LIBRARY
                                  EL2GOAGENT_LIBRARY
                                  EL2GOAgent_LIBRARIES
                                  EL2GOAgent_INCLUDE_DIRS)

mark_as_advanced(EL2GOAgent_AGENT_INCLUDE_DIRS EL2GOAgent_PROTOBUF_INCLUDE_DIRS EL2GOAGENT_LIBRARY EL2GOAGENT_COMMON_LIBRARY)
