/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/load_cr300_data.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "julian.h"
#include "query.h"
#include "basename.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "column.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "application.h"
#include "hydrology_library.h"
#include "station.h"
#include "datatype.h"
#include "units.h"
#include "hydrology.h"
#include "appaserver_link_file.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define QUEUE_TOP_BOTTOM_LINES		100
#define FILENAME_STEM			"load_cr300_bad"
#define PROMPT				"Bad Records File."

/* Prototypes */
/* ---------- */
void load_cr300_datatype_list_display( LIST *datatype_list );

/* Returns measurement_count */
/* ------------------------- */
int load_cr300_filespecification(	char *error_filespecification,
					LIST *datatype_list,
					char *input_filespecification,
					char *station,
					char execute_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char execute_yn;
	char *input_filespecification;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *input_directory;
	char *station;
	char *error_filespecification;
	char *error_ftp_filename;
	int measurement_count;
	char *process_name;
	int process_id = getpid();
	char format_buffer[ 128 ];
	APPASERVER_LINK_FILE *appaserver_link_file;
	LIST *datatype_list;

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
			 "Usage: %s process filename station execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	input_filespecification = argv[ 2 ];
	station = argv[ 3 ];
	execute_yn = *argv[ 4 ];

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

	/* Display the title and run stamp. */
	/* -------------------------------- */
	printf( "<h1>%s</h1>\n",
	 	format_initial_capital(
			format_buffer,
			process_name ) );

	printf( "<h2>" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	fflush( stdout );
	printf( "</h2>\n" );
	fflush( stdout );

	input_directory = basename_get_directory( input_filespecification );

	if ( !input_directory || !*input_directory )
	{
		printf( "<h3>ERROR: Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*station
	||   strcmp( station, "station" ) == 0 )
	{
		printf(
			"<h3>ERROR: Please choose a station.</h3>\n" );
		document_close();
		exit( 1 );
	}

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->
				document_root,
			FILENAME_STEM,
			application_name,
			process_id,
			(char *)0 /* session */,
			"txt" );

	error_filespecification =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	error_ftp_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	datatype_list = list_new();

	measurement_count =
		load_cr300_filespecification(
			error_filespecification,
			datatype_list,
			input_filespecification,
			station,
			execute_yn );

	load_cr300_datatype_list_display( datatype_list );

	if ( execute_yn == 'y' )
		printf( "<p>Loaded %d measurements.\n",
			measurement_count );
	else
		printf( "<p>Did not load %d measurements.\n",
			measurement_count );

	if ( timlib_file_populated( error_filespecification ) )
	{
		printf( "<br>" );
		appaserver_library_output_ftp_prompt(
				error_ftp_filename, 
				PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}

	if ( execute_yn == 'y' )
	{
		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}

	document_close();

	return 0;

} /* main() */

/* Returns measurement_count */
/* ------------------------- */
int load_cr300_filespecification(
			char *error_filespecification,
			LIST *datatype_list,
			char *input_filespecification,
			char *station,
			char execute_yn )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe;
	int measurement_count;
	char input_buffer[ 1024 ];
	char datatype_name[ 128 ];
	DATATYPE *datatype;

	/* Open input pipe */
	/* --------------- */
	sprintf( sys_string,
		 "parse_alias_data '%s' '%s' TIMESTAMP 2>%s",
		 input_filespecification,
		 station,
		 error_filespecification );

	input_pipe = popen( sys_string, "r" );

	/* Open output pipe */
	/* ---------------- */
	if ( execute_yn == 'y' )
	{
		 sprintf(sys_string,
		 	 "tr '^' ','					  |"
		 	 "measurement_insert ignored realdata n 1>&2	  |"
		 	 "cat >>%s					   ",
		 	 error_filespecification );
	}
	else
	{
		sprintf(sys_string,
			"queue_top_bottom_lines.e %d			  |"
			"html_table.e '' '%s' '^' %s		 	   ",
			QUEUE_TOP_BOTTOM_LINES,
		 	MEASUREMENT_INSERT_LIST,
		 	"left,left,left,left,right" );
	}

	output_pipe = popen( sys_string, "w" );

	measurement_count = 0;

	while ( timlib_get_line( input_buffer, input_pipe, 1024 ) )
	{
		if ( !piece(	datatype_name,
				'^',
				input_buffer,
				1 ) )
		{
			continue;
		}

		datatype =
			datatype_set_or_get_datatype(
				datatype_list,
				datatype_name );

		datatype->measurement_count++;

		fprintf( output_pipe, "%s\n", input_buffer );
		measurement_count++;
	}

	pclose( input_pipe );
	pclose( output_pipe );

	return measurement_count;

} /* load_cr300_filespecification() */

void load_cr300_datatype_list_display( LIST *datatype_list )
{
	DATATYPE *datatype;
	char sys_string[ 1024 ];
	FILE *output_pipe;

	strcpy( sys_string,
	"html_table.e '^^Datatype Summary' ',count' '^' 'left,right'" );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	if ( list_rewind( datatype_list ) )
	{
		do {
			datatype = list_get_pointer( datatype_list );

			fprintf( output_pipe,
				 "%s^%d\n",
				 datatype->datatype_name,
				 datatype->measurement_count );

		} while ( list_next( datatype_list ) );
	}

	pclose( output_pipe );
	fflush( stdout );

} /* load_cr300_datatype_list_display() */


