/* utility/trim_length.c				*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	int length;
	char buffer[ 65536 ];

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s length\n", argv[ 0 ] );
		exit( 1 );
	}

	length = atoi( argv[ 1 ] );

	while( get_line( buffer, stdin ) )
	{
		printf( "%s\n", trim_length( buffer, length ) );
	}
	return 0;

} /* main() */

