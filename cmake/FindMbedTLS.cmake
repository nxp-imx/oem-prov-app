set(CMAKE_FIND_LIBRARY_SUFFIXES .so .a)

find_path(MbedTLS_INCLUDE_DIRS
          NAMES mbedtls/ssl.h
          PATH_SUFFIXES local/el2go/include usr/local/el2go/include
          CMAKE_FIND_ROOT_PATH_BOTH)

find_library(MbedTLS_LIBRARY mbedtls
             PATH_SUFFIXES local/el2go/lib usr/local/el2go/lib
             CMAKE_FIND_ROOT_PATH_BOTH)

find_library(MbedTLS_CRYPTO_LIBRARY mbedcrypto
             PATH_SUFFIXES local/el2go/lib usr/local/el2go/lib
             CMAKE_FIND_ROOT_PATH_BOTH)

find_library(MbedTLS_X509_LIBRARY mbedx509
             PATH_SUFFIXES local/el2go/lib usr/local/el2go/lib
             CMAKE_FIND_ROOT_PATH_BOTH)

set(MbedTLS_LIBRARIES
    ${MbedTLS_LIBRARY}
    ${MbedTLS_CRYPTO_LIBRARY}
    ${MbedTLS_X509_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(${CMAKE_FIND_PACKAGE_NAME} REQUIRED_VARS
                                  MbedTLS_LIBRARIES MbedTLS_INCLUDE_DIRS)

mark_as_advanced(MbedTLS_LIBRARIES MbedTLS_INCLUDE_DIRS)
