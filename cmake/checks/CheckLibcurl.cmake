# This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

function ( check_libcurl )
    if ( DEFINED LIBCURL_IS_USABLE )
        return ()
    endif ()
    
    if ( NOT LIBCURL_FOUND )
        set ( LIBCURL_IS_USABLE false CACHE STRING "Status of libcurl usability" )
       	message ( STATUS "Check for libcurl usability - no" )
       	return ()
    endif ()
    
    include ( CheckPipe )
    include ( CheckVerbose )
    include ( CheckC99 )
    check_pipe ()
    check_verbose ()
    check_c99 ()
    find_package ( libcurl )
    
    set ( BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/libcurl" )
    set ( SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/checks/libcurl" )
    set ( NAME "check_libcurl" )
    
    try_compile (
        CHECK_LIBCURL_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
        CMAKE_FLAGS
            "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${PIPE_CFLAGS} ${VERBOSE_CFLAGS} ${WERROR_CFLAGS} ${C99_CFLAGS}"
            "-D_INCLUDE_DIRECTORIES=${_INCLUDE_DIRECTORIES}"
            "-D_LINK_DIRECTORIES=${_LINK_DIRECTORIES}"
        OUTPUT_VARIABLE CHECK_LIBCURL_COMPILE_RESULT
    )
    if ( ${CMAKE_CONFIG_VERBOSE_MAKEFILE} )
        message ( STATUS ${CHECK_LIBCURL_COMPILE_RESULT} )
    endif ()
    FILE ( REMOVE_RECURSE ${BINARY_DIR} )
    
    if ( ${CHECK_LIBCURL_RESULT} )
        set ( LIBCURL_IS_USABLE true CACHE STRING "Status of libcurl usability" )
        message ( STATUS "Check for libcurl usability - yes" )
        return ()
    endif ()
    
    set ( LIBCURL_IS_USABLE false CACHE STRING "Status of libcurl usability" )
    message ( STATUS "Check for libcurl usability - no" )
endfunction ()
