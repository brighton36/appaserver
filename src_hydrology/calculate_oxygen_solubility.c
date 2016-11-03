/* --------------------------------------------------- 	*/
/* src_hydrology/calculate_oxygen_solubility.c	      	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "boolean.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "hydrology_library.h"

/* Constants */
/* --------- */
#define DATATYPE1			"bottom_temperature"
#define DATATYPE2			"salinity"
#define MEASUREMENT_UPDATE_LIST		"station,measurement_date,measurement_time,datatype"

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
void insert_station_datatype(
				char *application_name,
				char *station,
				char *oxygen_solubility_datatype );

void update_validation_columns(
				char *application_name,
				char *station,
				char *where_clause );

int main( int argc, char **argv )
{
	char *application_name;
	char *where_clause;
	char *station;
	char *database_string;
	char sys_string[ 1024 ];
	char input_buffer[ 128 ];
	char date_time_string[ 32 ];
	char bottom_temperature[ 16 ];
	char salinity[ 16 ];
	FILE *input_pipe;
	FILE *output_pipe;
	int with_sql_statements = 0;
	boolean got_one = 0;

	if ( argc < 4 )
	{
		fprintf(stderr,
"Usage: %s application station \"where\" [sql]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	station = argv[ 2 ];
	where_clause = argv[ 3 ];

	if ( argc == 5 )
	{
		with_sql_statements = ( strcmp( argv[ 4 ], "sql" ) == 0 );
	}

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

	if ( with_sql_statements )
	{
		char *table_name;
		char *field;

		table_name = get_table_name( application_name, "measurement" );
		field =
	"station,datatype,measurement_date,measurement_time,measurement_value";

		sprintf(sys_string,
			"oxygen_solubility.py 3 4			    |"
		 	"tr ' ' '^'					    |"
			"piece.e '^' 0,1,2,3,6				    |"
			"insert_statement.e table=%s field=%s delimiter='^'  ",
			table_name,
			field );
	}
	else
	{
		sprintf( sys_string,
		 	"oxygen_solubility.py 3 4" );
	}

	output_pipe = popen( sys_string, "w" );

	sprintf(sys_string,
		"measurement_merge.sh %s %s %s %s \"%s\"",
		application_name,
		station,
		DATATYPE1,
		DATATYPE2,
		where_clause );

	input_pipe = popen( sys_string, "r" );;

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( !got_one )
		{
			insert_station_datatype(
				application_name,
				station,
				OXYGEN_SOLUBILITY_DATATYPE );
			got_one = 1;
		}

		piece(	date_time_string,
			'|',
			input_buffer,
			0 );

		piece(	bottom_temperature,
			'|',
			input_buffer,
			1 );

		piece(	salinity,
			'|',
			input_buffer,
			2 );

		if ( *bottom_temperature && *salinity )
		{
			fprintf(output_pipe,
				"%s %s %s %s %s\n",
				station,
				OXYGEN_SOLUBILITY_DATATYPE,
				date_time_string,
				salinity,
				bottom_temperature );
		}
	}

	pclose( input_pipe );
	pclose( output_pipe );

	if ( with_sql_statements )
	{
		update_validation_columns(
			application_name,
			station,
			where_clause );
	}

	return 0;

} /* main() */

void update_validation_columns(
			char *application_name,
			char *station,
			char *where_clause )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	FILE *update_pipe;
	char expanded_where_clause[ 1024 ];
	char *select;
	char input_buffer[ 1024 ];
	char measurement_date_string[ 128 ];
	char measurement_time_string[ 128 ];
	char piece_buffer[ 128 ];
	char *table_name;

	table_name = get_table_name( application_name, "measurement" );

	sprintf(sys_string,
		"update_statement.e %s %s '^^'",
		table_name,
		MEASUREMENT_UPDATE_LIST );

	update_pipe = popen( sys_string, "w" );

	sprintf( expanded_where_clause,
		 "%s and datatype = 'salinity'",
		 where_clause );

	select =
"measurement_date,measurement_time,last_validation_date,last_person_validating,last_validation_process,measurement_update_method";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=measurement	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 expanded_where_clause );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	measurement_date_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	measurement_time_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		/* Last Validation Date */
		/* -------------------- */
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		if ( *piece_buffer )
		{
			fprintf(update_pipe,
				"%s^%s^%s^%s^last_validation_date^%s\n",
				station,
				measurement_date_string,
				measurement_time_string,
				OXYGEN_SOLUBILITY_DATATYPE,
				piece_buffer );
		}

		/* Last Person Validating */
		/* ---------------------- */
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		if ( *piece_buffer )
		{
			fprintf(update_pipe,
				"%s^%s^%s^%s^last_person_validating^%s\n",
				station,
				measurement_date_string,
				measurement_time_string,
				OXYGEN_SOLUBILITY_DATATYPE,
				piece_buffer );
		}

		/* Last Validation Process */
		/* ----------------------- */
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		if ( *piece_buffer )
		{
			fprintf(update_pipe,
				"%s^%s^%s^%s^last_validation_process^%s\n",
				station,
				measurement_date_string,
				measurement_time_string,
				OXYGEN_SOLUBILITY_DATATYPE,
				piece_buffer );
		}

		/* Measurement Update Method */
		/* ------------------------- */
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		if ( *piece_buffer )
		{
			fprintf(update_pipe,
				"%s^%s^%s^%s^measurement_update_method^%s\n",
				station,
				measurement_date_string,
				measurement_time_string,
				OXYGEN_SOLUBILITY_DATATYPE,
				piece_buffer );
		}

	}
	pclose( input_pipe );
	pclose( update_pipe );
} /* update_validation_columns() */

void insert_station_datatype(
				char *application_name,
				char *station,
				char *oxygen_solubility_datatype )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *field_list_string;
	FILE *output_pipe;

	table_name = get_table_name( application_name, "station_datatype" );
	field_list_string = "station,datatype";

	sprintf( sys_string,
"insert_statement.e table=%s field=%s | sql.e 2>&1 | grep -vi duplicate",
		 table_name,
		 field_list_string );

	output_pipe = popen( sys_string, "w" );
	fprintf(	output_pipe,
			"%s|%s\n",
			station,
			oxygen_solubility_datatype );
	pclose( output_pipe );

} /* insert_station_datatype() */

