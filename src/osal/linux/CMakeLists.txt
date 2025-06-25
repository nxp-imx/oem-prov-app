find_package(Cyaml REQUIRED)
add_library(osal OBJECT "")

target_sources(osal PRIVATE
               oem_prov_config.c
               oem_prov_os_utils.c
               oem_prov_os.c
               oem_prov_part.c
               oem_prov_file.c
               oem_prov_debug.c)

target_include_directories(osal
                           PRIVATE ${CMAKE_SOURCE_DIR}/include
                           PRIVATE ${CYAML_INCLUDE_DIRS})

set(EXT_LIBS ${EXT_LIBS} ${CYAML_LIBRARY} PARENT_SCOPE)