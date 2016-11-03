/* ---------------------------------------------------	*/
/* src_hydrology/google_map_station_operation.c		*/
/* ---------------------------------------------------	*/
/* This is the operation process for the google map     */
/* station button.					*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "date.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "google_map_station.h"
#include "environ.h"
#include "station.h"
#include "google_map.h"
#include "application_constants.h"

/* Constants */
/* --------- */
#define SEMAPHORE_TEMPLATE	"%s/%s/google_maps_semaphore_%s.html"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *station_name;
	char url_filename[ 256 ];
	char prompt_filename[ 256 ];
	char semaphore_filename[ 256 ];
	char window_name[ 128 ];
	char *server_address;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	FILE *output_file;
	STATION *station;
	char *parent_process_id_string;
	char *operation_row_count_string;
	char sys_string[ 1024 ];
	char *results;
	char increment_string[ 16];
	char *session;
	boolean group_first_time;
	boolean group_last_time;
	char *mode;

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s application process station process_id operation_row_count session\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	station_name = argv[ 3 ];
	parent_process_id_string = argv[ 4 ];
	operation_row_count_string = argv[ 5 ];
	session = argv[ 6 ];

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

	appaserver_parameter_file = new_appaserver_parameter_file();

	sprintf(	semaphore_filename,
			SEMAPHORE_TEMPLATE,
			appaserver_parameter_file->
				appaserver_mount_point,
			application_name,
			parent_process_id_string );

	if ( !timlib_file_exists( semaphore_filename ) )
	{
		char sys_string[ 1024 ];

		sprintf( sys_string, "echo 0 > %s", semaphore_filename );
		fflush( stdout );
		system( sys_string );
		fflush( stdout );
		group_first_time = 1;
	}
	else
	{
		group_first_time = 0;
	}

	sprintf( sys_string, "cat %s", semaphore_filename );
	results = pipe2string( sys_string );
	sprintf( increment_string, "%d", atoi( results ) + 1 );
	sprintf( sys_string,
		 "echo %s > %s",
		 increment_string,
		 semaphore_filename );
	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	group_last_time =
		( strcmp(	operation_row_count_string,
				increment_string ) == 0 );

	document = document_new( "", application_name );
	
	if ( group_first_time )
	{
		document_set_output_content_type( document );

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
	}

	sprintf(	url_filename,
			GOOGLE_MAP_STATION_URL_TEMPLATE,
			appaserver_parameter_file->
				appaserver_mount_point,
			application_name,
			session );

	if ( group_first_time )
	{
		mode = "w";
	}
	else
	{
		mode = "a";
	}

	if ( ! ( output_file = fopen( url_filename, mode ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot open %s for write.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			url_filename );
		exit( 1 );
	}

	if ( group_first_time )
	{
		char *google_map_key_data;

		google_map_key_data =
			google_map_station_get_map_key(
				application_name );

		google_map_station_output_heading(
				output_file,
				google_map_key_data,
				(char *)0 /* balloon_click_parameter */,
				(char *)0 /* balloon_click_function */,
				application_name,
				0 /* absolute_position_top */,
				0 /* absolute_position_left */ );
	}

	station = station_new( station_name );

	if ( !station_load(	
				&station->lat_nad83,
				&station->long_nad83,
				&station->agency,
				&station->ground_surface_elevation_ft,
				&station->ground_surface_elevation_null,
				&station->vertical_datum,
				&station->ngvd29_navd88_conversion,
				&station->comments,
				application_name,
				station->station_name ) )
	{
		printf( "<p>Error: cannot find station = (%s)\n",
			station_name );
		document_close();
		fclose( output_file );
		exit( 0 );
	}

	if ( !station->lat_nad83 || !station->long_nad83 )
	{
		printf(
		"<p>Warning: either the latitude or longitude is not set.\n" ); 
	}
	else
	{
		google_map_station_output_station(
			output_file,
			station->station_name,
			station->long_nad83,
			station->lat_nad83 );
	}

	if ( group_last_time )
	{
		google_map_station_output_body(
			output_file,
			0 /* internet_explorer doesn't matter */,
			0 /* not with_table */,
			(char *)0 /* additional_javascript */ );

		fprintf( output_file, "</body>\n" );
		fprintf( output_file, "</html>\n" );
	}

	fclose( output_file );

	if ( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' )
	{
		server_address = appaserver_library_get_server_address();

		sprintf(	prompt_filename,
				GOOGLE_MAP_STATION_HTTP_PROMPT_TEMPLATE,
				application_get_http_prefix( application_name ),
				server_address,
				application_name,
				session );
	}
	else
	{
		sprintf(	prompt_filename,
				GOOGLE_MAP_STATION_PROMPT_TEMPLATE,
				application_name,
				session );
	}

	if ( group_last_time )
	{
		printf( "<body bgcolor=\"%s\" onload=\"",
			application_get_background_color( application_name ) );

		sprintf( window_name, "%s", process_name );
		printf( "window.open('%s','%s');",
			prompt_filename,
			window_name );

		printf( "\">\n" );
		printf( "<h1>Google Earth Station Viewer " );
		fflush( stdout );
		system( "date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h1>\n" );

		printf( "<a href='%s' target=%s>Press to view map.</a>\n",
			prompt_filename,
			window_name );

		document_close();
	}

	process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );

	exit( 0 );
} /* main() */

