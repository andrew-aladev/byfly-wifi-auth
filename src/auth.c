// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#include "common.h"
#include "auth.h"

#include <stdlib.h>
#include <string.h>

#define _BWA_AUTH_STRING         "user=&pw=&login=Log+in"
#define _BWA_AUTH_STRING_PATTERN "user=%s&pw=%s&login=Log+in"
#define _BWA_AUTH_HAVE_FORM      "loginform"
#define _BWA_AUTH_HAVE_LOGIN     "Login"
#define _BWA_AUTH_HAVE_PASSWORD  "Password"
#define _BWA_AUTH_IS_ACTIVE      "Service is active"
#define _BWA_AUTH_IS_ONGOING     "Service is connecting"


void bwa_auth_new ( bwa_auth_data * data, bwa_https_context * context, const char * url )
{
    data->context = context;
    data->url     = url;
}

uint8_t bwa_auth_is_logged_in ( bwa_auth_data * data, bool * result )
{
    char * text;
    if ( bwa_https_query ( data->context, data->url, NULL, &text ) != 0 ) {
        return 1;
    }
    
    if (
        strstr ( text, _BWA_AUTH_IS_ACTIVE  ) != NULL ||
        strstr ( text, _BWA_AUTH_IS_ONGOING ) != NULL
    ) {
        * result = true;
    } else if (
        strstr ( text, _BWA_AUTH_HAVE_FORM     ) != NULL &&
        strstr ( text, _BWA_AUTH_HAVE_LOGIN    ) != NULL &&
        strstr ( text, _BWA_AUTH_HAVE_PASSWORD ) != NULL
    ) {
        * result = false;
    } else {
        BWA_PRINT_ERROR ( "page content is invalid" );
        free ( text );
        return 2;
    }
    
    free ( text );
    return 0;
}

uint8_t bwa_auth_do_login ( bwa_auth_data * data, const bwa_auth_credentials * credentials, bool * result )
{
    size_t post_data_length = strlen ( _BWA_AUTH_STRING ) + strlen ( credentials->login ) + strlen ( credentials->password ) + 1;
    char * post_data = malloc ( post_data_length );
    if ( post_data == NULL ) {
        BWA_PRINT_ERROR ( "malloc failed" );
        return 1;
    }
    
    if ( post_data_length - 1 > INT_MAX || snprintf ( post_data, post_data_length, _BWA_AUTH_STRING_PATTERN, credentials->login, credentials->password ) != ( int ) ( post_data_length - 1 ) ) {
        BWA_PRINT_ERROR ( "snprintf failed" );
        free ( post_data );
        return 2;
    }
    
    char * text;
    if ( bwa_https_query ( data->context, data->url, post_data, &text ) != 0 ) {
        free ( post_data );
        return 3;
    }
    free ( post_data );
    
    bool temp_result = (
        strstr ( text, _BWA_AUTH_IS_ACTIVE  ) != NULL ||
        strstr ( text, _BWA_AUTH_IS_ONGOING ) != NULL
    );
    free ( text );
    if ( temp_result ) {
         * result = true;
        return 0;
    }
    
    // The remote "system" can response AUTH_IS_ONGOING, AUTH_IS_ACTIVE or just nothing.
    // Another check is required, but anyway there is no guarantee that it will return true after login.
    // Upstream link can become up at any time after auth.
    // Any reliable setup should be based on vpn with low ping timeouts (like 'keepalive' option for openvpn).
    return bwa_auth_is_logged_in ( data, result );
}
