/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_rentalproperty/schedule_e_report.c		*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "environ.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "ledger.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "date.h"
#include "boolean.h"
#include "tax.h"

/* Constants */
/* --------- */
#define CURRENT_CENTURY		2000
#define SCHEDULE_E		"Schedule e"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int tax_year;
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	char title[ 128 ];
	char sub_title[ 128 ];
	char buffer[ 128 ];
	TAX *tax;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s process tax_year\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	tax_year = atoi( argv[ 2 ] );

	if ( !tax_year )
	{
		tax_year = atoi( pipe2string( "now.sh ymd | piece.e '-' 0" ) );
	}
	else
	if ( tax_year < 99 )
	{
		tax_year += CURRENT_CENTURY;
	}

	sprintf( begin_date_string, "%d-01-01", tax_year );
	sprintf( end_date_string, "%d-12-31", tax_year );

	appaserver_parameter_file = appaserver_parameter_file_new();

	sprintf(title,
		"%s",
		application_get_application_title(
			application_name ) );

	sprintf(sub_title,
	 	"%s Tax Year: %d",
	 	format_initial_capital( buffer, process_name ),
	 	tax_year );

	document = document_new( process_name, application_name );
	document->output_content_type = 1;

	document_output_heading(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(	document->application_name,
				document->onload_control_string );

	tax = tax_new(	application_name,
			(char *)0 /* fund_name */,
			begin_date_string,
			end_date_string,
			SCHEDULE_E );

	tax->tax_input.rental_property_street_address_list =
		tax_get_rental_property_string_list(
			application_name,
			begin_date_string,
			end_date_string );

	if ( !list_length(
		tax->
			tax_input.
			rental_property_street_address_list ) )
	{
		printf(
		"<h3>No rental properties owned during this period.</h3>\n" );
		document_close();
		exit( 0 );
	}

	document_close();

	return 0;

} /* main() */

