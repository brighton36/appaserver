/* --------------------------------------------------- 	*/
/* rename_table.c				       	*/
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
#include "environ.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "folder.h"
#include "attribute.h"
#include "application.h"
#include "appaserver_parameter_file.h"

/* Constants */
/* --------- */
/*
#define SESSION_PROCESS_NAME	"rename_table"
*/

/* Prototypes */
/* ---------- */
char *get_sys_string( char *old_table_name, char *new_table_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *old_folder_name;
	char *new_folder_name;
	char parsed_new_folder_name[ 128 ];
	char old_table_name[ 128 ];
	char new_table_name[ 128 ];
	char *really_yn;
	char *sys_string;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	if ( ! ( application_name =
			environ_get_environment(
				APPASERVER_DATABASE_ENVIRONMENT_VARIABLE ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot get environment of %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE );
		exit( 1 );
	}

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s ignored ignored ignored ignored old_folder new_folder really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	old_folder_name = argv[ 5 ];
	new_folder_name = argv[ 6 ];
	really_yn = argv[ 7 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	piece_last(	parsed_new_folder_name, 
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			new_folder_name );

	strcpy( old_table_name, 
		get_table_name( application_name, old_folder_name ) );

	strcpy( new_table_name, 
		get_table_name( application_name, parsed_new_folder_name ) );

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

	sys_string = get_sys_string( old_table_name, new_table_name );

	printf( "<BR><p>%s\n", sys_string );

	if ( toupper( *really_yn ) == 'Y' )
	{
		fflush( stdout );
		system( sys_string );
		printf( "<BR><h3>Process complete</h3>\n" );
	}
		
	document_close();
	exit( 0 );
} /* main() */


char *get_sys_string( char *old_table_name, char *new_table_name )
{
	char buffer[ 1024 ];

	sprintf( buffer,
"echo \"alter table %s rename as %s;\" | sql.e 2>&1 | html_paragraph_wrapper",
		 old_table_name,
		 new_table_name );

	return strdup( buffer );
} /* get_sys_string() */

