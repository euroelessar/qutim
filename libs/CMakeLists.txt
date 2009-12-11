CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

project( gloox )

SET( CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Jabber/XMPP library" )
SET( CPACK_PACKAGE_VENDOR "Jakob Schröter" )
SET( CPACK_PACKAGE_DESCRIPTION_FILE "../libs/gloox/README" )
SET( CPACK_RESOURCE_FILE_LICENSE "../libs/gloox/LICENSE" )
SET( CPACK_PACKAGE_VERSION_MAJOR "1" )
SET( CPACK_PACKAGE_VERSION_MINOR "1" )
SET( CPACK_PACKAGE_VERSION_PATCH "svn" )
SET( CPACK_SOURCE_GENERATOR "TBZ2" )
set( CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}-${CPACK_PACKAGE_VERSION_PATCH}" )
set( CPACK_SOURCE_IGNORE_FILES "/build/;/.svn/;~$;${CPACK_SOURCE_IGNORE_FILES}" )
SET( CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_PROJECT_NAME}-${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}-${CMake_VERSION_PATCH}" )
include( CPack )


set( GLOOX_ENABLE_MAINTAINER false CACHE BOOL "Enable Maintainer Mode" )
set( GLOOX_ENABLE_GETADDRINFO false CACHE BOOL "Enable getaddrinfo()" )
if( WIN32 )
        set( GLOOX_WITH_SCHANNEL false CACHE BOOL "Enable SChannel" )
endif( WIN32 )

if( GLOOX_ENABLE_MAINTAINER )
        set( CMAKE_CXX_FLAGS "-Wshadow -Werror" CACHE STRING "Maintainer Flags" FORCE )
endif( GLOOX_ENABLE_MAINTAINER )

set( CMAKE_CXX_FLAGS_DEBUG "-g3 -fno-inline -DDEBUG" CACHE STRING "Debug Flags" FORCE )

find_package( GNUTLS )
find_package( OpenSSL )
find_package( ZLIB )
find_package( IDN )
find_package( Threads )

include( CheckFunctionExists )

## Check functions
check_function_exists( setsockopt SETSOCKOPT_EXISTS )
if(NOT WIN32)
#SET( CMAKE_REQUIRED_FLAGS "-fno-builtin" )
SET( CMAKE_REQUIRED_LIBRARIES "resolv" ) # FIXME
        check_function_exists( res_querydomain QUERYDOMAIN_EXISTS )
        check_function_exists( dn_skipname SKIPNAME_EXISTS )
        check_function_exists( res_query QUERY_EXISTS )
endif(NOT WIN32)

if( GLOOX_ENABLE_GETADDRINFO )
        check_function_exists( getaddrinfo GETADDRINFO_EXISTS )
        if( GETADDRINFO_EXISTS )
                set( HAVE_GETADDRINFO true )
        endif( GETADDRINFO_EXISTS )
endif( GLOOX_ENABLE_GETADDRINFO )

if( SETSOCKOPT_EXISTS )
        set( HAVE_SETSOCKOPT true )
endif( SETSOCKOPT_EXISTS )


## Zlib
if( ZLIB_FOUND )
        list( APPEND LIBS ${ZLIB_LIBRARIES} )
        list( APPEND INCLUDE_DIRS ${ZLIB_INCLUDE_DIR} )
        set( HAVE_ZLIB true )
endif( ZLIB_FOUND )

## SSL/TLS
if( GNUTLS_FOUND )
        list( APPEND LIBS ${GNUTLS_LIBRARIES} )
        list( APPEND INCLUDE_DIRS ${GNUTLS_INCLUDE_DIRS} )
        set( HAVE_GNUTLS true )
else( GNUTLS_FOUND )
        if( OPENSSL_FOUND )
                list( APPEND LIBS ${OPENSSL_LIBRARIES} )
                list( APPEND INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR} )
                set( HAVE_OPENSSL true )
        endif( OPENSSL_FOUND )
endif( GNUTLS_FOUND )

## IDN
if( IDN_FOUND )
        add_definitions( ${IDN_DEFINITIONS} )
        list( APPEND LIBS ${IDN_LIBRARIES} )
        list( APPEND INCLUDE_DIRS ${IDN_INCLUDE_DIR} )
        set( HAVE_LIBIDN true )
endif( IDN_FOUND )

## Windows/Pthreads
if( WIN32 )
        list( APPEND LIBS "-lwsock32 -lgdi32 -ldnsapi" )
        if( GLOOX_WITH_SCHANNEL )
                list( LIBS APPEND "-lcrypt32 -lsecur32" )
        endif( GLOOX_WITH_SCHANNEL )
        set( HAVE_WINTLS true )
        set( HAVE_WINDNS_H true )
else( WIN32 )
        if( CMAKE_USE_PTHREADS_INIT )
                set( HAVE_PTHREAD true )
        else( CMAKE_USE_PTHREADS_INIT )
                message( FATAL_ERROR "we do need pthread support!" )
        endif( CMAKE_USE_PTHREADS_INIT )
endif( WIN32 )

if( UNIX )
configure_file( ${CMAKE_SOURCE_DIR}/libs/gloox/config.h.cmake ${CMAKE_BINARY_DIR}/libs/gloox/config.h.unix )
endif( UNIX )

add_subdirectory ( libs/gloox/ )
