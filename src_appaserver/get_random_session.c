/* ---------------------------------------------------	*/
/* get_random_session.c					*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include "session.h"
#include "timlib.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
int get_device_seed( void );

int main( int argc, char **argv )
{
	char *application_name;
	char session_number[ SESSION_MAX_SESSION_SIZE + 1 ];
	char *ptr;
	int i, n;
	int seed;

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
			 "Usage: %s ignored seed\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	seed = atoi( argv[ 2 ] );
	seed += time( (time_t *)0 );

	srand48( seed );

	ptr = session_number;

	/* Start with 1 */
	/* ------------ */
	n = timlib_random( 9 ) - 1 + '1';
	*ptr++ = n;

	for( i = 0; i < SESSION_MAX_SESSION_SIZE - 1; i++ )
	{
		n = timlib_random( 10 ) - 1 + '0';
		*ptr++ = n;
	}
	*ptr = '\0';

	printf( "%s\n", session_number );

	return 0;
} /* main() */

int get_device_seed()
{
	int seed = 0;
	FILE *file;
	char buffer[ 128 ];
	char *ptr = buffer;
	int number_read;

	file = fopen( "/dev/urandom", "r" );
	if ( !file ) return 0;

	number_read = fread( buffer, 1, 1, file );
	fclose( file );

	if ( !number_read ) return 0;

	while( number_read-- ) seed += *ptr++;

	return seed;
} /* get_device_seed() */

