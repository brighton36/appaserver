/* get_table_name.c */
/* ---------------- */
/* Tim Riley	    */
/* ---------------- */

#include <stdio.h>
#include <stdlib.h>
#include "appaserver_library.h"

int main( int argc, char **argv )
{
	char *application, *folder;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s application folder\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application = argv[ 1 ];
	folder = argv[ 2 ];

	printf( "%s\n", get_table_name( application, folder ) );
	return 0;
} /* main() */
