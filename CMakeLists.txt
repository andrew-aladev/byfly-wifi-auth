# This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

cmake_minimum_required ( VERSION "2.8" )
project ( "byfly-wifi-auth" "C" ) # BWA

set ( BWA_VERSION "1.0.1" )

if ( NOT DEFINED BWA_TARGET )
    set ( BWA_TARGET ${PROJECT_NAME} )
endif ()

if ( NOT DEFINED CMAKE_INSTALL_BINDIR )
    set ( CMAKE_INSTALL_BINDIR "usr/bin" CACHE PATH "Output directory for binaries" )
endif ( )
if ( NOT DEFINED CMAKE_INSTALL_CONFIGDIR )
    set ( CMAKE_INSTALL_CONFIGDIR "etc/byfly-wifi" CACHE PATH "Output directory for config" )
endif ()

if ( DEFINED _INCLUDE_DIRECTORIES )
    include_directories ( ${_INCLUDE_DIRECTORIES} )
endif ()
if ( DEFINED _LINK_DIRECTORIES )
    link_directories ( ${_LINK_DIRECTORIES} )
endif ()

set ( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/checks" "${PROJECT_SOURCE_DIR}/cmake/finds" )

include ( CheckPipe )
include ( CheckVerbose )
include ( CheckC99 )
include ( CheckLibcurl )
include ( CheckLTO )
check_pipe ()
check_verbose ()
check_c99 ()
find_package ( libcurl )
check_libcurl ()
check_lto ()

if ( NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "" )
    set ( CMAKE_BUILD_TYPE DEBUG )
endif ()

set ( CMAKE_C_FLAGS       "${CMAKE_C_FLAGS} ${PIPE_CFLAGS} ${VERBOSE_CFLAGS} ${C99_CFLAGS}" )
set ( CMAKE_C_FLAGS_DEBUG "-O0 -g" )

set ( CMAKE_C_FLAGS_RELEASE             "-O2 ${LTO_CFLAGS}" )
set ( CMAKE_EXE_LINKER_FLAGS_RELEASE    "${CMAKE_EXE_LINKER_FLAGS}    ${LTO_LDLAGS}" )
set ( CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS} ${LTO_LDLAGS}" )

set ( CMAKE_C_FLAGS_RELEASE_EMBED             "-Os ${LTO_CFLAGS}" )
set ( CMAKE_EXE_LINKER_FLAGS_RELEASE_EMBED    "${CMAKE_EXE_LINKER_FLAGS}    ${LTO_LDLAGS}" )
set ( CMAKE_SHARED_LINKER_FLAGS_RELEASE_EMBED "${CMAKE_SHARED_LINKER_FLAGS} ${LTO_LDLAGS}" )

add_subdirectory ( "src" )

set ( CPACK_SET_DESTDIR true )
set ( CPACK_INSTALL_PREFIX "/" )

include ( CPackConfig.cmake )
include ( CPack )
