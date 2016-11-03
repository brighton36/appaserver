/* src_hydrology/get_aggregation_sum_yn.c */
/* -------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "station_datatype.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	STATION_DATATYPE *station_datatype;
	char *application_name, *datatype;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s application datatype\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	datatype = argv[ 2 ];

	station_datatype =
		station_datatype_get_station_datatype(
			application_name,
			(char *)0 /* station */,
			datatype );

	if ( !station_datatype )
		printf( "n\n" );
	else
		printf( "%c\n", station_datatype->aggregation_sum_yn );

	return 0;
} /* main() */

