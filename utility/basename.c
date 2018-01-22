/* utility/basename.c					*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "basename.h"

int main( int argc, char **argv )
{
	char *filename;
	boolean strip_extension;

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s filename strip_extension_yn\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	filename = argv[ 1 ];
	strip_extension = ( *argv[ 2 ] == 'y' );

	printf( "%s\n",
		basename_get_base_name(
			filename,	
			strip_extension ) );

	return 0;

} /* main() */

