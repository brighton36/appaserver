/* ---------------------------------------------------	*/
/* src_appaserver/view_diagrams.c			*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver.h"
#include "folder.h"
#include "form.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "application.h"
#include "document.h"
#include "session.h"
#include "environ.h"
#include "piece.h"
#include "basename.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"view_diagrams"
#define SYS_STRING		\
"ls -1 *.xls *.jpg *.doc *.pdf *.png *.txt *.ppt *.xml 2>/dev/null | sort"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char current_directory[ 256 ];
	DOCUMENT *document;
	LIST *file_list;
	char *file_to_process;
	char buffer[ 512 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *login_name = "";
	char *database_string = {0};
	char base_name[ 128 ];
				
	if ( argc < 2 )
	{
		fprintf( stderr,
	"Usage: %s application [ignored] [login_name] [ignored]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	/* session = argv[ 2 ]; */

	if ( argc >= 4 ) login_name = argv[ 3 ];

	/* role_name = argv[ 4 ]; */
 
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

	appaserver_parameter_file = appaserver_parameter_file_new();
	add_pwd_to_path();

	sprintf( current_directory,
		 "%s/%s", 
		 appaserver_parameter_file->appaserver_mount_point,
		 application_get_first_relative_source_directory(
					application_name ) );

	if ( chdir( current_directory ) == -1 )
	{
		fprintf( stderr, 
			 "Cannot change directory to %s\n",
			 current_directory );
		exit( 1 );
	}

	file_list = pipe2list( SYS_STRING );

	document = document_new( "", application_name );

	if ( !*login_name
	||   !appaserver_get_frameset_menu_horizontal(
					application_name,
					login_name ) )
	{
		document_set_output_content_type( document );
	}

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			"diagram_list.css",
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	if ( !list_rewind( file_list ) )
	{
		printf( "<p>ERROR: no documents found\n" );
		document_close();
		exit( 1 );
	}

	printf( "<h1>Documentation List</h1>\n" );
	printf( "<ul>\n" );

	do {
		file_to_process = list_get_string( file_list );

		strcpy( base_name,
			basename_get_base_name(
				file_to_process, 1 /* strip_extension */ ) );

/*
		printf(
"<li><a class=diagram target=\"%s\" href=\"/appaserver/%s/%s/%s\">%s</a>\n",
			base_name,
			application_name,
		 	application_get_first_relative_source_directory(
				application_name ),
		 	file_to_process,
			format_initial_capital( buffer, file_to_process ) );
*/

		printf(
"<li><a class=diagram target=\"%s\" href=\"/appaserver/%s/%s\">%s</a>\n",
			base_name,
		 	application_get_first_relative_source_directory(
				application_name ),
		 	file_to_process,
			format_initial_capital( buffer, file_to_process ) );
	} while( list_next( file_list ) );

	printf( "</ul>\n" );

	if ( *login_name ) form_output_back_to_top_button();

	document_close();

	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

