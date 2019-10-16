/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/alias_datatype.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "query.h"
#include "date.h"
#include "basename.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "timlib.h"
#include "column.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "julian.h"
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "application.h"
#include "hydrology_library.h"
#include "hydrology.h"
#include "station.h"
#include "datatype.h"
#include "units.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *datatype_alias;
	char *station_name;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc < 2 )
	{
		fprintf( stderr,
			 "Usage: %s station [datatype_alias]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	station_name = argv[ 1 ];

	if ( argc == 2 )
	{
		char datatype_alias[ 128 ];

		while ( timlib_get_line( datatype_alias, stdin, 128 ) )
		{
			printf( "%s:%s\n",
				datatype_alias,
				datatype_alias_display(
					application_name,
					station_name,
					datatype_alias ) );
		}
	}
	else
	{
		datatype_alias = argv[ 2 ];

		printf( "%s:%s\n",
			datatype_alias,
			datatype_alias_display(
				application_name,
				station_name,
				datatype_alias ) );
	}

	return 0;

} /* main() */

