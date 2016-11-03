/* --------------------------------------------------- 	*/
/* src_appaserver/generic_load.c	      	 	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "document.h"
#include "attribute.h"
#include "application.h"
#include "appaserver.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "folder.h"
#include "session.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *session;
	char *role_name;
	char *login_name;
	char *process_name;
	char process_title_initial_capital[ 256 ];
	char buffer[ 256 ];
	char title[ 256 ];
	LIST *folder_name_list;
	char *folder_name;
	char *database_string = {0};

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s application session role process\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];

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

	login_name = session_get_login_name(
					application_name,
					session );

	format_initial_capital( process_title_initial_capital,
				process_name );

	document = document_new(process_title_initial_capital,
				application_name );
	if ( !appaserver_get_frameset_menu_horizontal(
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
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	sprintf( title,
		 "%s %s",
		 application_get_title_string( application_name ),
		 process_title_initial_capital );

	printf( "<h1>%s</h1>\n", title );

	folder_name_list =
		folder_get_insert_folder_name_list(
				application_name,
				login_name,
				role_name );

	printf( "<table border>\n" );
	if ( folder_name_list && list_rewind( folder_name_list ) )
	{
		do {
			folder_name = list_get_pointer( folder_name_list );
			printf( "<tr><td>" );
			printf(
"<a href=\"%s/post_generic_load?%s+%s+%s+%s+one\" target=%s>%s</a></td>\n",
				appaserver_parameter_file_get_cgi_directory(),
				timlib_get_parameter_application_name(
					application_name,
					database_string ),
				session,
				role_name,
				folder_name,
				PROMPT_FRAME,
				format_initial_capital(
					buffer,
					folder_name ) );
		} while( list_next( folder_name_list ) );
	}
	printf( "</table>\n" );

	document_close();

	return 0;

} /* main() */

