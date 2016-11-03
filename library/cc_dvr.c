#include <stdio.h>
#include <stdlib.h>
#include "cc_chk.h"

int main( int argc, char **argv )
{
	char *get_cc_name();
        int return_val;

        if( argc != 2 )
        {
                fprintf( stderr, "Usage: cc_dvr cc_number\n" );
                exit( 1 );
        }

        if( cc_chk_digit( argv[ 1 ] ) == 1 )
	{
	        if( ( return_val = cc_identify( argv[ 1 ] ) ) == -1 )
		{
	                printf("ERROR: cc_identify failed to identify\n" );
		}
	        else
	        {
	                printf("%s\n", get_cc_name( return_val ) );
	        }
	}
        else
	{
                printf("This is NOT a valid CC number.\n");
		exit( 1 );
	}

        return 0;

} /* end main */
