// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#include "common.h"
#include "https.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>


static const size_t curl_user_agents_length = 3;
static const char * curl_user_agents[] = {
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/48.0.2564.109 Safari/537.36",
    "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko",
    "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:44.0) Gecko/20100101 Firefox/44.0"
};

static const size_t curl_headers_length = 4;
static const char * curl_headers[] = {
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
    "Accept-Language: en-US,en;q=0.8,be;q=0.6,fr;q=0.4,ru;q=0.2",
    "Cache-control: no-cache",
    "Pragma: no-cache"
};

uint8_t bwa_https_new ( bwa_https_context * context, const char * host, const char * address, const char * root_url, const char * ca_bundle_path, size_t timeout )
{
    context->curl           = NULL;
    context->ca_bundle_path = ca_bundle_path;
    context->hosts_list     = NULL;
    context->headers_list   = NULL;

    curl_global_init ( CURL_GLOBAL_DEFAULT );
    context->curl = curl_easy_init ();
    if ( context->curl == NULL ) {
        BWA_PRINT_ERROR ( "curl_easy_init failed" );
        bwa_https_free ( context );
        return 1;
    }

    size_t host_list_entry_length = strlen ( host ) + 5 + strlen ( address ) + 1; // "host:443:ip"
    char * host_list_entry        = malloc ( host_list_entry_length );
    if ( host_list_entry == NULL ) {
        BWA_PRINT_ERROR ( "malloc failed" );
        bwa_https_free ( context );
        return 2;
    }
    strcpy ( host_list_entry, host );
    strcpy ( host_list_entry + strlen ( host ), ":443:" );
    strcpy ( host_list_entry + strlen ( host ) + 5, address );
    host_list_entry[host_list_entry_length - 1] = '\0';

    context->hosts_list = curl_slist_append ( NULL, host_list_entry );
    // curl_slist_append () copies the string
    free ( host_list_entry );
    if ( context->hosts_list == NULL ) {
        BWA_PRINT_ERROR ( "curl_slist_append failed" );
        bwa_https_free ( context );
        return 3;
    }

    size_t index;
    for ( index = 0; index < curl_headers_length; index ++ ) {
        if ( ( context->headers_list = curl_slist_append ( context->headers_list, curl_headers[index] ) ) == NULL ) {
            BWA_PRINT_ERROR ( "curl_slist_append failed" );
            bwa_https_free ( context );
            return 4;
        }
    }

    srand ( time ( NULL ) );
    const char * user_agent = curl_user_agents[ rand () % curl_user_agents_length ];
    if (
        curl_easy_setopt ( context->curl, CURLOPT_RESOLVE, context->hosts_list     ) != CURLE_OK ||
        curl_easy_setopt ( context->curl, CURLOPT_CAINFO,  context->ca_bundle_path ) != CURLE_OK ||

        curl_easy_setopt ( context->curl, CURLOPT_HTTPHEADER,  context->headers_list ) != CURLE_OK ||
        curl_easy_setopt ( context->curl, CURLOPT_USERAGENT,   user_agent )            != CURLE_OK ||
        curl_easy_setopt ( context->curl, CURLOPT_REFERER,     root_url )              != CURLE_OK ||
        curl_easy_setopt ( context->curl, CURLOPT_AUTOREFERER, 1 )                     != CURLE_OK ||

        curl_easy_setopt ( context->curl, CURLOPT_TIMEOUT, timeout ) != CURLE_OK
    ) {
        BWA_PRINT_ERROR ( "curl_easy_setopt failed" );
        bwa_https_free ( context );
        return 5;
    }

    return 0;
}

typedef struct _bwa_https_buffer_t {
    char * data;
    size_t length;
} _bwa_https_buffer;

static
size_t bwa_https_body_callback ( char * data, size_t size, size_t nmemb, void * user_data )
{
    size *= nmemb;
    _bwa_https_buffer * buffer = user_data;
    if ( buffer->data == NULL ) {
        buffer->data = malloc ( size );
        if ( buffer->data == NULL ) {
            BWA_PRINT_ERROR ( "malloc failed" );
            return 0;
        }
    } else {
        buffer->data = realloc ( buffer->data, buffer->length + size );
       	if ( buffer->data == NULL ) {
            BWA_PRINT_ERROR ( "realloc failed" );
            return 0;
        }
    }

    memcpy ( buffer->data + buffer->length, data, size );
    buffer->length += size;
    return size;
}

uint8_t bwa_https_query ( bwa_https_context * context, const char * url, const char * post_data, char ** result )
{
    _bwa_https_buffer buffer;
    buffer.data   = NULL;
    buffer.length = 0;

    if (
        curl_easy_setopt ( context->curl, CURLOPT_WRITEFUNCTION, &bwa_https_body_callback ) != CURLE_OK ||
        curl_easy_setopt ( context->curl, CURLOPT_WRITEDATA,     &buffer                  ) != CURLE_OK ||
        curl_easy_setopt ( context->curl, CURLOPT_URL,           url                      ) != CURLE_OK
    ) {
        BWA_PRINT_ERROR ( "curl_easy_setopt failed" );
        return 1;
    }

    if ( post_data != NULL ) {
        if (
            curl_easy_setopt ( context->curl, CURLOPT_POST,       1         ) != CURLE_OK ||
            curl_easy_setopt ( context->curl, CURLOPT_POSTFIELDS, post_data ) != CURLE_OK
        ) {
            BWA_PRINT_ERROR ( "curl_easy_setopt failed" );
            return 2;
        }
    }

    CURLcode code = curl_easy_perform ( context->curl );
    if ( code != CURLE_OK ) {
        BWA_PRINT_ERROR ( "curl_easy_perform failed" );
        free ( buffer.data );
        return 3;
    }

    if ( result != NULL ) {
        char * new_result = strndup ( buffer.data, buffer.length );
        if ( new_result == NULL ) {
            BWA_PRINT_ERROR ( "strndup failed" );
            free ( buffer.data );
            return 4;
        }
        * result = new_result;
    }

    free ( buffer.data );
    return 0;
}

void bwa_https_free ( bwa_https_context * context )
{
    if ( context->headers_list != NULL ) {
        curl_slist_free_all ( context->headers_list );
    }
    if ( context->hosts_list != NULL ) {
        curl_slist_free_all ( context->hosts_list );
    }
    if ( context->curl != NULL ) {
        curl_easy_cleanup ( context->curl );
    }
    curl_global_cleanup ();
}
