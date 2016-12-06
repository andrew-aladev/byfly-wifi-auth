// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#include "common.h"
#include "resolve_conf.h"

#include <stdlib.h>
#include <string.h>


uint8_t bwa_resolve_conf_get_nameserver ( const char * path, char ** result )
{
    FILE * resolve_conf = fopen ( path, "r" );
    if ( resolve_conf == NULL ) {
        BWA_FPRINT_ERROR ( "fopen failed %s", path );
        return 1;
    }

    char * line = NULL, * line_end;
    size_t buffer_length = 0;
    ssize_t line_length;
    char * nameserver = NULL, * iterator;
    size_t nameserver_length;
    while ( ( line_length = getline ( &line, &buffer_length, resolve_conf ) ) != -1 ) {
        line_end = line + line_length;
        iterator = line;
        if ( iterator != NULL && strstr ( iterator, "nameserver" ) == line ) {
            iterator += strlen ( "nameserver" );
            while ( strchr ( " \t", * iterator ) != NULL && iterator < line_end ) {
                iterator ++;
            }
            nameserver_length = 0;
            while ( strchr ( "0123456789.", * ( iterator + nameserver_length ) ) != NULL && iterator + nameserver_length < line_end ) {
                nameserver_length ++;
            }
            if ( nameserver_length > 0 && iterator + nameserver_length < line_end ) {
                iterator[nameserver_length] = '\0';
                nameserver = strdup ( iterator );
                if ( nameserver == NULL ) {
                    BWA_PRINT_ERROR ( "strdup failed" );
                    free ( line );
                    fclose ( resolve_conf );
                    return 2;
                }
                break;
            }
        }
    }
    free ( line );
    fclose ( resolve_conf );

    if ( nameserver == NULL ) {
        BWA_FPRINT_ERROR ( "can't find nameserver in %s", path );
        return 3;
    }

    * result = nameserver;
    return 0;
}
