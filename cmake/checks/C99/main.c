// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#include <stdbool.h>
#include <stdlib.h>

int main ()
{
    long long int * data_1 = malloc ( sizeof ( long long int ) * 5 );
    if ( data_1 == NULL ) {
        return 1;
    }
    bool * data_2 = calloc ( 1, sizeof ( bool ) * 10 );
    if ( data_2 == NULL ) {
        return 2;
    }
    free ( data_1 );
    free ( data_2 );
    return 0;
}
