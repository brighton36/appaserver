/* ---------------------------------------------------	*/
/* src_creel/load_sport_fishing_trips.c			*/
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
#include "list.h"
#include "environ.h"
#include "process.h"
#include "dictionary.h"
#include "application.h"
#include "session.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char *input_filename;
	char really_yn;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char sys_string[ 1024 ];

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process login_name filename really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	login_name = argv[ 3 ];
	input_filename = argv[ 4 ];
	really_yn = *argv[ 5 ];

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
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h2>Load Sport Fishing Trips\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*input_filename
	||   strcmp( input_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"delete_sport_creel_census %s %s",
		 	application_name,
		 	input_filename );
		fflush( stdout );
		system( sys_string );
		fflush( stdout );
	}

	sprintf( sys_string,
		 "insert_sport_creel_census %s %s %c",
		 application_name,
		 input_filename,
		 really_yn );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	sprintf( sys_string,
		 "insert_sport_fishing_trips %s '%s' '%s' %c",
		 application_name,
		 login_name,
		 input_filename,
		 really_yn );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	sprintf( sys_string,
		 "insert_sport_catches %s %s %c",
		 application_name,
		 input_filename,
		 really_yn );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( really_yn == 'y' )
		printf( "<p>Process complete\n" );
	else
		printf( "<p>Process not executed\n" );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	return 0;

} /* main() */

