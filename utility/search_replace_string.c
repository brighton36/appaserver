/* utility/search_replace_string.c */
/* ------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 2048 ];
	char *search_string;
	char *replace_string;

	if ( argc != 3 )
	{
		fprintf(stderr,
		"Usage: %s: search replace\n",
			argv[ 0 ] );
		exit( 1 );
	}

	search_string = argv[ 1 ];
	replace_string = argv[ 2 ];

	while( get_line( buffer, stdin ) )
	{
		printf( "%s\n",
			search_replace_string(
				buffer,
				search_string,
				replace_string ) );
	}
	return 0;
} /* main() */
