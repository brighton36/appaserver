/* ----------------------------------------------------	*/
/* $APPASERVER_HOME/src_vegetation/load_photo.c		*/
/* ----------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "basename.h"
#include "piece.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "application.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define DESTINATION_FILESPEC	"%s/appaserver/%s/photos/%s/%s"
/*				 ^	       ^         ^  ^		*/
/*				 |	       |	 |  |		*/
/*				 |	       |	 | filename	*/
/*			  document_root   application  directory	*/
/* -------------------------------------------------------------------- */

#define RELATIVE_FILESPEC	"photos/%s/%s"
#define ATTRIBUTE_NAME		"filename"

/* Prototypes */
/* ---------- */
void insert_photo(		char *application_name,
				int cell_id,
				char *date_string,
				char *filename );

void load_photo(		char *application_name,
				int cell_id,
				char *source_filename_directory_session );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				int cell_id );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	int cell_id;
	char *date_taken_string;
	char title[ 128 ];
	char sub_title[ 128 ];
	FILE *input_file;
	char *source_filename_directory_session;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name date_taken cell_id filename\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	date_taken_string = argv[ 3 ];
	cell_id = atoi( argv[ 4 ] );
	source_filename_directory_session = argv[ 5 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

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

	get_title_and_sub_title(
			title,
			sub_title,
			process_name,
			cell_id );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !source_filename_directory_session
	||   !( input_file = fopen( source_filename_directory_session, "r" ) ) )
	{
		printf( "<h3>ERROR: please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}
	fclose( input_file );

	if ( !*collection_name
	||   strcmp( collection_name, "collection_name" ) == 0 )
	{
		printf( "<h3>ERROR: please choose a collection.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*location || strcmp( location, "location" ) == 0 )
	{
		printf( "<h3>ERROR: please choose a site.</h3>\n" );
		document_close();
		exit( 0 );
	}

	load_field_sheet_scan(
				application_name,
				project,
				collection_name,
				location,
				site_number,
				source_filename_directory_session );

	printf( "<h3>Process complete.</h3>\n" );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				char *project,
				char *collection_name,
				char *location,
				int site_number )
{
	format_initial_capital( title, process_name );

	sprintf(	sub_title,
			"%s/%s %s/%d",
			project,
			collection_name,
			location,
			site_number );

	format_initial_capital( sub_title, sub_title );

} /* get_title_and_sub_title() */

void load_field_sheet_scan(
				char *application_name,
				char *project,
				char *collection_name,
				char *location,
				int site_number,
				char *source_filename_directory_session )
{
	char *full_filename_directory;
	char *scan_filename;
	char tmp_buffer[ 256 ];
	char sys_string[ 1024 ];

	strcpy( tmp_buffer, source_filename_directory_session );

	full_filename_directory =
		right_trim_session(
			tmp_buffer );

	scan_filename = basename_get_filename( full_filename_directory );

	sprintf( sys_string,
		 "cp %s %s/%s",
		 source_filename_directory_session,
		 DESTINATION_DIRECTORY,
		 scan_filename );

	system( sys_string );

	sprintf( sys_string,
		 "chmod g+w %s/%s",
		 DESTINATION_DIRECTORY,
		 scan_filename );

	system( sys_string );

	update_sampling_point(	application_name,
				project,
				collection_name,
				location,
				site_number,
				scan_filename );

} /* load_field_sheet_scan() */

void update_sampling_point(	char *application_name,
				char *project,
				char *collection_name,
				char *location,
				int site_number,
				char *scan_filename )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *key_field_list;

	table_name = get_table_name( application_name, "sampling_point" );
	key_field_list = "project,collection_name,location,site_number";

	sprintf( sys_string,
		 "echo \"%s^%s^%s^%d^%s^%s/%s\"		|"
		 "update_statement.e	table_name=%s	 "
		 "			key=%s		 "
		 "			carrot=y	|"
		 "sql.e					 ",
		 project,
		 collection_name,
		 location,
		 site_number,
		 ATTRIBUTE_NAME,
		 RELATIVE_DIRECTORY,
		 scan_filename,
		 table_name,
		 key_field_list );

	system( sys_string );

} /* update_sampling_point() */

