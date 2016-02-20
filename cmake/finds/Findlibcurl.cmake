if ( DEFINED LIBCURL )
    return ()
endif ()

find_library ( LIBCURL "curl" )

if ( LIBCURL )
    set ( LIBCURL_FOUND true CACHE STRING "Status of libcurl" )
    message ( STATUS "Look for \"libcurl\" - yes" )
else ()
    set ( LIBCURL_FOUND false CACHE STRING "Status of libcurl" )
    message ( STATUS "Look for \"libcurl\" - no" )
endif ()

