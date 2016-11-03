
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

void piece_commas_in_double( int piece_offset );

int main( int argc, char **argv )
{
	int piece_offset;

	if ( argc != 2 )
	{
		fprintf( stderr,
			 "Usage: %s piece_offset\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	piece_offset = atoi( argv[ 1 ] );

	piece_commas_in_double( piece_offset );
	return 0;

} /* main() */

void piece_commas_in_double( int piece_offset )
{
	char buffer[ 1024 ];
	char piece_buffer[ 128 ];
	char results[ 128 ];

	while( get_line( buffer, stdin ) )
	{
		if ( ! piece( piece_buffer, '^', buffer, piece_offset ) )
		{
			fprintf( stderr, 
				 "%s: cannot get piece: %d in (%s)\n",
				 __FILE__,
				 piece_offset,
				 buffer );

			printf( "%s\n", buffer );
			continue;
		}

		printf( "%s\n",
			piece_replace(	buffer,
					'^',
		 			commas_in_double(
						results,
						atof( piece_buffer ) ),
					piece_offset ) );
	}

} /* piece_commas_in_double() */


