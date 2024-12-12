add_library(utils OBJECT "")

target_sources(utils PRIVATE
               oem_prov_list.c
               oem_prov_utils.c)

target_include_directories(utils
                           PRIVATE ${CMAKE_SOURCE_DIR}/include)