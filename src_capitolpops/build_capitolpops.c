/* ----------------------------------------------------	*/
/* $APPASERVER_HOME/src_capitolpops/build_capitolpops.c	*/
/* ----------------------------------------------------	*/
/* This creates the Capitolpops website. 		*/
/* ----------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

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

/* Constants */
/* --------- */
#define HOME_DIRECTORY_KEY	"home_directory"
#define BUILD_EXECUTABLE	"build.sh"

/* Prototypes */
/* ---------- */
void build_capitolpops( char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *database_string = {0};
	char title[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s application process_name\n",
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

	process_name = argv[ 2 ];

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

	build_capitolpops( application_name );

	printf( "<p>Process complete.\n" );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

void build_capitolpops( char *application_name )
{
	char *home_directory;
	int results;

	if ( ! ( home_directory =
			application_constants_quick_fetch(
				application_name,
				HOME_DIRECTORY_KEY ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get home directory from application constants: %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 HOME_DIRECTORY_KEY );
		printf(
"<p>An internal error occurred. See the appaserver error file.\n" );
		document_close();
		exit( 1 );
	}

	results = chdir( home_directory );

	if ( results != 0 )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot chdir to home directory: %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 home_directory );
		printf(
"<p>An internal error occurred. See the appaserver error file.\n" );
		document_close();
		exit( 1 );
	}

	system( BUILD_EXECUTABLE );

	if ( results != 0 )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: %s returned an error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 BUILD_EXECUTABLE );
		printf(
"<p>An internal error occurred. See the appaserver error file.\n" );
		document_close();
		exit( 1 );
	}

} /* build_capitolpops() */
