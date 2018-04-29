/* get_table_name.c */
/* ---------------- */

#include <stdio.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"

int main( int argc, char **argv )
{
	char *application, *folder;

	application = environ_get_application_name( argv[ 0 ] );

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s ignored folder\n",
			argv[ 0 ] );
		exit( 1 );
	}

	folder = argv[ 2 ];

	printf( "%s\n", get_table_name( application, folder ) );
	return 0;
} /* main() */
