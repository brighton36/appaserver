/* src_modeland/get_aggregation_sum_yn.c */
/* -------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *datatype;
	char *table_name;
	char sys_string[ 1024 ];
	char *results;

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

	table_name = get_table_name( application_name, "datatype" );

	sprintf( sys_string,
		 "echo \"	select aggregation_sum_yn		 "
		 "		from %s					 "
		 "		where datatype = '%s';\"		|"
		 "sql.e							 ",
		 table_name,
		 datatype );

	results = pipe2string( sys_string );

	if ( results && *results )
		printf( "%s\n", results );
	else
		printf( "n\n" );

	return 0;

} /* main() */

