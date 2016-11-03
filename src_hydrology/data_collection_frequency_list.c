/* --------------------------------------------------- 	*/
/* src_hydrology/data_collection_frequency_list.c	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hydrology_library.h"
#include "appaserver_library.h"
#include "environ.h"
#include "timlib.h"
#include "piece.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *station;
	char *datatype;
	char *aggregate_string;
	char *begin_measurement_date_string;
	char *end_measurement_date_string;
	char *database_string = {0};
	char sys_string[ 1024 ];

/*
	output_starting_argv_stderr( argc, argv );
*/

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s application station datatype aggregate_string begin_measurement_date end_measurement_date\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	station = argv[ 2 ];
	datatype = argv[ 3 ];
	aggregate_string = argv[ 4 ];
	begin_measurement_date_string = argv[ 5 ];
	end_measurement_date_string = argv[ 6 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	if ( strcmp( aggregate_string, "real_time" ) == 0 )
	{
		sprintf(sys_string,
			"time_ticker.e '^' %s %s %s '%s'		|"
			"cat						 ",
			aggregate_string,
			begin_measurement_date_string,
			end_measurement_date_string,
			hydrology_library_get_expected_count_list_string(
				application_name, station, datatype, '|' ) );
	}
	else
	{
		sprintf(sys_string,
			"time_ticker.e '^' %s %s %s ''			|"
			"cat						 ",
			aggregate_string,
			begin_measurement_date_string,
			end_measurement_date_string );
	}
	system( sys_string );
	exit( 0 );
} /* main() */

