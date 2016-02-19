// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#include "common.h"

#include <stdarg.h>
#include <string.h>
#include <time.h>


static
void _bwa_print_header ( FILE * target, const char * file, size_t line, const char * func )
{
    time_t time_data = time ( NULL );
    if ( time_data != ( time_t ) -1 ) {
        struct tm * t = localtime ( &time_data );
        if ( t != NULL ) {
            fprintf (
                target,
                "%02u:%02u:%02u %02u-%02u-%04u",
               	t->tm_hour, t->tm_min,     t->tm_sec,
               	t->tm_mday, t->tm_mon + 1, t->tm_year + 1900
            );
       	}
    }
    const char * file_name = strrchr ( file, '/' );
    if ( file_name == NULL ) {
       	file_name = file;
    } else {
       	file_name ++;
    }
    fprintf ( target, " %s %s:%zu - ", func, file_name, line );
}

void bwa_print  ( FILE * target, const char * file, size_t line, const char * func, const char * text )
{
    _bwa_print_header ( target, file, line, func );
    fputs ( text, target );
    fputs ( "\n", target );
}

void bwa_fprint ( FILE * target, const char * file, size_t line, const char * func, const char * format, ... )
{
    _bwa_print_header ( target, file, line, func );
    
    va_list arguments;
    va_start ( arguments, format );
    vfprintf ( target, format, arguments );
    va_end ( arguments );
    
    fputs ( "\n", target );
}
