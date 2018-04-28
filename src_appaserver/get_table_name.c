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

	if ( ! ( application =
			environ_get_environment(
				APPASERVER_DATABASE_ENVIRONMENT_VARIABLE ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot get environment of %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE );
		exit( 1 );
	}

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
