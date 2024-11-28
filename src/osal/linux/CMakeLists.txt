add_library(osal OBJECT "")

target_sources(osal PRIVATE
               oem_prov_config.c
               oem_prov_utils.c
               oem_prov_os.c)

target_include_directories(osal
                           PRIVATE ${CMAKE_SOURCE_DIR}/include)