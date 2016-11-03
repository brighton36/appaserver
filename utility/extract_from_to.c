/* utility/extract_from_to.c */
/* ------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "boolean.h"

int main( int argc, char **argv )
{
	char buffer[ 1024 ];
	boolean is_inside = 0;
	boolean is_inverse = 0;


	if ( argc < 3 )
	{
		fprintf( stderr, 
			 "Usage: cat filename | %s from to [inverse]\n",
			 argv[ 0 ]);
		exit( 1 );
	}

	if ( argc == 4 )
	{
		if ( strcmp( argv[ 3 ], "inverse" ) != 0 )
		{
			fprintf( stderr, 
			 "Usage: cat filename | %s from to [inverse]\n",
			 	argv[ 0 ]);
			exit( 1 );
		}
		else
		{
			is_inverse = 1;
		}
	}

	while( get_line( buffer, stdin ) )
	{
		if ( is_inverse )
		{
			if ( !is_inside )
			{
				printf( "%s\n", buffer );
				if ( strcmp( buffer, argv[ 1 ] ) == 0 )
				{
					is_inside = 1;
				}
			}
			else
			{
				if ( strcmp( buffer, argv[ 2 ] ) == 0 )
					is_inside = 0;
			}
		}
		else
		{
			if ( !is_inside )
			{
				if ( strcmp( buffer, argv[ 1 ] ) == 0 )
				{
					is_inside = 1;
					printf( "%s\n", buffer );
				}
			}
			else
			{
				printf( "%s\n", buffer );
				if ( strcmp( buffer, argv[ 2 ] ) == 0 )
					exit( 0 );
			}
		}
	}
	return 0;
} /* main() */
