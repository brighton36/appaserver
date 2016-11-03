/* html_invoice_dvr.c					   */
/* ======================================================= */
/* Test drive program for HTML_INVOICE ADT		   */
/* ======================================================= */
/* Freely available software: see Appaserver.org	   */
/* ======================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document.h"
#include "application.h"
#include "html_invoice.h"
#include "appaserver_parameter_file.h"

#define APPLICATION	"airfreight"
#define TITLE		"HL Cargo Handlers Invoice"
#define IMAGE_SOURCE	"<img src=\"/airfreight/hl_cargo_handlers.jpg\">"

int main( void )
{
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	HTML_INVOICE *i;

	appaserver_parameter_file = new_appaserver_parameter_file();

	document = document_new( TITLE, APPLICATION );

	document_output_head_stream(
			stdout,
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			(char *)0 /* relative_source_directory */,
			0 /* not with_dynarch_menu */,
			1 /* with_head_close */ );

	document_output_body_stream(
			stdout,
			document->application_name,
			document->onload_control_string );

	i = html_invoice_new(
				strdup( "September 22, 2006" ),
				strdup( IMAGE_SOURCE ),
				strdup( "6701 Lindbergh Dr." ),
				strdup( "" /* suite_number */ ),
				strdup( "Sacramento" ),
				strdup( "CA" ),
				strdup( "95837" ),
				strdup( "Airway Bill Number" ) );

	html_invoice_output_header( stdout );

	i->invoice_customer = html_invoice_customer_new(
					strdup( "000001 May 2006" ),
					strdup( "American Airlines, Inc." ),
					strdup( "6928 Airport Boulevard" ),
					strdup( "" ),
					strdup( "Sacramento" ),
					strdup( "CA" ),
					strdup( "95837" ) );

	i->invoice_customer->extension_total +=
		html_invoice_append_line_item(
					i->invoice_customer->
						invoice_line_item_list,
					strdup( "12345678" ),
					strdup( "Cargo Pounds In" ),
					3819,
					0.045 );

	i->invoice_customer->extension_total +=
		html_invoice_append_line_item(
					i->invoice_customer->
						invoice_line_item_list,
					strdup( "12345679" ),
					strdup( "Cargo Pounds Out" ),
					39500,
					0.045 );

	i->invoice_customer->extension_total +=
		html_invoice_append_line_item(
					i->invoice_customer->
						invoice_line_item_list,
					strdup( "12345680" ),
					strdup( "Trucked Pounds Out" ),
					15570,
					0.000 );

	i->invoice_customer->extension_total +=
		html_invoice_append_line_item(
					i->invoice_customer->
						invoice_line_item_list,
					strdup( "12345681" ),
					strdup( "Priority Parcels Out Count" ),
					56,
					3.000 );

	html_invoice_output_invoice_header(
					stdout,
					i->invoice_date,
					i->line_item_key_header,
					&i->invoice_company,
					i->invoice_customer,
					i->omit_money );

	html_invoice_output_invoice_line_items(
				stdout,
				i->invoice_customer->invoice_line_item_list,
				i->omit_money );

	html_invoice_output_invoice_footer(
				stdout,
				i->invoice_customer->extension_total,
				i->line_item_key_header );

	html_invoice_customer_free( i->invoice_customer );

	i->invoice_customer = html_invoice_customer_new(
					strdup( "000002 May 2006" ),
					strdup("Air Container Transport, Inc."),
					strdup( "270 Lawrence Ave." ),
					strdup( "" ),
					strdup( "S. San Francisco" ),
					strdup( "CA" ),
					strdup( "94080" ) );

	i->invoice_customer->extension_total +=
		html_invoice_append_line_item(
					i->invoice_customer->
						invoice_line_item_list,
					strdup( "12345682" ),
					strdup( "Cargo Pounds Out" ),
					15235,
					0.025 );

	html_invoice_output_invoice_header(
					stdout,
					i->invoice_date,
					i->line_item_key_header,
					&i->invoice_company,
					i->invoice_customer,
					i->omit_money );

	html_invoice_output_invoice_line_items(
				stdout,
				i->invoice_customer->invoice_line_item_list,
				i->omit_money );

	html_invoice_output_invoice_footer(
				stdout,
				i->invoice_customer->extension_total,
				i->line_item_key_header );

	html_invoice_customer_free( i->invoice_customer );
	html_invoice_company_free( &i->invoice_company );
	html_invoice_free( i );

	document_close_stream( stdout );
	return 0;
}

