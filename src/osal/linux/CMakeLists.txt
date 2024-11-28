add_library(osal OBJECT "")

target_sources(osal PRIVATE
               oem_prov_config.c
               oem_prov_utils.c
               oem_prov_os.c
               oem_prov_part.c
               oem_prov_objects_file.c)

target_include_directories(osal
                           PRIVATE ${CMAKE_SOURCE_DIR}/include)