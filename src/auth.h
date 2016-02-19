// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#if !defined ( _BWA_AUTH_H )
#define _BWA_AUTH_H

#include "auth_conf.h"
#include "https.h"

#include <stdbool.h>


typedef struct bwa_auth_data_t {
    bwa_https_context * context;
    const char * url;
} bwa_auth_data;

void    bwa_auth_new          ( bwa_auth_data * data, bwa_https_context * context, const char * url );
uint8_t bwa_auth_is_logged_in ( bwa_auth_data * data, bool * result );
uint8_t bwa_auth_do_login     ( bwa_auth_data * data, const bwa_auth_credentials * credentials, bool * result );


#endif
