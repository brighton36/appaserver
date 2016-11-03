/* --------------------------------------------------- 	*/
/* src_capitolpops/composition_venue_count_output.c    	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "folder.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "environ.h"
#include "dictionary.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "application.h"
#include "appaserver_library.h"
#include "capitolpops.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define DEFAULT_OUTPUT_MEDIUM	"table"
#define FILENAME_STEM		"composition_venue_count"

/*
#define OUTPUT_SPREADSHEET_TEMPLATE		"%s/%s/composition_venue_count_%s.csv"
#define FTP_SPREADSHEET_PREPEND_TEMPLATE 	"%s://%s/%s/composition_venue_count_%s.csv"
#define FTP_SPREADSHEET_NOPREPEND_TEMPLATE	"/%s/composition_venue_count_%s.csv"
*/

#define VENUE_HEADING "city,venue,composition,latest_date,count"
#define COMPOSITION_HEADING "composition,latest_date,count"
#define VENUE_JUSTIFY "left,left,left,left,right"
#define COMPOSITION_JUSTIFY "left,left,right"

/* Prototypes */
/* ---------- */
void composition_venue_count_output_spreadsheet(
			char *application_name,
			char *session,
			FILE *input_pipe,
			char *document_root_directory );

void composition_count_output_table(
			FILE *input_pipe );

void composition_venue_count_output_table(
			FILE *input_pipe );

void composition_count_output(
			char *output_medium );

void composition_venue_count_output(
			char *application_name,
			char *session,
			char *composition,
			char *city,
			char *venue,
			char *output_medium,
			char *appaserver_mount_point );

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};
	char *session;
	char *process_name;
	char *composition;
	char *city;
	char *venue;
	char *output_medium;
	char title[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s application session process_name composition city venue output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	session = argv[ 2 ];
	process_name = argv[ 3 ];
	composition = argv[ 4 ];
	city = argv[ 5 ];
	venue = argv[ 6 ];
	output_medium = argv[ 7 ];

	if ( !*output_medium )
		output_medium = "stdout";
	else
	if ( strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = DEFAULT_OUTPUT_MEDIUM;

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	format_initial_capital( title, process_name );

	document = document_new( "", application_name );
	document->output_content_type = 1;
	
	document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
	document_output_body(
				document->application_name,
				document->onload_control_string );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "/bin/date '+%m/%d/%Y %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	if ( strcmp( composition, "composition" ) == 0
	&&   strcmp( city, "city" ) == 0
	&&   strcmp( venue, "venue" ) == 0 )
	{
		composition_count_output( output_medium );
	}
	else
	{
		composition_venue_count_output(
			application_name,
			session,
			composition,
			city,
			venue,
			output_medium,
			appaserver_parameter_file->
				document_root );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

void composition_count_output(
			char *output_medium )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;

	sprintf( sys_string,
		 "composition_venue_count \"%s\" \"%s\" \"%s\"",
		 "composition",
		 "city",
		 "venue" );

	input_pipe = popen( sys_string, "r" );

/*
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		composition_count_output_spreadsheet(
				application_name,
				session,
				input_pipe,
				appaserver_parameter_file->
					document_root );
	}
	else
	{
		composition_count_output_table( input_pipe );
	}
*/

	composition_count_output_table( input_pipe );
	pclose( input_pipe );

} /* composition_count_output() */

void composition_venue_count_output(
			char *application_name,
			char *session,
			char *composition,
			char *city,
			char *venue,
			char *output_medium,
			char *document_root_directory )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;

	sprintf( sys_string,
		 "composition_venue_count \"%s\" \"%s\" \"%s\"",
		 composition,
		 city,
		 venue );

	input_pipe = popen( sys_string, "r" );

	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		composition_venue_count_output_spreadsheet(
				application_name,
				session,
				input_pipe,
				document_root_directory );
	}
	else
	{
		composition_venue_count_output_table( input_pipe );
	}

	pclose( input_pipe );

} /* composition_venue_count_output() */

void composition_count_output_table(
			FILE *input_pipe )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char input_line[ 1024 ];

	sprintf( sys_string,
		 "html_table.e '' '%s' '%c' '%s'",
		 COMPOSITION_HEADING,
		 FOLDER_DATA_DELIMITER,
		 COMPOSITION_JUSTIFY );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_line, input_pipe ) )
	{
		if ( timlib_strncmp( input_line, RECORD_LABEL ) == 0 )
		{
			fprintf( output_pipe,
				 "%s\n",
				 input_line + strlen( RECORD_LABEL ) );
		}

		if ( timlib_strncmp( input_line, TOTAL_LABEL ) == 0 )
		{
			fprintf( output_pipe,
				 "Total%c%c%s\n",
				 FOLDER_DATA_DELIMITER,
				 FOLDER_DATA_DELIMITER,
				 input_line + strlen( TOTAL_LABEL ) );
		}

	}

	pclose( output_pipe );

} /* composition_count_output_table() */

void composition_venue_count_output_table(
			FILE *input_pipe )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char input_line[ 1024 ];

	sprintf( sys_string,
		 "html_table.e '' '%s' '%c' '%s'",
		 VENUE_HEADING,
		 FOLDER_DATA_DELIMITER,
		 VENUE_JUSTIFY );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_line, input_pipe ) )
	{
		if ( timlib_strncmp( input_line, RECORD_LABEL ) == 0 )
		{
			fprintf( output_pipe,
				 "%s\n",
				 input_line + strlen( RECORD_LABEL ) );
		}

		if ( timlib_strncmp( input_line, TOTAL_LABEL ) == 0 )
		{
			fprintf( output_pipe,
				 "Total%c%c%c%c%s\n",
				 FOLDER_DATA_DELIMITER,
				 FOLDER_DATA_DELIMITER,
				 FOLDER_DATA_DELIMITER,
				 FOLDER_DATA_DELIMITER,
				 input_line + strlen( TOTAL_LABEL ) );
		}

	}

	pclose( output_pipe );

} /* composition_venue_count_output_table() */

void composition_venue_count_output_spreadsheet(
			char *application_name,
			char *session,
			FILE *input_pipe,
			char *document_root_directory )
{
	char sys_string[ 256 ];
	char *output_filename;
	char *ftp_filename;
	FILE *output_file;
	FILE *output_pipe;
	char input_line[ 1024 ];
	char heading[ 256 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			FILENAME_STEM,
			application_name,
			0 /* process_id */,
			session,
			"csv" );

	output_filename =
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

	ftp_filename =
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

/*
	sprintf( output_filename,
		 OUTPUT_SPREADSHEET_TEMPLATE,
		 appaserver_mount_point,
		 application_name,
		 session );

	if ( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' )
	{
		sprintf( ftp_filename,
			 FTP_SPREADSHEET_PREPEND_TEMPLATE,
			 application_get_http_prefix( application_name ),
			 appaserver_library_get_server_address(),
		 	 application_name,
		 	 session );
	}
	else
	{
		sprintf( ftp_filename,
			 FTP_SPREADSHEET_NOPREPEND_TEMPLATE,
		 	 application_name,
		 	 session );
	}
*/

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s</h2>\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fclose( output_file );

	sprintf( sys_string,
		 "double_quote_comma_delimited.e '%c' > %s",
		 FOLDER_DATA_DELIMITER,
		 output_filename );

	output_pipe = popen( sys_string, "w" );

	format_initial_capital( heading, VENUE_HEADING );
	search_replace_character( heading, ',', FOLDER_DATA_DELIMITER );
	fprintf( output_pipe, "%s\n", heading );

	while( get_line( input_line, input_pipe ) )
	{
		if ( timlib_strncmp( input_line, RECORD_LABEL ) == 0 )
		{
			fprintf( output_pipe,
				 "%s\n",
				 input_line + strlen( RECORD_LABEL ) );
		}

		if ( timlib_strncmp( input_line, TOTAL_LABEL ) == 0 )
		{
			fprintf( output_pipe,
				 "Total%c%c%c%c%s\n",
				 FOLDER_DATA_DELIMITER,
				 FOLDER_DATA_DELIMITER,
				 FOLDER_DATA_DELIMITER,
				 FOLDER_DATA_DELIMITER,
				 input_line + strlen( TOTAL_LABEL ) );
		}

	}

	pclose( output_pipe );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* composition_venue_count_output_spreadsheet() */

