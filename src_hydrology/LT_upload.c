/* ------------------------------------------		*/
/* $APPASERVER_HOME/src_hydrology/LT_upload.c		*/
/* ------------------------------------------		*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "date_convert.h"
#include "application.h"
#include "basename.h"
#include "application_constants.h"

/* Constants */
/* --------- */
#define SHEF_CONVERT_STATION_PIECE	0
#define SHEF_CONVERT_CODE_PIECE		4
#define SHEF_CONVERT_DELIMITER		'^'

#define STDERR_COUNT			300

#define INSERT_MEASUREMENT		\
	"station,measurement_date,measurement_time,measurement_value,datatype"

/* Prototypes */
/* ---------- */
void delete_measurement(	char *application_name,
				char *input_filename,
				char *station );

boolean extract_static_attributes(
				char **error_message,
				char *measurement_date,
				char *measurement_time,
				char *input_string );

void remove_error_file(		char *error_filename );

int load_measurement(		char *application_name,
				char *input_filename,
				char *station,
				boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *station;
	boolean execute;
	char *input_filename;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int load_count = 0;
	char buffer[ 128 ];

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s process_name station filename execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	station = argv[ 2 ];
	input_filename = argv[ 3 ];
	execute = (*argv[ 4 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h2>%s\n", format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	system( "date '+%x %H:%M'" );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*input_filename || strcmp( input_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( execute )
	{
		delete_measurement(	application_name,
					input_filename,
					station );
	}

	load_count =
		load_measurement(
			application_name,
			input_filename,
			station,
			execute );

	if ( execute )
	{
		printf( "<p>Process complete with %d measurements.\n",
			load_count );

		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( "<p>Process did not load %d measurements.\n",
			load_count );
	}

	document_close();

	return 0;

} /* main() */

int load_measurement(	char *application_name,
			char *input_filename,
			char *station,
			boolean execute )
{
	char sys_string[ 1024 ];
	FILE *input_file;
	FILE *error_file;
	char input_buffer[ 4096 ];
	char header_buffer[ 4096 ];
	LIST *header_string_list;
	char *shef_code;
	char measurement_value[ 128 ];
	int value_piece;
	char *measurement_table_name;
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char error_filename[ 128 ];
	FILE *output_pipe;
	int load_count = 0;
	int line_number = 0;
	char *error_message;
	char basename_filename[ 128 ];

	sprintf(error_filename,
		"/tmp/lt_load_%d.txt", getpid() );

	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		return 0;
	}

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		printf( "<h2>ERROR: cannot open %s for read</h2>\n",
			input_filename );

		fclose( error_file );
		return 0;
	}

	trim_index(	basename_filename,
			basename_get_filename( input_filename ) );

	measurement_table_name =
		get_table_name(	application_name,
				"measurement" );

	if ( execute )
	{
		sprintf(
		 sys_string,
		 "shef_upload_datatype_convert %d %d '%c'		  |"
		 "count.e %d 'LT Load count'				  |"
		 "piece_inverse.e %d '%c'				  |"
		 "insert_statement table=%s field=%s del='%c' compress=n  |"
		 "sql.e 2>&1						  |"
		 "grep -vi duplicate					  |"
		 "html_paragraph_wrapper.e				   ",
		 SHEF_CONVERT_STATION_PIECE,
		 SHEF_CONVERT_CODE_PIECE,
		 SHEF_CONVERT_DELIMITER,
		 STDERR_COUNT,
		 SHEF_CONVERT_CODE_PIECE,
		 SHEF_CONVERT_DELIMITER,
		 measurement_table_name,
		 INSERT_MEASUREMENT,
		 SHEF_CONVERT_DELIMITER );

		output_pipe = popen( sys_string, "w" );

	}
	else
	{
		sprintf( sys_string,
		"queue_top_bottom_lines.e 50				|"
		"html_table.e 'Insert into MEASUREMENT' %s '%c'		 ",
			 INSERT_MEASUREMENT,
			 SHEF_CONVERT_DELIMITER );

		output_pipe = popen( sys_string, "w" );
	}

	/* Get the header */
	/* --------------- */
	if ( !timlib_get_line( header_buffer, input_file, 4096 ) )
	{
		printf( "<h3>Error: cannot get the header.</h3>\n" );
		pclose( output_pipe );
		fclose( input_file );
		return 0;
	}

	line_number++;
	header_string_list = list_string_to_list( header_buffer, ',' );

	if ( !list_length( header_string_list ) )
	{
		printf( "<h3>Error: empty header.</h3>\n" );
		pclose( output_pipe );
		fclose( input_file );
		return 0;
	}

	while( timlib_get_line( input_buffer, input_file, 4096 ) )
	{
		line_number++;
		trim( input_buffer );
		if ( !*input_buffer ) continue;

		if ( !extract_static_attributes(
			&error_message,
			measurement_date,
			measurement_time,
			input_buffer ) )
		{
			if ( *error_message )
			{
				fprintf(error_file,
			"Warning in line %d: ignored = (%s) because %s.\n",
					line_number,
					input_buffer,
					error_message );
			}
			else
			{
				fprintf(error_file,
			"Warning in line %d: ignored = (%s)\n",
					line_number,
					input_buffer );
			}
			continue;
		}

		list_rewind( header_string_list );

		do {

			for(	value_piece = 0;
				piece(	measurement_value,
					',',
					input_buffer,
					value_piece );
				value_piece++ )
			{
				/* ------------------- */
				/* Skip the date_time  */
				/* or any empty cells. */
				/* ------------------- */
				if ( value_piece == 0
				||   !*measurement_value )
				{
					list_next( header_string_list );
					continue;
				}

				shef_code = list_get( header_string_list );

				fprintf( output_pipe,
					 "%s^%s^%s^%s^%s\n",
					 station,
					 measurement_date,
					 measurement_time,
					 measurement_value,
					 shef_code );
					 
				load_count++;

			} /* for each cell in row */

		} while ( list_next( header_string_list ) );

	} /* for each line */

	fclose( input_file );
	fclose( error_file );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e %d | html_table.e 'Errors' '' '|'",
			 error_filename,
			 STDERR_COUNT );
		system( sys_string );
	}

	remove_error_file( error_filename );
	return load_count;

} /* load_measurement() */

#define DELETE_FIELD_LIST	\
	"station,datatype,measurement_date,measurement_time"

void delete_measurement(	char *application_name,
				char *input_filename,
				char *station )
{
#ifdef NOT_DEFINED

	FILE *input_file;
	FILE *collection_delete_pipe;
	FILE *results_delete_pipe;
	char sys_string[ 1024 ];
	char input_string[ 4096 ];
	char *table_name;
	char station[ 128 ];
	char collection_date_international[ 128 ];
	char depth_meters[ 128 ];
	char longitude[ 128 ];
	char latitude[ 128 ];
	char collection_time_without_colon[ 128 ];

	/* Collection */
	/* ---------- */
	table_name =
		get_table_name(
			application_name, "collection" );

	sprintf( sys_string,
		 "sort -u						|"
		 "delete_statement.e t=%s f=%s d='|'			|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e				 ",
		 table_name,
		 DELETE_FIELD_LIST );

	collection_delete_pipe = popen( sys_string, "w" );

	/* Results */
	/* ------- */
	table_name =
		get_table_name(
			application_name, "results" );

	sprintf( sys_string,
		 "sort -u						|"
		 "delete_statement.e t=%s f=%s d='|'			|"
		 "count.e %d 'WQ delete collection count'		|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e				 ",
		 table_name,
		 DELETE_FIELD_LIST,
		 STDERR_COUNT );

	results_delete_pipe = popen( sys_string, "w" );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		pclose( collection_delete_pipe );
		pclose( results_delete_pipe );
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	/* Skip the header */
	/* --------------- */
	if ( !get_line( input_string, input_file ) )
	{
		pclose( collection_delete_pipe );
		pclose( results_delete_pipe );
		return;
	}

	while( get_line( input_string, input_file ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !extract_station_collection_attributes(
			(char **)0,
			station,
			collection_date_international,
			collection_time_without_colon,
			depth_meters,
			longitude,
			latitude,
			input_string,
			water_quality->load_column_list,
			application_name ) )
		{
			continue;
		}

		fprintf( collection_delete_pipe,
			 "%s|%s|%s\n",
			 station,
			 collection_date_international,
			 collection_time_without_colon );

		fprintf( results_delete_pipe,
			 "%s|%s|%s\n",
			 station,
			 collection_date_international,
			 collection_time_without_colon );
	}

	fclose( input_file );
	pclose( collection_delete_pipe );
	pclose( results_delete_pipe );

#endif

} /* delete_measurement() */

boolean extract_static_attributes(
			char **error_message,
			char *measurement_date,
			char *measurement_time,
			char *input_string )
{
	char date_time_buffer[ 128 ];
	char date_buffer[ 64 ];
	char time_buffer[ 64 ];
	char hour_buffer[ 32 ];
	char minute_buffer[ 32 ];
	static DATE *date = {0};

	if ( !date )
	{
		date = date_calloc();
	}

	if ( !piece_quote_comma(
		date_time_buffer,
		input_string,
		0 )
	||   !*date_time_buffer )
	{
		if ( error_message )
			*error_message =
			 	"Cannnot identify DateTime";
		return 0;
	}

	/* -------------------------------- */
	/* Looks like: 2018-08-31T14:30:22Z */
	/* -------------------------------- */

	piece( date_buffer, 'T', date_time_buffer, 0 );

	if ( !piece( time_buffer, 'T', date_time_buffer, 1 ) )
	{
		timlib_strcpy( measurement_date, date_buffer, 0 );
		timlib_strcpy( measurement_time, "null", 0 );
		return 1;
	}

	if ( strlen( time_buffer ) != 9 )
	{
		if ( error_message )
			*error_message =
			 	"Cannot get Zulo time";
		return 0;
	}

	/* --------------------- */
	/* Looks like: 14:30:22Z */
	/* --------------------- */

	if ( *(time_buffer + 8 ) != 'Z' )
	{
		if ( error_message )
			*error_message =
			 	"Expecting a 'Z' in the time";
		return 0;
	}

	piece( hour_buffer, ':', time_buffer, 0 );

	if ( !piece( minute_buffer, ':', time_buffer, 1 ) )
	{
		if ( error_message )
			*error_message =
			 	"Expecting a ':' in the time";
		return 0;
	}

	if ( !date_set_yyyy_mm_dd(
			date,
			date_buffer ) )
	{
		if ( error_message )
			*error_message =
			 	"Invalid date";
		return 0;
	}

	sprintf( time_buffer, "%s%s", hour_buffer, minute_buffer );

	if ( !date_set_time_hhmm(
			date,
			time_buffer /* hhmm */ ) )
	{
		if ( error_message )
			*error_message =
			 	"Invalid time";
		return 0;
	}

	/* Convert from zulu */
	/* ----------------- */
	date_increment_hours( date, date_get_utc_offset() );

	date_get_yyyy_mm_dd( measurement_date, date );
	date_get_hhmm( measurement_time, date );

	return 1;

} /* extract_static_attributes() */

void remove_error_file( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );
}

