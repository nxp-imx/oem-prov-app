find_package(NXP_SMW CONFIG REQUIRED)
find_package(Cyaml REQUIRED)
find_package(EL2GOAgent REQUIRED)

add_executable(${PROJECT_NAME} "")

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(osal/linux)

target_sources(${PROJECT_NAME} PRIVATE
               oem_prov_app_main.c
               oem_prov_uuid.c
               oem_prov_setup.c
               oem_prov_claim_code.c
               oem_prov_online.c
               oem_prov_indirect.c)

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${INCLUDE_DIR}
                           ${EL2GOAgent_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
                      NXP_SMW::smw
                      LIBCYAML::cyaml
                      ${EL2GOAgent_LIBRARIES}
                      osal)

install(TARGETS ${PROJECT_NAME} RUNTIME)