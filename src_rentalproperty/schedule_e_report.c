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
#define ROWS_BETWEEN_HEADING	10

/* Prototypes */
/* ---------- */
HTML_TABLE *tax_form_report_get_html_table(
			char *title,
			char *sub_title,
			LIST *rental_property_street_address_list );

void tax_form_report_html_table(
			char *title,
			char *sub_title,
			LIST *tax_form_line_rental_list,
			LIST *rental_property_street_address_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	int tax_year;
	char *output_medium;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
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

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s process tax_year output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	tax_year = atoi( argv[ 2 ] );
	output_medium = argv[ 3 ];

	if ( !tax_year )
	{
		tax_year = atoi( pipe2string( "now.sh ymd | piece.e '-' 0" ) );
	}
	else
	if ( tax_year < 99 )
	{
		tax_year += CURRENT_CENTURY;
	}

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

	sprintf( begin_date_string, "%d-01-01", tax_year );
	sprintf( end_date_string, "%d-12-31", tax_year );

	appaserver_parameter_file = appaserver_parameter_file_new();

	sprintf(title,
		"%s",
		application_get_application_title(
			application_name ) );

	sprintf(sub_title,
	 	"%s %s Tax Year: %d",
	 	format_initial_capital( buffer, process_name ),
		SCHEDULE_E,
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

	if ( !tax || !tax->tax_input.tax_form->tax_form )
	{
		printf(
		"<h3>An internal error occurred. Check log.</h3>\n" );
		document_close();
		exit( 0 );
	}

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

	tax->tax_output_rental.tax_form_line_rental_list =
		tax_get_tax_form_line_rental_list(
			tax->tax_process.tax_form_line_list,
			tax->tax_input.rental_property_street_address_list );

	if ( !list_length(
		tax->tax_output_rental.tax_form_line_rental_list ) )
	{
		printf(
	     "<h3>No rental property transactions during this period.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		tax_form_report_html_table(
			title,
			sub_title,
			tax->tax_output_rental.tax_form_line_rental_list,
			tax->tax_input.rental_property_street_address_list );
	}

	document_close();

	return 0;

} /* main() */

void tax_form_report_html_table(
			char *title,
			char *sub_title,
			LIST *tax_form_line_rental_list,
			LIST *rental_property_street_address_list )
{
	TAX_FORM_LINE_RENTAL *tax_form_line_rental;
	TAX_OUTPUT_RENTAL_PROPERTY *rental_property;
	int count = 0;
	HTML_TABLE *html_table;

	html_table =
		tax_form_report_get_html_table(
			title,
			sub_title,
			rental_property_street_address_list );

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->justify_list );

	if ( !list_rewind( tax_form_line_rental_list ) )
	{
		printf(
"<h3>ERROR: there are no tax form lines for this tax form.</h3>\n" );
		html_table_close();
		document_close();
		exit( 1 );
	}

	do {
		tax_form_line_rental = list_get( tax_form_line_rental_list );

		if ( !list_length(
			tax_form_line_rental->
				rental_property_list ) )
		{
			continue;
		}

		if ( ++count == ROWS_BETWEEN_HEADING )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->
				number_left_justified_columns,
				html_table->
				number_right_justified_columns,
				html_table->justify_list );
			count = 0;
		}

		html_table_set_data(
			html_table->data_list,
			strdup( tax_form_line_rental->tax_form_line ) );

		html_table_set_data(
			html_table->data_list,
			strdup( tax_form_line_rental->tax_form_description ) );

		list_rewind( tax_form_line_rental->rental_property_list );

		do {
			rental_property =
				list_get_pointer(
					tax_form_line_rental->
						rental_property_list );

			html_table_set_data(
				html_table->data_list,
				strdup( timlib_place_commas_in_money(
					     rental_property->
						tax_form_line_total ) ) );

		} while( list_next(
				tax_form_line_rental->
					rental_property_list ) );

		html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

			list_free( html_table->data_list );
			html_table->data_list = list_new();

	} while( list_next( tax_form_line_rental_list ) );

	html_table_close();

} /* tax_form_report_html_table() */

HTML_TABLE *tax_form_report_get_html_table(
				char *title,
				char *sub_title,
				LIST *rental_property_street_address_list )
{
	HTML_TABLE *html_table;
	LIST *heading_list;
	char *rental_property_street_address;

	html_table = new_html_table(
			title,
			strdup( sub_title ) );

	if ( !list_rewind( rental_property_street_address_list ) )
		return html_table;

	heading_list = list_new();
	list_append_string( heading_list, "tax_form_line" );
	list_append_string( heading_list, "tax_form_description" );

	do {
		rental_property_street_address =
			list_get_pointer(
				rental_property_street_address_list );

		list_append_string(
			heading_list,
			rental_property_street_address );

	} while( list_next( rental_property_street_address_list ) );

	html_table_set_heading_list( html_table, heading_list );
	html_table->number_left_justified_columns = 2;

	html_table->number_right_justified_columns =
		list_length( rental_property_street_address_list );

	return html_table;

} /* tax_form_report_get_html_table() */
