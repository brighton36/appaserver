/* ---------------------------------------------------	*/
/* view_pdf_files.c					*/
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
#include "form.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "application.h"
#include "document.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char current_directory[ 256 ];
	char sys_string[ 512 ];
	DOCUMENT *document;
	LIST *file_list;
	char *file_to_process;
	char buffer[ 512 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
				
	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s application\n", argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
 
	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();
	add_pwd_to_path();

	sprintf( current_directory,
		 "%s/%s", 
		 appaserver_parameter_file->appaserver_mount_point,
		 application_name );

	if ( chdir( current_directory ) == -1 )
	{
		fprintf( stderr, 
			 "Cannot change directory to %s\n",
			 current_directory );
		exit( 1 );
	}

	sprintf( sys_string, 
		 "echo *.pdf | tr ' ' '\012' | grep -v '^\\*'" );
	file_list = pipe2list( sys_string );

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

	if ( !list_rewind( file_list ) )
	{
		printf( "<p>ERROR: no documents found\n" );
		document_close();
		exit( 1 );
	}

	printf( "<h1>Diagram list</h1>\n" );
	printf( "<ul>\n" );

	do {
		file_to_process = list_get_string( file_list );

		printf(
"<li><a class=external_link target=diagram_list href=\"%s:/%s/%s\">%s</a>\n",
			application_get_http_prefix( application_name ),
			application_name,
		 	file_to_process,
			format_initial_capital( buffer, file_to_process ) );

	} while( list_next( file_list ) );

	printf( "</ul>\n" );

	form_output_back_to_top_button();

	document_close();

	exit( 0 );
} /* main() */

