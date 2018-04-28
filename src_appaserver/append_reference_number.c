/* src_appaserver/append_reference_number.c				*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "list.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char delimiter;
	LIST *input_list;
	char buffer[ 128 ];
	int reference_number;
	char *input_string;

	if ( ! ( application_name =
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

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf( stderr,
		"Usage: %s ignored delimiter\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 2 ];

	strcpy( buffer, "cat -" );
	input_list = pipe2list( buffer );

	if ( !list_rewind( input_list ) ) exit( 0 );

	sprintf(	buffer,
			"reference_number.sh %s %d",
			application_name,
			list_length( input_list ) );


	reference_number = atoi( pipe2string( buffer ) );

	do {
		input_string = list_get_pointer( input_list );
		printf( "%s%c%d\n",
			input_string,
			delimiter,
			reference_number++ );
	} while( list_next( input_list ) );

	return 0;
} /* main() */

