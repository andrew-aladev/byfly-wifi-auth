# This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

cmake_minimum_required ( VERSION "2.8" )
project ( "check_libcurl" "C" )

if ( DEFINED _INCLUDE_DIRECTORIES )
    include_directories ( ${_INCLUDE_DIRECTORIES} )
endif ()
if ( DEFINED _LINK_DIRECTORIES )
    link_directories ( ${_LINK_DIRECTORIES} )
endif ()

set ( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../finds" )

find_package ( libcurl )

if ( NOT LIBCURL_FOUND )
    message ( SEND_ERROR "libcurl is required" )
    return ()
endif ()

add_executable (
    "main"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
)
target_link_libraries ( "main" ${LIBCURL} )
