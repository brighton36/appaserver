/* ----------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/sensor_exposed_null.c	*/
/* ----------------------------------------------------------- 	*/
/* 						       		*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "measurement_update_parameter.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "measurement_backup.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "boolean.h"
#include "date.h"

/* Constants */
/* --------- */
#define MEASUREMENT_UPDATE_METHOD	"sensor_exposed_null"
#define MEASUREMENT_SELECT_LIST		"station,datatype,measurement_date,measurement_time,measurement_value"

#define MEASUREMENT_KEY_LIST		 "station,datatype,measurement_date,measurement_time"

#define MEASUREMENT_DATE_PIECE		2
#define MEASUREMENT_TIME_PIECE		3

/* Prototypes */
/* ---------- */
void backup_and_update(		FILE *update_pipe,
				char *primary_key_half,
				char *results_string,
				char *buffer,
				MEASUREMENT_BACKUP *measurement_backup,
				boolean execute );

void output_buffer(		char *buffer,
				char *results_string );

int main( int argc, char **argv )
{
	int counter = 0;
	char *application_name;
	char *process_name;
	char *station;
	char *datatype;
	char *where_clause;
	char buffer[ 4096 ];
	char primary_key_half[ 4096 ];
	char value_half[ 4096 ];
	MEASUREMENT_BACKUP *measurement_backup;
	char *table_name;
	FILE *input_pipe;
	FILE *update_pipe;
	char results_string[ 128 ];
	double threshold_value;
	char *above_below;
	boolean null_above;
	boolean execute;
	DICTIONARY *parameter_dictionary;
	char *parameter_dictionary_string;
	MEASUREMENT_UPDATE_PARAMETER *measurement_update_parameter;
	char *login_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *begin_date_string, *end_date_string;
	char *begin_time_string, *end_time_string;
	char measurement_date_string[ 128 ];
	char measurement_time_string[ 128 ];
	DATE *begin_date, *end_date;
	DATE *measurement_date = {0};
	char *notes;

	if ( argc != 15 )
	{
		fprintf(stderr,
"Usage: %s login_name process station datatype threshold_value above_below begin_date begin_time end_date end_time parameter_dictionary \"where_clause\" notes execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	process_name = argv[ 2 ];
	station = argv[ 3 ];
	datatype = argv[ 4 ];
	threshold_value = atof( argv[ 5 ] );
	above_below = argv[ 6 ];
	begin_date_string = argv[ 7 ];
	begin_time_string = argv[ 8 ];
	end_date_string = argv[ 9 ];
	end_time_string = argv[ 10 ];
	parameter_dictionary_string = argv[ 11 ];
	where_clause = argv[ 12 ];
	notes = argv[ 13 ];
	execute = ( *argv[ 14 ] == 'y' );

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !*above_below
	||   strcmp( above_below, "above_below" ) == 0
	||   strcmp( above_below, "select" ) == 0 )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
		"<h3>ERROR: Please choose above or below.</h3>\n" );
		document_close();
		exit( 0 );
	}

	null_above = ( strcmp( above_below, "above" ) == 0 );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date_string,
					&end_date_string,
					application_name,
					appaserver_parameter_file->
						database_management_system,
					(PROCESS_GENERIC_OUTPUT *)0,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
		"<h3>ERROR: no data available for these dates.</h3>\n" );
		document_close();
		exit( 0 );
	}

	begin_date =
		date_new_yyyy_mm_dd_hhmm(
			begin_date_string,
			begin_time_string );

	end_date =
		date_new_yyyy_mm_dd_hhmm(
			end_date_string,
			end_time_string );

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

	measurement_update_parameter =
		measurement_update_parameter_new(
					application_name,
					station,
					datatype,
					MEASUREMENT_UPDATE_METHOD,
					login_name,
					parameter_dictionary,
					notes );

	measurement_backup = 
		measurement_backup_new(
				application_name, 
				MEASUREMENT_UPDATE_METHOD,
				login_name,
				measurement_update_parameter->now_date,
				measurement_update_parameter->now_time );

	measurement_backup->insert_pipe =
		measurement_backup_open_insert_pipe(
					application_name );

	table_name = get_table_name( application_name, "measurement" );

	sprintf(buffer,
		"get_folder_data	application=%s			  "
		"			folder=measurement		  "
		"			select='%s'			  "
		"			where=\"%s\"			 |"
		"tr '%c' ','						 |"
		"sed 's/,/|/4'	 					  ",
		application_name,
		MEASUREMENT_SELECT_LIST,
		where_clause,
		FOLDER_DATA_DELIMITER );

	input_pipe = popen( buffer, "r" );

	sprintf(buffer,
		"update_statement.e %s %s | sql.e",
		table_name,
		MEASUREMENT_KEY_LIST );

	update_pipe = popen( buffer, "w" );

	/* Sample input: "BD,bottom_temperature,1999-01-01,1000|5.00" */
	/* ---------------------------------------------------------- */
	while( get_line( buffer, input_pipe ) )
	{
		piece(	measurement_date_string,
			',',
			buffer,
			MEASUREMENT_DATE_PIECE );

		piece(	measurement_time_string,
			',',
			buffer,
			MEASUREMENT_TIME_PIECE );

		if ( measurement_date ) date_free( measurement_date );

		measurement_date =
			date_new_yyyy_mm_dd_hhmm(
				measurement_date_string,
				measurement_time_string );

		if ( !measurement_date )
		{
			fprintf( stderr, 
		"ERROR in %s/%s(): cannot get measurement_date\n",
				 __FILE__,
				 __FUNCTION__ );
			exit( 1 );
		}

		piece( primary_key_half, '|', buffer, 0 );

		if ( !piece( value_half, '|', buffer, 1 ) )
		{
			fprintf( stderr, 
		"ERROR in %s/%s(): the sed didn't work\n",
				 __FILE__,
				 __FUNCTION__ );
			exit( 1 );
		}

		if ( measurement_date->current < begin_date->current
		||   measurement_date->current > end_date->current )
		{
			output_buffer( buffer, value_half );
		}
		else
		{
			/* Stub */
			/* ---- */
			*results_string = '\0';

			if ( execute )
			{
				if ( *results_string )
				{
					backup_and_update(
						update_pipe,
						primary_key_half,
						results_string,
						buffer,
						measurement_backup,
						execute );

				}
			}
			else
			{
				output_buffer( buffer, results_string );
			}
		}
	}

	pclose( input_pipe );
	pclose( update_pipe );
	measurement_backup_close( measurement_backup->insert_pipe );

	if ( execute )
	{
		char counter_string[ 128 ];

		sprintf( counter_string, "%d", counter );
		dictionary_add_string( 	parameter_dictionary,
					"count",
					counter_string );

		measurement_update_parameter_save(
				measurement_update_parameter );

		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	printf( "%d\n", counter );

	exit( 0 );
} /* main() */

void output_buffer(		char *buffer,
				char *results_string )
{
	printf( "%s,%s\n", buffer, results_string );
}

void backup_and_update(		FILE *update_pipe,
				char *primary_key_half,
				char *results_string,
				char *buffer,
				MEASUREMENT_BACKUP *measurement_backup,
				boolean execute )
{
	char comma_delimited_record[ 256 ];

	fprintf(update_pipe, 
		"%s|measurement_value=%s\n",
		primary_key_half,
		results_string );
	
	fprintf(update_pipe, 
		"%s|measurement_update_method=%s\n",
		primary_key_half,
		MEASUREMENT_UPDATE_METHOD );

	strcpy( comma_delimited_record, buffer );
	search_replace_string(	comma_delimited_record,
				"|",
				"," );

	measurement_backup_insert(
		measurement_backup->insert_pipe,
		measurement_backup->measurement_update_date,
		measurement_backup->measurement_update_time,
		measurement_backup->measurement_update_method,
		measurement_backup->login_name,
		comma_delimited_record,
		(execute) ? 'y' : 'n',
		',' );

} /* backup_and_update() */

