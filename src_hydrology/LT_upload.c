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
/*
void delete_measurement(	char *application_name,
				char *input_filename,
				char *station );
*/

boolean extract_static_attributes(
				char **error_message,
				char *measurement_date,
				char *measurement_time,
				char *input_string );

void remove_error_file(		char *error_filename );

int LT_upload(		
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
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*input_filename || strcmp( input_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	load_count =
		LT_upload(
			input_filename,
			station,
			execute );

	fflush( stdout );

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

int LT_upload(		char *input_filename,
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
			basename_get_filename(
				input_filename ) );

	if ( execute )
	{
/*
#define INSERT_MEASUREMENT		\
	"station,measurement_date,measurement_time,measurement_value,datatype"
*/
		sprintf(
		 sys_string,
		 "shef_upload_datatype_convert %d %d '%c'		  |"
		 "count.e %d 'LT Load count'				  |"
		 "piece_inverse.e %d '%c'				  |"
		 "tr '%c' ','						  |"
		 "measurement_insert_order ',' 0,4,1,2,3		  |"
		 "measurement_insert ignored realdata y 2>&1		  |"
		 "html_paragraph_wrapper.e				   ",
		 SHEF_CONVERT_STATION_PIECE,
		 SHEF_CONVERT_CODE_PIECE,
		 SHEF_CONVERT_DELIMITER,
		 STDERR_COUNT,
		 SHEF_CONVERT_CODE_PIECE,
		 SHEF_CONVERT_DELIMITER,
		 SHEF_CONVERT_DELIMITER );

		output_pipe = popen( sys_string, "w" );

	}
	else
	{
		sprintf( sys_string,
		"shef_upload_datatype_convert %d %d '%c'		|"
		"piece_inverse.e %d '%c'				|"
"tee -a /var/log/appaserver/appaserver_hydrology.err |"
		"measurement_insert_order '^' 0,4,1,2,3		  	|"
		"measurement_insert ignored realdata n 2>&1		|"
		"cat							 ",
		 	 SHEF_CONVERT_STATION_PIECE,
		 	 SHEF_CONVERT_CODE_PIECE,
		 	 SHEF_CONVERT_DELIMITER,
		 	 SHEF_CONVERT_CODE_PIECE,
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
				/* Skip date_time, SN  */
				/* or any empty cells. */
				/* ------------------- */
				if ( value_piece <= 1
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
				list_next( header_string_list );

			} /* for each cell in row */

		} while ( list_next( header_string_list ) );

	} /* for each line */

	fclose( input_file );
	fclose( error_file );
	pclose( output_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e %d | html_table.e 'Errors' '' '|'",
			 error_filename,
			 STDERR_COUNT );
		if ( system( sys_string ) ){};
	}

	remove_error_file( error_filename );
	return load_count;

} /* LT_upload() */

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
	if ( system( sys_string ) ){};
}

