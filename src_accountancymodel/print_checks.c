/* ----------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/print_checks.c	*/
/* ----------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "date.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "ledger.h"
#include "entity.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *full_name_list_string;
	char *street_address_list_string;
	char *account_list_string;
	int starting_check_number;
	double single_check_amount;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;
	char title[ 128 ];
	char *database_string = {0};

	if ( argc != 8 )
	{
		fprintf( stderr,
		"Usage: %s application process full_name[,full_name] street_address[,street_address] account[,account] starting_check_number single_check_amount\n",
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
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	process_name = argv[ 2 ];
	full_name_list_string = argv[ 3 ];
	street_address_list_string = argv[ 4 ];
	account_list_string = argv[ 5 ];
	starting_check_number = atoi( argv[ 6 ] );
	single_check_amount = atof( argv[ 7 ] );

	appaserver_parameter_file = appaserver_parameter_file_new();

	format_initial_capital( title, process_name ),

	document = document_new( title, application_name );
	document->output_content_type = 1;

	document_output_head_stream(
			stdout,
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */,
			1 /* with close_head */ );

	printf( "<h1>%s</h1>\n", title );
	fflush( stdout );

	printf( "<p>%s\n", full_name_list_string );
	printf( "<p>%s\n", street_address_list_string );
	printf( "<p>%s\n", account_list_string );
	printf( "<p>%d\n", starting_check_number );
	printf( "<p>%.2lf\n", single_check_amount );

	document_close();

	return 0;

} /* main() */

