// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#include "common.h"
#include "auth.h"
#include "auth_conf.h"
#include "dns.h"
#include "https.h"
#include "resolve_conf.h"

#include <stdio.h>
#include <stdlib.h>


#define _BWA_TIMEOUT        20
#define _BWA_CA_BUNDLE_PATH "/etc/byfly-wifi/ca-bundle.crt"
#define _BWA_AUTH_CONF      "/etc/byfly-wifi/passwd"

#define _BWA_RESOLVE_CONF "/tmp/resolv.conf.auto"
#define _BWA_ROOT_URL     "https://homewifi.beltelecom.by"
#define _BWA_LOGIN_URL    "https://homewifi.beltelecom.by/login_en"
#define _BWA_HOST         "homewifi.beltelecom.by"


int main () {
    char * nameserver;
    if ( bwa_resolve_conf_get_nameserver ( _BWA_RESOLVE_CONF, &nameserver ) != 0 ) {
        BWA_PRINT_ERROR ( "resolve_conf_get_nameserver failed" );
        return 1;
    }

    char * address;
    if ( bwa_dns_query ( _BWA_HOST, nameserver, &address ) != 0 ) {
        BWA_PRINT_ERROR ( "dns_query failed" );
        free ( nameserver );
        return 2;
    }
    free ( nameserver );

    bwa_https_context context;
    if ( bwa_https_new ( &context, _BWA_HOST, address, _BWA_ROOT_URL, _BWA_CA_BUNDLE_PATH, _BWA_TIMEOUT ) != 0 ) {
        BWA_PRINT_ERROR ( "https_new failed" );
        free ( address );
        return 3;
    }
    free ( address );

    bwa_auth_credentials credentials;
    if ( bwa_auth_conf_read ( &credentials, _BWA_AUTH_CONF ) != 0 ) {
        BWA_PRINT_ERROR ( "auth_conf_read failed" );
        bwa_https_free ( &context );
        return 4;
    }

    bwa_auth_data data;
    bwa_auth_new ( &data, &context, _BWA_LOGIN_URL );

    bool is_logged_in;
    if ( bwa_auth_is_logged_in ( &data, &is_logged_in ) != 0 ) {
        BWA_PRINT_ERROR ( "auth_is_logged_in failed" );
        bwa_auth_conf_free ( &credentials );
        bwa_https_free ( &context );
        return 5;
    }

    if ( !is_logged_in ) {
        if ( bwa_auth_do_login ( &data, &credentials, &is_logged_in ) != 0 ) {
            BWA_PRINT_ERROR ( "auth_do_login failed" );
            bwa_auth_conf_free ( &credentials );
            bwa_https_free ( &context );
            return 6;
        }
        if ( !is_logged_in ) {
            BWA_PRINT_ERROR ( "login failed" );
            bwa_auth_conf_free ( &credentials );
            bwa_https_free ( &context );
            return 7;
        }
        BWA_PRINT ( "successfully logged in" );
    } else {
        BWA_PRINT ( "already logged in" );
    }

    bwa_auth_conf_free ( &credentials );
    bwa_https_free ( &context );
    return 0;
}
