/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/shef_upload.c	       	*/
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
#include "appaserver_error.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define STATION_FILENAME_STEM	"shef_upload_bad_station"
#define SHEF_FILENAME_STEM	"shef_upload_bad_shef"

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
void shef_upload(	char *station_bad_file, 
			char *shef_bad_file,
			char *application_name,
			char *shef_filename,
			boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *shef_filename;
	boolean execute;
	DOCUMENT *document;
	char *station_bad_file;
	char *shef_bad_file;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *session;
	APPASERVER_LINK_FILE *appaserver_link_file;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
		"Usage: %s process_name session shef_filename execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	session = argv[ 2 ];
	shef_filename = argv[ 3 ];
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
			0 /* process_id */,
			session,
			"dat" /* extension */ );

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

	shef_upload(	station_bad_file, 
			shef_bad_file,
			application_name,
			shef_filename,
			execute );

	if ( execute )
	{
		printf( "<p>Process complete.\n" );

		process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( "<p>Process NOT executed.\n" );
	}

	document_close();
	return 0;

} /* main() */

void shef_upload(	char *station_bad_file, 
			char *shef_bad_file,
			char *application_name,
			char *shef_filename,
			boolean execute )

{
	char insert_process[ 512 ];
	char prune_process[ 512 ];
	char remove_process[ 512 ];
	char display_error_process[ 512 ];
	char sys_string[ 4096 ];

	strcpy( prune_process, "echo > /dev/null" );
	strcpy( remove_process, "echo > /dev/null" );

	sprintf(display_error_process,
		"echo \"<br><th>Bad records:</th>\"		;"
		"cat %s | html_paragraph_wrapper		;"
		"cat %s | html_paragraph_wrapper		 ",
		station_bad_file,
		shef_bad_file );

	printf( "<p>Processing: %s\n", shef_filename );
	fflush( stdout );

	sprintf( insert_process,
	"measurement_insert %s shef %c 2>&1 | html_paragraph_wrapper",
			 application_name,
			 (execute) ? 'y' : 'n' );

	sprintf( sys_string,
	"cat %s					      	     	     |"
	"station_alias_to_station %s 1 \'/\' 2>%s 		     |"
	"shef_to_comma_delimited %s 2>%s		  	     |"
	"%s 					      		     ;"
	"%s							     ;"
	"%s							     ;"
	"%s 							      ",
			 shef_filename,
			 application_name,
			 station_bad_file,
			 application_name,
			 shef_bad_file,
			 insert_process,
			 prune_process,
			 display_error_process,
			 remove_process );

	if ( system( sys_string ) ) {};

} /* shef_upload() */
