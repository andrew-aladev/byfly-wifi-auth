// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#include <curl/curl.h>


int main ()
{
    curl_global_init ( CURL_GLOBAL_DEFAULT );
    CURL * curl = curl_easy_init ();
    if ( curl == NULL ) {
        curl_easy_cleanup ( curl );
        return 1;
    }
    
    struct curl_slist * list = curl_slist_append ( NULL, "123" );
    if ( list == NULL ) {
        curl_easy_cleanup ( curl );
        curl_global_cleanup ();
        return 2;
    }
    list = curl_slist_append ( list, "456" );
    if ( list == NULL ) {
        curl_easy_cleanup ( curl );
        curl_global_cleanup ();
        return 3;
    }
    
    if (
        curl_easy_setopt ( curl, CURLOPT_TIMEOUT, 5    ) != CURLE_OK ||
        curl_easy_setopt ( curl, CURLOPT_RESOLVE, list ) != CURLE_OK
    ) {
        curl_slist_free_all ( list );
        curl_easy_cleanup ( curl );
        curl_global_cleanup ();
        return 4;
    }
    
    curl_slist_free_all ( list );
    curl_easy_cleanup ( curl );
    curl_global_cleanup ();
    return 0;
}
