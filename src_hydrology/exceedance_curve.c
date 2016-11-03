/* --------------------------------------------------- 	*/
/* src_everglades/exceedance_curve.c		       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_parameter_file.h"
#include "list.h"
#include "environ.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"

/* Constants */
/* --------- */
#define REAL_TIME_SELECT_LIST "measurement_value,measurement_date,measurement_time,station"
#define AGGREGATION_SELECT_LIST "measurement_date,measurement_time,measurement_value,station"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *where_clause;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *aggregate_level_string;
	enum aggregate_level aggregate_level;
	char *aggregate_statistic_string;
	enum aggregate_statistic aggregate_statistic;
	char sys_string[ 1024 ];
	char aggregate_process[ 256 ];
	char *select_list;
	char *units;
	char *units_converted;
	char units_converted_process[ 128 ];
	char *database_string = {0};
	int station_piece;
	int results;

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s application \"where_clause\" aggregate_level aggregate_statistic units units_converted\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	where_clause = argv[ 2 ];
	aggregate_level_string = argv[ 3 ];
	aggregate_statistic_string = argv[ 4 ];
	units = argv[ 5 ];
	units_converted = argv[ 6 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	aggregate_level =
		aggregate_level_get_aggregate_level( aggregate_level_string );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			aggregate_statistic_string,
			aggregate_level );

	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( aggregate_level == real_time )
	{
		strcpy( aggregate_process, "cat" );
		select_list = REAL_TIME_SELECT_LIST;
		station_piece = 3;
	}
	else
	{
		sprintf( aggregate_process,
		"real_time2aggregate_value.e %s 0 1 2 '%c' '%s' n ''  |"
		"piece_shift_left.e '%c'			      |"
		"piece_shift_left.e '%c'			      |"
		"cat						       ",
		aggregate_statistic_get_string( aggregate_statistic ),
		FOLDER_DATA_DELIMITER,
		aggregate_level_get_string( aggregate_level ),
		FOLDER_DATA_DELIMITER,
		FOLDER_DATA_DELIMITER );

		select_list = AGGREGATION_SELECT_LIST;
		station_piece = 1;
	}

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s %s 0 '^' %d",
			 application_name,
			 units,
			 units_converted,
			 station_piece );
		units = units_converted;
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	sprintf(sys_string,
		"get_folder_data	application=%s			  "
		"			folder=measurement		  "
		"			select=\"%s\"			  "
		"			where=\"%s\"			  "
		"			quick=yes			 |"
		"%s							 |"
		"piece_exceedance.e '%c'				 |"
		"%s							 |"
		"cat							  ",
		application_name,
		select_list,
		where_clause,
		aggregate_process,
		FOLDER_DATA_DELIMITER,
		units_converted_process );

	results = system( sys_string );

	exit( 0 );
} /* main() */

