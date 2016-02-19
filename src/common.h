// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#if !defined ( _BWA_COMMON_H )
#define _BWA_COMMON_H

#include <stdio.h>


#if defined ( __GNUC__ )
#    define BWA_UNUSED(x) x __attribute__((__unused__))
#    define BWA_UNUSED_FUNCTION(x) __attribute__((__unused__)) x
#else
#    define BWA_UNUSED(x) x
#    define BWA_UNUSED_FUNCTION(x) x
#endif

#define _BWA_PRINT( target, text ) \
    bwa_print ( target, __FILE__, __LINE__, __func__, text );

#define _BWA_FPRINT( target, ... ) \
    bwa_fprint ( target, __FILE__, __LINE__, __func__, __VA_ARGS__ );

#define BWA_PRINT( text ) \
    _BWA_PRINT ( stdout, text );

#define BWA_FPRINT( ... ) \
    _BWA_FPRINT ( stdout, __VA_ARGS__ );

#define BWA_PRINT_ERROR( text ) \
    _BWA_PRINT ( stderr, text );

#define BWA_FPRINT_ERROR( ... ) \
    _BWA_FPRINT ( stderr, __VA_ARGS__ );

void bwa_print  ( FILE * target, const char * file, size_t line, const char * func, const char * text );
void bwa_fprint ( FILE * target, const char * file, size_t line, const char * func, const char * format, ... );


#endif
