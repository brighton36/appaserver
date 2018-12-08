/* ---------------------------------------------------	*/
/* src_hydrology/load_cr10_single.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

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
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "basename.h"
#include "application.h"
#include "appaserver_link_file.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define PROCESS_NAME			"load_cr10_single_file"
#define FILENAME_STEM			"load_cr10"
#define PROMPT	"&lt;Left Click&gt; to view bad records or &lt;Right Click&gt; to save."

/* Prototypes */
/* ---------- */
void load_cr10_file(
			char *application_name,
			char *station,
			char *input_filename,
			char really_yn,
			char *document_root_directory,
			char *with_shift_left_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char really_yn;
	char *input_filename;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *station;
	char *database_string = {0};
	char *with_shift_left_yn = "";

	if ( argc < 8 )
	{
		fprintf( stderr, 
"Usage: %s application ignored ignored ignored filename station really_yn [with_shift_left]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	/* session = argv[ 2 ]; */
	/* login_name = argv[ 3 ]; */
	/* role_name = argv[ 4 ]; */
	input_filename = argv[ 5 ];
	station = argv[ 6 ];
	really_yn = *argv[ 7 ];

	if ( argc == 9 ) with_shift_left_yn = argv[ 8 ];

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

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	load_cr10_file(
			application_name,
			station,
			input_filename,
			really_yn,
			appaserver_parameter_file->
				document_root,
			with_shift_left_yn );

	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

void load_cr10_file(
			char *application_name,
			char *station,
			char *input_filename,
			char really_yn,
			char *document_root_directory,
			char *with_shift_left_yn )
{
	char *original_directory;
	char sys_string[ 1024 ];
	char process_filename[ 1024 ];
	char process_directory[ 1024 ];
	char *ftp_filename;
	int process_id = getpid();
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
			process_id,
			(char *)0 /* session */,
			"DAT" );

	original_directory = basename_get_directory( input_filename );

	if ( !original_directory || !*original_directory )
	{
		document_quick_output_body(
			application_name,
			(char *)0 /* appaserver_mount_point */ );

		printf(
			"<h3>ERROR: could not locate file.\n" );
		document_close();
		exit( 0 );
	}

	if ( really_yn == 'y' )
	{
		strcpy(	process_filename, 
			appaserver_link_get_abbreviated_output_filename(
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension ) );

		strcpy(	process_directory,
			appaserver_link_get_source_directory(
				document_root_directory,
				application_name ) );

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

		sprintf(sys_string,
			"cp %s %s/%s",
			input_filename,
			process_directory,
			process_filename );
	
		system( sys_string );
	}
	else
	{
		strcpy( process_directory, original_directory );
		strcpy( process_filename,
			basename_get_filename( input_filename ) );
	}

	sprintf(sys_string,
"load_cr10 %s %s %s %s date_offset_comma_list y email_address %c '%s'",
		application_name,
		process_directory,
		process_filename,
		station,
		really_yn,
		with_shift_left_yn );

	system( sys_string );

	if ( really_yn == 'y' )
	{
		char process_filespec[ 512 ];

		sprintf( process_filespec,
			 "%s/%s",
			 process_directory,
			 process_filename );

		if ( timlib_file_populated( process_filespec ) )
		{
			printf( "<br>" );
			appaserver_library_output_ftp_prompt(
					ftp_filename, 
					PROMPT,
					(char *)0 /* target */,
					(char *)0 /* application_type */ );
		}
	}

} /* load_cr10_file() */

