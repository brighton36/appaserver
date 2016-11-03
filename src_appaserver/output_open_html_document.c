/* output_open_html_document.c						*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	char *application_name, *session, *title;
	char *content_type_yn;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

/*
	output_starting_argv_stderr( argc, argv );
*/

	if ( argc != 5 )
	{
		fprintf( stderr, 
	"Usage: %s application session title content_type_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	title = argv[ 3 ];
	content_type_yn = argv[ 4 ];

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	document = document_new( title, application_name );

	document_output_heading(
			document->application_name,
			document->title,
			(*content_type_yn == 'y' ),
			document->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(	document->application_name,
				document->onload_control_string );

	return 0;
} /* main() */

