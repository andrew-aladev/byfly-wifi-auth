// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#include "common.h"
#include "auth_conf.h"

#include <stdlib.h>
#include <string.h>

#define _BWA_AUTH_CONF_SPACE " \t\r\n"


static
uint8_t _bwa_auth_conf_read_value ( FILE * auth_conf, char ** result )
{
    char * line = NULL;
    size_t buffer_length = 0;
    ssize_t line_length;
    if ( ( line_length = getline ( &line, &buffer_length, auth_conf ) ) == -1 ) {
        BWA_PRINT_ERROR ( "getline failed" );
        free ( line );
        return 1;
    }
    char * line_end = line + line_length;

    char * iterator = line;
    while ( strchr ( _BWA_AUTH_CONF_SPACE, * iterator ) != NULL && iterator < line_end ) {
        iterator ++;
    }
    char * value = iterator;
    while ( strchr ( _BWA_AUTH_CONF_SPACE, * iterator ) == NULL && iterator < line_end ) {
        iterator ++;
    }
    size_t value_length = iterator - value;
    if ( value_length == 0 ) {
        BWA_PRINT_ERROR ( "required value is empty" );
        free ( line );
        return 2;
    }

    value = strndup ( value, value_length );
    if ( value == NULL ) {
        BWA_PRINT_ERROR ( "strndup failed" );
        free ( line );
        return 3;
    }

    free ( line );
    * result = value;
    return 0;
}

uint8_t bwa_auth_conf_read ( bwa_auth_credentials * credentials, const char * path )
{
    FILE * auth_conf = fopen ( path, "r" );
    if ( auth_conf == NULL ) {
        BWA_FPRINT_ERROR ( "fopen failed for %s", path );
        return 1;
    }

    credentials->login    = NULL;
    credentials->password = NULL;

    if (
        _bwa_auth_conf_read_value ( auth_conf, &credentials->login    ) != 0 ||
        _bwa_auth_conf_read_value ( auth_conf, &credentials->password ) != 0
    ) {
        BWA_PRINT_ERROR ( "auth_conf_read_value failed" );
        bwa_auth_conf_free ( credentials );
        fclose ( auth_conf );
        return 2;
    }

    fclose ( auth_conf );
    return 0;
}

void bwa_auth_conf_free ( bwa_auth_credentials * credentials )
{
    free ( credentials->login );
    free ( credentials->password );
}
