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
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "folder.h"
#include "attribute.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"

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
	char *session;
	char *login_name;
	char *role_name;
	char *database_string = {0};

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s application session login_name role old_folder new_folder really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	old_folder_name = argv[ 5 ];
	new_folder_name = argv[ 6 ];
	really_yn = argv[ 7 ];

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

	appaserver_parameter_file = new_appaserver_parameter_file();

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

