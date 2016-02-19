// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#if !defined ( _BWA_AUTH_CONF_H )
#define _BWA_AUTH_CONF_H

#include <stdint.h>


typedef struct bwa_auth_credentials_t {
    char * login;
    char * password;
} bwa_auth_credentials;

uint8_t bwa_auth_conf_read ( bwa_auth_credentials * credentials, const char * path );
void    bwa_auth_conf_free ( bwa_auth_credentials * credentials );


#endif
