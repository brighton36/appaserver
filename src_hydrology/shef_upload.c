/* --------------------------------------------------- 	*/
/* src_hydrology/shef_upload.c			       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "basename.h"
#include "piece.h"
#include "list.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "process.h"
#include "session.h"
#include "application.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"shef_upload"
#define STATION_FILENAME_STEM	"shef_upload_bad_station"
#define SHEF_FILENAME_STEM	"shef_upload_bad_shef"
/*
#define STATION_BAD_FILE_TEMPLATE "/tmp/shef_upload_bad_station_%d.dat"
#define SHEF_BAD_FILE_TEMPLATE	  "/tmp/shef_upload_bad_shef_%d.dat"
*/

/* Sample
--------------------------------------------------------------------------------
.E/3273/STG/20000805/0900/DIH1/  6.90/  6.90/  6.90/  6.90/(M)/(M)/(M)/(M)/
.E/3273/STG/20000805/1700/DIH1/  6.90/  6.90/  6.90/  6.90/  6.90/  6.89/  6.89/
.E/3273/STG/20000806/0000/DIH1/  6.89/  6.89/(M)/(M)/(M)/(M)/(M)/(M)/(M)/  6.88/
.E/3273/STG/20000806/1000/DIH1/  6.88/  6.88/  6.98/  6.98/  6.98/  6.98/  6.98/
.E/3273/STG/20000806/1700/DIH1/  6.98/  7.02/  7.02/  7.02/  7.02/  7.02/  7.02/
.E/3273/STG/20000807/0000/DIH1/  7.02/  7.02/(M)/(M)/(M)/(M)/  7.01/  7.01/
.E/3273/STG/20000807/0800/DIH1/  7.01/
.E/ANGELS/HEAD/20000805/0900/DIH1/  6.84/  6.84/  6.84/  6.87/  6.87/  6.87/(M)/
.E/ANGELS/HEAD/20000805/1600/DIH1/(M)/  6.85/  6.85/  6.85/  6.84/  6.84/  6.84/
.E/ANGELS/HEAD/20000805/2300/DIH1/  6.84/  6.84/  6.82/  6.82/  6.82/  6.82/
#  ^      ^    ^   ^ ^  ^
#  |      |    |   | |  |
#  |      |    |   | |  hhmm
#  |      |    |   | day
#  |      |    |   month
#  |      |    year
#  |      shef code
#  station
--------------------------------------------------------------------------------
*/

/* Prototypes */
/* ---------- */
void shef_upload(	LIST *file_list, 
			char *station_bad_file, 
			char *shef_bad_file,
			int with_file_trim_yn,
			char *email_address,
			int really_yn,
			char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *shef_file_specification;
	char *starting_filename;
	char with_file_trim_yn;
	char really_yn;
	DOCUMENT *document;
	char *station_bad_file;
	char *shef_bad_file;
	LIST *file_list;
	char *email_address;
	char sys_string[ 4096 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *session;
	char *login_name;
	char *role_name;
	char *database_string = {0};
	APPASERVER_LINK_FILE *appaserver_link_file;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 10 )
	{
		fprintf(stderr,
"Usage: %s application session login_name role 'shef_file_specification' with_file_trim_yn email_address starting_filename really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	shef_file_specification = argv[ 5 ];
	with_file_trim_yn = *argv[ 6 ];
	email_address = argv[ 7 ];
	starting_filename = argv[ 8 ];
	really_yn = *argv[ 9 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

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
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->
				document_root,
			STATION_FILENAME_STEM,
			application_name,
			getpid(),
			(char *)0 /* session */,
			"dat" );

	station_bad_file =
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

	appaserver_link_file->filename_stem = SHEF_FILENAME_STEM;

	shef_bad_file =
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

	sprintf( sys_string, 
		 "echo %s | tr ' ' '\012'",
		 shef_file_specification );

	file_list = pipe2list( sys_string );

	if ( *starting_filename 
	&&   strcmp( starting_filename, "starting_filename" ) != 0 )
	{
		if ( !list_search_string( file_list, starting_filename ) )
		{
			fprintf( stderr, 
				 "ERROR: in %s: cannot find file = (%s)\n",
				 argv[ 0 ],
				 starting_filename );
			fprintf( stderr, "Trying using the full path\n" );
			exit( 1 );
		}
	}
	else
		list_rewind( file_list );

	shef_upload(	file_list, 
			station_bad_file, 
			shef_bad_file,
			with_file_trim_yn,
			email_address,
			really_yn,
			application_name );

	if ( *email_address && strcmp( email_address, "email_address" ) != 0 )
	{
		if ( really_yn == 'y' )
		{
			printf( 
		"<p>In progress. Check email for error messages.\n" );
			process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
		}
		else
		{
			printf( 
		"<p>Process NOT executed. Check email for SQL statements.\n" );
		}
	}
	else
	{
		if ( really_yn == 'y' )
		{
			printf( "<p>Process complete.\n" );
			process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
		}
		else
		{
			printf( "<p>Process NOT executed.\n" );
		}
	}

	document_close();
	exit( 0 );
} /* main() */

void shef_upload(	LIST *file_list, 
			char *station_bad_file, 
			char *shef_bad_file,
			int with_file_trim_yn,
			char *email_address,
			int really_yn,
			char *application_name )
{
	char *file;
	char insert_process[ 512 ];
	char prune_process[ 512 ];
	char remove_process[ 512 ];
	char display_error_process[ 512 ];
	char background_process[ 512 ];
	char sys_string[ 4096 ];

	sprintf(remove_process,
		"/bin/rm -f %s 2>/dev/null; /bin/rm -f %s 2>/dev/null",
		station_bad_file,
		shef_bad_file );

	sprintf(display_error_process,
		"echo \"<br><th>Bad records:</th>\"		;"
		"cat %s | html_paragraph_wrapper		;"
		"cat %s | html_paragraph_wrapper		 ",
		station_bad_file,
		shef_bad_file );

	/* -------------------------------------------------------------- */
	/* Note: don't rewind list because of the possibility of a search */
	/* -------------------------------------------------------------- */

	do {
		file = list_get_string( file_list );

		if ( *email_address 
		&&   strcmp( email_address, "email_address" ) != 0 )
		{
			sprintf( insert_process, 
"measurement_insert %s shef %c 2>&1 | mailx -s \"%s: Shef upload output\" %s",
		 		application_name,
				really_yn,
				basename_get_base_name( file, 0 ),
				email_address );

			strcpy( background_process, "&" );
		}
		else
		{
			printf( "<p>Processing: %s\n", file );
			fflush( stdout );

			sprintf( insert_process,
	"measurement_insert %s shef %c 2>&1 | html_paragraph_wrapper",
				 application_name,
				 really_yn );

			*background_process = '\0';
		}

		if ( really_yn == 'y' && with_file_trim_yn == 'y' )
		{
			sprintf( prune_process,
				 "> %s				;"
				 "cat %s > %s			;"
				 "cat %s >> %s			 ",
				 file,
				 station_bad_file, file,
				 shef_bad_file, file );
		}
		else
			strcpy( prune_process, "echo > /dev/null" );

		sprintf( sys_string,
	"(cat %s					      	     |"
	"station_alias_to_station %s 1 \'/\' 2>%s 		     |"
	"shef_to_comma_delimited %s 2>%s		  	     |"
	"%s 					      		     ;"
	"%s							     ;"
	"%s							     ;"
	"%s) %s							      ",
			 file,
			 application_name,
			 station_bad_file,
			 application_name,
			 shef_bad_file,
			 insert_process,
			 prune_process,
			 display_error_process,
			 remove_process,
			 background_process );

		system( sys_string );

		/* Try to slow down Solaris' pipelining */
		/* ------------------------------------ */
		sleep( 1 );

	} while( list_next( file_list ) );
} /* shef_upload() */
