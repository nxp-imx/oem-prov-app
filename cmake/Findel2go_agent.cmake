find_path(el2go_agent_AGENT_INCLUDE_DIRS
          NAMES nxp_iot_agent_config.h
          PATH_SUFFIXES ${CMAKE_INSTALL_INCLUDEDIR}/el2go-agent/nxp-iot-agent /usr/${CMAKE_INSTALL_INCLUDEDIR}/el2go-agent/nxp-iot-agent
          CMAKE_FIND_ROOT_PATH_BOTH)

find_path(el2go_agent_PROTOBUF_INCLUDE_DIRS
          NAMES Agent.pb.h
          PATH_SUFFIXES ${CMAKE_INSTALL_INCLUDEDIR}/el2go-agent/protobuf /usr/${CMAKE_INSTALL_INCLUDEDIR}/el2go-agent/protobuf
          CMAKE_FIND_ROOT_PATH_BOTH)

list(APPEND el2go_agent_INCLUDE_DIRS ${el2go_agent_AGENT_INCLUDE_DIRS} ${el2go_agent_PROTOBUF_INCLUDE_DIRS})

find_library(el2go_agent_COMMON_LIBRARY nxp_iot_agent_common
             PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR} /usr/${CMAKE_INSTALL_LIBDIR}
             CMAKE_FIND_ROOT_PATH_BOTH)

find_library(el2go_agent_LIBRARY nxp_iot_agent
             PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR} /usr/${CMAKE_INSTALL_LIBDIR}
             CMAKE_FIND_ROOT_PATH_BOTH)

list(APPEND el2go_agent_LIBRARIES ${el2go_agent_LIBRARY} ${el2go_agent_COMMON_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(${CMAKE_FIND_PACKAGE_NAME}
                                  REQUIRED_VARS
                                  el2go_agent_AGENT_INCLUDE_DIRS
                                  el2go_agent_PROTOBUF_INCLUDE_DIRS
                                  el2go_agent_COMMON_LIBRARY
                                  el2go_agent_LIBRARY
                                  el2go_agent_LIBRARIES
                                  el2go_agent_INCLUDE_DIRS)

mark_as_advanced(el2go_agent_AGENT_INCLUDE_DIRS el2go_agent_PROTOBUF_INCLUDE_DIRS el2go_agent_LIBRARY el2go_agent_COMMON_LIBRARY)