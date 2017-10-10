/* src_hydrology/generate_data_collection_frequency.c	*/
/* ---------------------------------------------------- */
/*
-------------------------------------------------------------
This program generates the data collection frequency table
by selecting from the measurement table.
-------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "timlib.h"
#include "boolean.h"
#include "date.h"
#include "appaserver_library.h"

/* Constants */
/* --------- */
#define MINUTES_BETWEEN_ARRAY_LENGTH	8

enum possibility {	first_time,
			not_first_time,
			gap,
			change };

/* Prototypes */
/* ---------- */
void generate_data_collection_frequency_list(
			char *application_name,
			char *station_list_string,
			char *datatype_list_string,
			boolean produce_sql_statements );

void generate_data_collection_frequency_station_datatype(
			char *application_name,
			char *station,
			char *datatype,
			boolean produce_sql_statements );

int *get_expected_minutes_between_array( void );

boolean exists_expected_minutes(
			int *expected_minutes_between_array,
			int expected_minutes );

char *get_sys_string(	char *application_name,
			char *station,
			char *datatype );

void generate_data_collection_frequency(
			char *application_name,
			char *station,
			char *datatype,
			boolean produce_sql_statements );

int main( int argc, char **argv )
{
	char *application_name;
	char *station;
	char *datatype;
	boolean produce_sql_statements;

	if ( argc < 4 )
	{
		fprintf( stderr,
			 "Usage: %s application [station datatype] or [station[,...] all] or [all all] [sql]\n",
			 argv[ 0 ] );
		fprintf( stderr,
"Note: if the forth parameter is 'sql', then sql statements are produced.\n" );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	station = argv[ 2 ];
	datatype = argv[ 3 ];

	if ( argc == 5 )
		produce_sql_statements =
			( strcmp( argv[ 4 ], "sql" ) == 0 );
	else
		produce_sql_statements = 0;

	generate_data_collection_frequency(
			application_name,
			station,
			datatype,
			produce_sql_statements );

	exit( 0 );

} /* main() */

void generate_data_collection_frequency(
			char *application_name,
			char *station,
			char *datatype,
			boolean produce_sql_statements )
{
	if ( strcmp( station, "all" ) == 0 )
	{
		FILE *input_pipe;
		char sys_string[ 1024 ];
		char input_buffer[ 256 ];
		char local_station[ 128 ];
		char local_datatype[ 128 ];
		char *select = "station,datatype";

		sprintf( sys_string,
			 "get_folder_data	application=%s		"
			 "			select=%s		"
			 "			folder=station_datatype	",
			 application_name,
			 select );
		input_pipe = popen( sys_string, "r" );
		while( get_line( input_buffer, input_pipe ) )
		{
			piece( local_station,
			       FOLDER_DATA_DELIMITER,
			       input_buffer,
			       0 );

			if ( *local_station == '1' ) continue;

			piece( local_datatype,
			       FOLDER_DATA_DELIMITER,
			       input_buffer,
			       1 );

			generate_data_collection_frequency_list(
					application_name,
					local_station,
					local_datatype,
					produce_sql_statements );
		}
		pclose( input_pipe );
	}
	else
	if ( strcmp( datatype, "all" ) == 0 )
	{
		FILE *input_pipe;
		char sys_string[ 1024 ];
		char where_clause[ 128 ];
		char local_station[ 128 ];
		char local_datatype[ 128 ];
		char input_buffer[ 256 ];
		char *select = "station,datatype";

		sprintf( where_clause,
			 "station in (%s)",
			 timlib_get_in_clause( station ) );

		sprintf( sys_string,
			 "get_folder_data	application=%s		"
			 "			select=%s		"
			 "			folder=station_datatype	"
			 "			where=\"%s\"		",
			 application_name,
			 select,
			 where_clause );
		input_pipe = popen( sys_string, "r" );
		while( get_line( input_buffer, input_pipe ) )
		{
			piece( local_station,
			       FOLDER_DATA_DELIMITER,
			       input_buffer,
			       0 );

			piece( local_datatype,
			       FOLDER_DATA_DELIMITER,
			       input_buffer,
			       1 );
			generate_data_collection_frequency_list(
					application_name,
					local_station,
					local_datatype,
					produce_sql_statements );
		}
		pclose( input_pipe );
	}
	else
	{
		generate_data_collection_frequency_list(
			application_name,
			station,
			datatype,
			produce_sql_statements );
	}

} /* generate_data_collection_frequency() */

char *get_sys_string(	char *application_name,
			char *station,
			char *datatype )
{
	static char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	char *select;
	select = "measurement_date,measurement_time";

	sprintf(where_clause,
		"station = '%s' and datatype = '%s'",
		station,
		datatype );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=measurement		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where_clause );

	return sys_string;
} /* get_sys_string() */

int *get_expected_minutes_between_array( void )
{
	static int expected_minutes_between_array[
			MINUTES_BETWEEN_ARRAY_LENGTH ];

	expected_minutes_between_array[ 0 ] = 1440;
	expected_minutes_between_array[ 1 ] = 180;
	expected_minutes_between_array[ 2 ] = 60;
	expected_minutes_between_array[ 3 ] = 30;
	expected_minutes_between_array[ 4 ] = 15;
	expected_minutes_between_array[ 5 ] = 10;
	expected_minutes_between_array[ 6 ] = 6;
	expected_minutes_between_array[ 7 ] = 5;

	return expected_minutes_between_array;
} /* get_expected_minutes_between_array() */

boolean exists_expected_minutes(
		int *expected_minutes_between_array,
		int minutes_between )
{
	int x;

	for( x = 0; x < MINUTES_BETWEEN_ARRAY_LENGTH; x++ )
	{
		if ( expected_minutes_between_array[ x ] == minutes_between )
		{
			return 1;
		}
	}
	return 0;
} /* exists_expected_minutes() */

void generate_data_collection_frequency_list(
			char *application_name,
			char *station_list_string,
			char *datatype_list_string,
			boolean produce_sql_statements )
{
	LIST *station_list;
	LIST *datatype_list;
	char *station;
	char *datatype;

	station_list = list_string2list( station_list_string, ',' );
	datatype_list = list_string2list( datatype_list_string, ',' );

	if ( !list_rewind( station_list ) ) return;

	do {
		station = list_get_pointer( station_list );

		if ( !list_rewind( datatype_list ) ) return;

		do {
			datatype = list_get_pointer( datatype_list );

			generate_data_collection_frequency_station_datatype(
				application_name,
				station,
				datatype,
				produce_sql_statements );
		} while( list_next( datatype_list ) );
	} while( list_next( station_list ) );

} /* generate_data_collection_frequency_station_datatype() */

void generate_data_collection_frequency_station_datatype(
			char *application_name,
			char *station,
			char *datatype,
			boolean produce_sql_statements )
{
	char *sys_string;
	char input_buffer[ 128 ];
	FILE *input_pipe;
	char previous_measurement_date[ 16 ];
	char previous_measurement_time[ 16 ];
	char measurement_date[ 16 ];
	char measurement_time[ 16 ];
	int minutes_between;
	int previous_minutes_between = -1;
	int *expected_minutes_between_array;
	enum possibility possibility;
	FILE *output_pipe;

	if ( produce_sql_statements )
	{
		char *field_list;
		char *table_name;
		char sys_string[ 256 ];

		field_list =
"station,datatype,begin_measurement_date,begin_measurement_time,expected_count";

		table_name =
			get_table_name(
				application_name,
				"data_collection_frequency" );
		sprintf( sys_string,
			 "insert_statement.e t=%s f=%s d=,",
			 table_name,
			 field_list );
		fprintf( stdout,
		"delete from %s where station = '%s' and datatype = '%s\n",
			table_name,
			station,
			datatype );
		fflush( stdout );
		output_pipe = popen( sys_string, "w" );
	}
	else
	{
		output_pipe = popen( "cat", "w" );
	}


	expected_minutes_between_array =
		get_expected_minutes_between_array();

	sys_string =
		get_sys_string(	application_name,
				station,
				datatype );

	input_pipe = popen( sys_string, "r" );

	possibility = first_time;
	while( get_line( input_buffer, input_pipe ) )
	{
		if ( possibility == first_time )
		{
			piece( previous_measurement_date,
			       FOLDER_DATA_DELIMITER,
			       input_buffer,
			       0 );
			piece( previous_measurement_time,
			       FOLDER_DATA_DELIMITER,
			       input_buffer,
			       1 );
			possibility = not_first_time;
			continue;
		}

		piece(	measurement_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );
		piece(	measurement_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		minutes_between =
			date_minutes_between(
				previous_measurement_date,
				previous_measurement_time,
				measurement_date,
				measurement_time,
				0 /* don't add_one */,
				HOURS_WEST_GMT );

		if ( exists_expected_minutes(
			expected_minutes_between_array,
			minutes_between ) )
		{
			possibility = not_first_time;
		}
		else
		{
			strcpy( previous_measurement_date, measurement_date );
			strcpy( previous_measurement_time, measurement_time );
			possibility = gap;
			continue;
		}

		if ( previous_minutes_between != minutes_between )
		{
			possibility = change;
		}

		if ( possibility == gap || possibility == change )
		{
			if ( strcmp( previous_measurement_time, "null" ) == 0 )
			{
				strcpy(	previous_measurement_time,
					"0000" );
			}

			fprintf(output_pipe,
				"%s,%s,%s,%s,%d\n",
				station,
				datatype,
				previous_measurement_date,
				previous_measurement_time,
				1440 / minutes_between );

			previous_minutes_between = minutes_between;
		}
		strcpy( previous_measurement_date, measurement_date );
		strcpy( previous_measurement_time, measurement_time );

	}
	pclose( input_pipe );
	pclose( output_pipe );

} /* generate_data_collection_frequency_station_datatype() */

