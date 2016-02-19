# This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

set ( CMAKE_SYSTEM_NAME "Linux" )
set ( CMAKE_C_COMPILER "gcc" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wno-maybe-uninitialized -fsanitize=leak -fno-omit-frame-pointer" CACHE STRING "gcc leak cflags" )
set ( HAVE_LTO false CACHE STRING "Status of LTO support" )
