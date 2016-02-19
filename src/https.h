// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#if !defined ( _BWA_HTTPS_H )
#define _BWA_HTTPS_H

#include <curl/curl.h>
#include <stdint.h>


typedef struct bwa_https_context_t {
    CURL * curl;
    const char * ca_bundle_path;
    struct curl_slist * hosts_list;
    struct curl_slist * headers_list;
} bwa_https_context;

uint8_t bwa_https_new   ( bwa_https_context * context, const char * host, const char * address, const char * root_url, const char * ca_bundle_path, size_t timeout );
uint8_t bwa_https_query ( bwa_https_context * context, const char * url, const char * post_data, char ** result );
void    bwa_https_free  ( bwa_https_context * context );


#endif
