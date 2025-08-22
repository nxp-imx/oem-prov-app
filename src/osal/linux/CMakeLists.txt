find_package(PkgConfig REQUIRED)
pkg_check_modules(CYAML REQUIRED IMPORTED_TARGET libcyaml)

add_library(osal OBJECT "")

target_sources(osal PRIVATE
               oem_prov_config.c
               oem_prov_os_utils.c
               oem_prov_os.c
               oem_prov_part.c
               oem_prov_file.c
               $<$<BOOL:${VERBOSE}>:oem_prov_debug.c>)

target_include_directories(osal
                           PRIVATE ${CMAKE_SOURCE_DIR}/include
                           PRIVATE ${CYAML_INCLUDE_DIRS})

target_link_libraries(osal
                      PRIVATE PkgConfig::CYAML)

