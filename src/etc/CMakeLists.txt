# This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

install (
    FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/byfly-wifi-auth.sh"
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install (
    FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/ca-bundle.crt"
    "${CMAKE_CURRENT_SOURCE_DIR}/passwd"
    DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
)
