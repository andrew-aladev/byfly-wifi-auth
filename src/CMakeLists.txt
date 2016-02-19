# This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

set (
    SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/auth.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/auth_conf.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/dns.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/https.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/resolve_conf.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
)

add_executable ( ${BWA_TARGET} ${SOURCES} )
target_link_libraries ( ${BWA_TARGET} curl )
add_custom_command (
    TARGET ${BWA_TARGET}
    POST_BUILD COMMAND ${CMAKE_STRIP} ${BWA_TARGET}
)
install ( TARGETS ${BWA_TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR} )

add_subdirectory ( "etc" )
