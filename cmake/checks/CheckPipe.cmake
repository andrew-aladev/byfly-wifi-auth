# This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

function ( check_pipe )
    if ( DEFINED HAVE_PIPE )
        return ()
    endif ()
    
    include ( CheckVerbose )
    check_verbose ()
    
    set ( BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/basic" )
    set ( SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/checks/basic" )
    set ( NAME "check_basic" )
    
    try_compile (
        CHECK_PIPE_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
        CMAKE_FLAGS
            "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${VERBOSE_CFLAGS} ${WERROR_CFLAGS} -pipe"
            "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_CONFIG_VERBOSE_MAKEFILE}"
        OUTPUT_VARIABLE CHECK_PIPE_COMPILE_RESULT
    )
    if ( ${CMAKE_CONFIG_VERBOSE_MAKEFILE} )
        message ( STATUS ${CHECK_PIPE_COMPILE_RESULT} )
    endif ()
    FILE ( REMOVE_RECURSE ${BINARY_DIR} )
    
    if ( ${CHECK_PIPE_RESULT} )
        set ( HAVE_PIPE true CACHE STRING "Status of pipe support" )
        set ( PIPE_CFLAGS "-pipe" CACHE STRING "pipe cflags" )
        message ( STATUS "Check for C compiler -pipe support - yes" )
        return ()
    endif ()
    
    set ( HAVE_PIPE false CACHE STRING "Status of pipe support" )
    set ( PIPE_CFLAGS "" CACHE STRING "pipe cflags" )
    message ( STATUS "Check for C compiler -pipe support - no" )
endfunction ()
