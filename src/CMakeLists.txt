find_package(NXP_SMW CONFIG REQUIRED)
find_package(Cyaml REQUIRED)
find_package(MbedTLS CONFIG REQUIRED)
find_package(EL2GOAgent REQUIRED)

add_executable(${PROJECT_NAME} "")

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

target_sources(${PROJECT_NAME} PRIVATE
               oem_prov_app_main.c
               oem_prov_utils.c
               oem_prov_setup.c
               oem_prov_claim_code.c
               oem_prov_config.c
               oem_prov_online.c)

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${INCLUDE_DIR}
                           ${EL2GOAgent_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
                      NXP_SMW::smw
                      LIBCYAML::cyaml
                      ${EL2GOAgent_LIBRARIES}
                      MbedTLS::mbedtls
                      MbedTLS::mbedx509
                      MbedTLS::mbedcrypto)

install(TARGETS ${PROJECT_NAME} RUNTIME)
