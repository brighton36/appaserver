/* ----------------------------------------------------- 	*/
/* src_hydrology/estimation_linear_interpolation_spike.c	*/
/* ----------------------------------------------------- 	*/
/* 						       		*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "query.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "dictionary.h"
#include "measurement_update_parameter.h"
#include "appaserver_parameter_file.h"
#include "measurement_backup.h"
#include "environ.h"

/* Constants */
/* --------- */
#define MAXIMUM_OUT_OF_RANGE		6
#define ESTIMATION_METHOD		"spike_correction"
#define MEASUREMENT_KEY_LIST		 "station,datatype,measurement_date,measurement_time"


/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *station;
	char *datatype;
	char *login_name;
	char *begin_date;
	char *end_date;
	int really_yn;
	int counter = 0;
	char sys_string[ 4096 ];
	char comma_delimited_record[ 1024 ];
	char *minimum_spike;  
	MEASUREMENT_UPDATE_PARAMETER *measurement_update_parameter;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FILE *update_pipe, *input_pipe;
	char date_string[ 128 ], time_string[ 128 ];
	char value_string[ 128 ], spike_value_string[ 128 ];
	MEASUREMENT_BACKUP *measurement_backup;
	char *table_name;
	DICTIONARY *parameter_dictionary;
	char *parameter_dictionary_string;
	char *notes;
	char *database_string = {0};

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 11 )
	{
		fprintf(stderr,
"Usage: %s login_name ignored station datatype begin_date end_date minimum_spike parameter_dictionary notes really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	/* application_name = argv[ 2 ]; */
	station = argv[ 3 ];
	datatype = argv[ 4 ];
	begin_date = argv[ 5 ];
	end_date = argv[ 6 ];
	minimum_spike = argv[ 7 ];
	parameter_dictionary_string = argv[ 8 ];
	notes = argv[ 9 ];
	really_yn = *argv[ 10 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					appaserver_parameter_file->
						database_management_system,
					(PROCESS_GENERIC_OUTPUT *)0,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	table_name = get_table_name( application_name, "measurement" );

	sprintf(sys_string,
		"update_statement.e %s %s '|' | sql",
		table_name,
		MEASUREMENT_KEY_LIST );

	update_pipe = popen( sys_string, "w" );

	sprintf( sys_string, 
		 "interpolation_spike_calculate			"
		 "	login_name=%s				"
		 "	application=%s				"
		 "	station=%s				"
		 "	datatype=%s				"
		 "	begin_date=%s				"
		 "	end_date=%s				"
		 "	minimum_spike=%s			"
		 "	maximum_out_of_range=%d			",
		 login_name,
		 application_name,
		 station,
		 datatype,
		 begin_date,
		 end_date,
		 minimum_spike,
		 MAXIMUM_OUT_OF_RANGE );

	input_pipe = popen( sys_string, "r" );

	parameter_dictionary = 
		dictionary_index_string2dictionary(
				parameter_dictionary_string );

	parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_STARTING_LABEL );

	parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_FROM_STARTING_LABEL );

	parameter_dictionary = dictionary_remove_index(
						parameter_dictionary );

	measurement_update_parameter = measurement_update_parameter_new(
					application_name,
					station,
					datatype,
					ESTIMATION_METHOD,
					login_name,
					parameter_dictionary,
					notes );

	measurement_backup = 
		new_measurement_backup(	application_name, 
					ESTIMATION_METHOD,
					login_name,
					measurement_update_parameter->now_date,
					measurement_update_parameter->now_time );

	measurement_backup->insert_pipe =
		measurement_backup_open_insert_pipe(
					application_name );

	/* Sample input: "BD,stage,1999-01-01,1000,666,1" */
	/* ---------------------------------------------- */
	while( get_line( comma_delimited_record, input_pipe ) )
	{
		if ( character_count( ',', comma_delimited_record ) < 4 )
		{
			fprintf( stderr, "ERROR in %s: bad input = (%s)\n",
				 argv[ 0 ], comma_delimited_record );
			exit( 1 );
		}

		piece( 	date_string,
			',',
			comma_delimited_record,
			2 );

		piece( 	time_string,
			',',
			comma_delimited_record,
			3 );

		piece( 	value_string,
			',',
			comma_delimited_record,
			4 );

		if ( *value_string
		||   character_count( ',', comma_delimited_record ) == 5 )
		{
			piece( 	spike_value_string,
				',',
				comma_delimited_record,
				5 );
	
			if ( *spike_value_string )
			{
				counter++;
	
				if ( really_yn == 'y' )
				{
					fprintf( update_pipe,
					"%s,%s,%s,%s|measurement_value=%s\n",
						 station,
						 datatype,
						 date_string,
						 time_string,
						 spike_value_string );

					fprintf( update_pipe,
				"%s,%s,%s,%s|measurement_update_method=%s\n",
						 station,
						 datatype,
						 date_string,
						 time_string,
						 ESTIMATION_METHOD );
						 
					measurement_backup_insert(
					 measurement_backup->insert_pipe,
					 measurement_backup->
					 	measurement_update_date,
					 measurement_backup->
					 	measurement_update_time,
					 measurement_backup->
					 	measurement_update_method,
					 measurement_backup->login_name,
					 comma_delimited_record,
					 really_yn,
					 ',' );
				}
			}
		}
		printf( "%s\n", comma_delimited_record );
	}

	if ( really_yn == 'y' )
	{
		DICTIONARY *parameter_dictionary;
		char count_string[ 16 ];

		parameter_dictionary = dictionary_new();

		sprintf( count_string, "%d", counter );

		dictionary_set_string(	parameter_dictionary,
					"count",
					count_string );

		dictionary_set_string(	parameter_dictionary,
					"login_name",
					login_name );

		dictionary_set_string(	parameter_dictionary,
					"datatype",
					datatype );

		dictionary_set_string(	parameter_dictionary,
					"station",
					station );

		dictionary_set_string(	parameter_dictionary,
					"begin_date",
					begin_date );

		dictionary_set_string(	parameter_dictionary,
					"minimum_spike",
					minimum_spike );

		dictionary_set_string(	parameter_dictionary,
					"end_date",
					end_date );

		measurement_update_parameter_save(
				measurement_update_parameter );
	}
	printf( "%d\n", counter );
	exit( 0 );
} /* main() */

