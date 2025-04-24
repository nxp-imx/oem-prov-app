find_package(NXP_SMW CONFIG REQUIRED)
find_package(el2go_agent CONFIG REQUIRED)

add_executable(${PROJECT_NAME} "")

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(osal/linux)
add_subdirectory(utils)

target_sources(${PROJECT_NAME} PRIVATE
               oem_prov_app_main.c
               oem_prov_uuid.c
               oem_prov_setup.c
               oem_prov_claim_code.c
               oem_prov_online.c
               oem_prov_obj_parser.c
               oem_prov_obj_import.c
               oem_prov_indirect.c
               oem_prov_dev_mgr.c
               oem_prov_storage.c)

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
                      NXP_SMW::smw
                      ${EXT_LIBS}
                      el2go_agent::nxp_iot_agent_common
                      el2go_agent::nxp_iot_agent
                      utils
                      osal)

install(TARGETS ${PROJECT_NAME} RUNTIME)