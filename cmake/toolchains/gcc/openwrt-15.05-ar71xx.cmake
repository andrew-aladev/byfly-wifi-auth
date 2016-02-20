# This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

set ( CMAKE_SYSTEM_NAME      "Linux" )
set ( CMAKE_SYSTEM_PROCESSOR "ar71xx" )

set ( TOOLCHAIN_DIR    ${STAGING_DIR}/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2/bin )
set ( CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/mips-openwrt-linux-uclibc-gcc )
set ( CMAKE_STRIP      ${TOOLCHAIN_DIR}/mips-openwrt-linux-uclibc-strip )

set ( ROOT ${STAGING_DIR}/target-mips_34kc_uClibc-0.9.33.2 )
set ( _INCLUDE_DIRECTORIES ${ROOT}/usr/include CACHE STRING "include_directories" )
set ( _LINK_DIRECTORIES    ${ROOT}/usr/lib     CACHE STRING "link_directories" )

set ( CMAKE_C_FLAGS "-march=mips32r2 -mtune=24kc" )
