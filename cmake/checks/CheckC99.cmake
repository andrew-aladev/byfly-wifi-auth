# This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

function (check_c99)
    if (DEFINED HAVE_C99)
        return ()
    endif ()
    
    include (CheckVerbose)
    check_verbose ()
    
    set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/C99")
    set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/checks/C99")
    set (NAME "check_C99")
    
    try_compile (CHECK_C99_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
        CMAKE_FLAGS
            "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${VERBOSE_CFLAGS} ${WERROR_CFLAGS} -std=gnu99"
            "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_CONFIG_VERBOSE_MAKEFILE}"
        OUTPUT_VARIABLE CHECK_C99_COMPILE_RESULT
    )
    if (${CMAKE_CONFIG_VERBOSE_MAKEFILE})
        message (STATUS ${CHECK_C99_COMPILE_RESULT})
    endif ()
    FILE (REMOVE_RECURSE ${BINARY_DIR})
    
    if (${CHECK_C99_RESULT})
        set (HAVE_C99 true CACHE STRING "Status of C99 support")
        set (C99_CFLAGS "-std=gnu99" CACHE STRING "c99 cflags")
        message (STATUS "Check for C compiler C99 support - gnu99")
        return ()
    endif ()
    
    try_compile (CHECK_C99_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
        CMAKE_FLAGS
            "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${VERBOSE_CFLAGS} ${WERROR_CFLAGS} -std=c99"
            "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_CONFIG_VERBOSE_MAKEFILE}"
        OUTPUT_VARIABLE CHECK_C99_COMPILE_RESULT
    )
    if (${CMAKE_CONFIG_VERBOSE_MAKEFILE})
        message (STATUS ${CHECK_C99_COMPILE_RESULT})
    endif ()
    FILE (REMOVE_RECURSE ${BINARY_DIR})
    
    if (${CHECK_C99_RESULT})
        set (HAVE_C99 true CACHE STRING "Status of C99 support")
        set (C99_CFLAGS "-std=c99" CACHE STRING "c99 cflags")
        message (STATUS "Check for C compiler C99 support - c99")
        return ()
    endif ()
    
    try_compile (CHECK_C99_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
        CMAKE_FLAGS
            "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${VERBOSE_CFLAGS} ${WERROR_CFLAGS}"
            "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_CONFIG_VERBOSE_MAKEFILE}"
        OUTPUT_VARIABLE CHECK_C99_COMPILE_RESULT
    )
    if (${CMAKE_CONFIG_VERBOSE_MAKEFILE})
        message (STATUS ${CHECK_C99_COMPILE_RESULT})
    endif ()
    FILE (REMOVE_RECURSE ${BINARY_DIR})
    
    if (${CHECK_C99_RESULT})
        set (HAVE_C99 true CACHE STRING "Status of C99 support")
        set (C99_CFLAGS "" CACHE STRING "c99 cflags")
        message (STATUS "Check for C compiler C99 support - vanilla")
        return ()
    endif ()
    
    set (HAVE_C99 false CACHE STRING "Status of C99 support")
    set (C99_CFLAGS "" CACHE STRING "c99 cflags")
    message (STATUS "Check for C compiler C99 support - no")
endfunction ()
