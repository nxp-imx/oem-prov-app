add_executable(${PROJECT_NAME} "")

target_sources(${PROJECT_NAME} PRIVATE
               oem_prov_app_main.c)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${INCLUDE_DIR})

install(TARGETS ${PROJECT_NAME} RUNTIME)
