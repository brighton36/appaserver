/* ------------------------------------------------------*/
/* src_benthic/compare_sampling_point_with_waypoint.c	 */
/* ------------------------------------------------------*/
/* Freely available software: see Appaserver.org	 */
/* ------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "date_convert.h"
#include "piece.h"
#include "query.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "hashtbl.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "benthic_library.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define OUTPUT_FILE			"%s/%s/compare_waypoint_%d.csv"
#define FTP_PREPEND_FILE		"%s://%s/%s/compare_waypoint_%d.csv"
#define FTP_NONPREPEND_FILE		"/%s/compare_waypoint_%d.csv"

/* Prototypes */
/* ---------- */
FILE *get_input_pipe(		char *application_name,
				LIST *collection_list,
				LIST *project_list,
				char delimiter );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				LIST *collection_list,
				LIST *project_list );

void output_compare_waypoint(
				char *application_name,
				char *appaserver_mount_point,
				pid_t process_id,
				LIST *collection_list,
				LIST *project_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *collection_list_string;
	char *project_list_string;
	LIST *collection_list;
	LIST *project_list;
	char title[ 128 ];
	char sub_title[ 65536 ];
	int results;

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s application process_name collection_list project_list\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	collection_list_string = argv[ 3 ];
	project_list_string = argv[ 4 ];

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

	document = document_new( "", application_name );
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
	
	document_output_body(
				document->application_name,
				document->onload_control_string );

	collection_list = list_string2list( collection_list_string, ',' );
	project_list = list_string2list( project_list_string, ',' );

	get_title_and_sub_title(
			title,
			sub_title,
			process_name,
			collection_list,
			project_list );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	results = system( "date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	if ( !*collection_list_string
	||   strcmp( collection_list_string, "collection_name" ) == 0 )
	{
		printf(
		"<h3>Please choose a collection.</h3>\n" );
		document_close();
		exit( 0 );
	}

	output_compare_waypoint(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point,
				getpid(),
				collection_list,
				project_list );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void output_compare_waypoint(
				char *application_name,
				char *appaserver_mount_point,
				pid_t process_id,
				LIST *collection_list,
				LIST *project_list )
{
	char sys_string[ 1024 ];
	char ftp_filename[ 256 ];
	char process_output_filename[ 256 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char *heading_string;
	char delimiter = ',';
	int results;

	heading_string =
"Season,Basin,Cell,Date,Time,Latitude Sampled,Longitude Sampled,Sample Success,Latitude Targeted,Longitude Targeted,Distance Meters";

	sprintf(process_output_filename,
	 	OUTPUT_FILE,
	 	appaserver_mount_point,
	 	application_name,
	 	process_id );

	sprintf(	sys_string,
			"echo \"%s\" > %s",
			heading_string,
			process_output_filename );
	results = system( sys_string );

	sprintf( sys_string, "cat >> %s", process_output_filename );
	output_pipe = popen( sys_string, "w" );

	input_pipe = get_input_pipe( 
				application_name,
				collection_list,
				project_list,
				delimiter );

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
	}

	pclose( output_pipe );
	pclose( input_pipe );

	if ( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' )
	{
		sprintf(ftp_filename, 
			FTP_PREPEND_FILE, 
			application_get_http_prefix( application_name ),
		 	appaserver_library_get_server_address(),
		 	application_name,
		 	process_id );
	}
	else
	{
		sprintf(ftp_filename,
	 		FTP_NONPREPEND_FILE, 
	 		application_name,
	 		process_id );
	}

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"Compare Waypoint: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_compare_waypoint() */

void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *process_name,
			LIST *collection_list,
			LIST *project_list )
{
	format_initial_capital( title, process_name );

	strcpy( sub_title,
		benthic_library_get_collection_project_combined_string(
			collection_list,
			project_list ) );

} /* get_title_and_sub_title() */

FILE *get_input_pipe(	char *application_name,
			LIST *collection_list,
			LIST *project_list,
			char delimiter )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
"compare_sampling_point_with_waypoint_process %s \"%s\" \"%s\" '%c'",
		 application_name,
		 list_display( collection_list ),
		 list_display( project_list ),
		 delimiter );

	return popen( sys_string, "r" );

} /* get_input_pipe() */

