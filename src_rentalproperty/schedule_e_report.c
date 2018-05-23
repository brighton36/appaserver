/* ----------------------------------------------------------------	*/
/* src_rentalproperty/schedule_e_report.c				*/
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
#include "rental.h"
#include "tax.h"

/* Constants */
/* --------- */
#define CURRENT_CENTURY		2000

/* Prototypes */
/* ---------- */
boolean no_rental_property_transactions(
				char *account_name,
				LIST *rental_property_string_list,
				DICTIONARY *rental_ledger_summary_dictionary );

DICTIONARY *get_rental_ledger_summary_dictionary(
					int tax_year );

char *get_dictionary_key(		char *rental_property_street_address,
					char *account_name );

void output_rental_property_summary(
				char *account_name,
				int number_left_justified_columns,
				int number_right_justified_columns,
				boolean background_shaded,
				LIST *justify_list,
				LIST *rental_property_string_list,
				DICTIONARY *rental_ledger_summary_dictionary,
				double balance );

void output_journal_ledger_balance(	LIST *data_list,
					char *element_name,
					char *subclassification_name,
					char *account_name,
					double balance,
					int number_left_justified_columns,
					int number_right_justified_columns,
					boolean background_shaded,
					LIST *justify_list,
					boolean accumulate_debit );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	HTML_TABLE *html_table;
	char title[ 256 ];
	char sub_title[ 256 ];
	char buffer[ 128 ];
	LIST *heading_list;
	LIST *element_list;
	LEDGER_ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	int tax_year;
	char as_of_date[ 16 ];
	LIST *filter_element_name_list;
	LIST *rental_property_string_list;
	DICTIONARY *rental_ledger_summary_dictionary;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s ignored process tax_year\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	tax_year = atoi( argv[ 3 ] );

	if ( !tax_year )
	{
		tax_year = atoi( pipe2string( "now.sh ymd | piece.e '-' 0" ) );
	}
	else
	if ( tax_year < 99 )
	{
		tax_year += CURRENT_CENTURY;
	}

	rental_property_string_list =
		rental_get_rental_property_string_list(
			application_name,
			tax_year );

	rental_ledger_summary_dictionary =
		get_rental_ledger_summary_dictionary(
			tax_year );

	sprintf( as_of_date, "%d-12-31", tax_year );

	appaserver_parameter_file = new_appaserver_parameter_file();

	sprintf(title,
		"%s",
		application_get_application_title( application_name ) );

	sprintf( sub_title,
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

	filter_element_name_list = list_new();
	list_append_pointer( filter_element_name_list, "revenue" );
	list_append_pointer( filter_element_name_list, "expense" );

	element_list =
		ledger_get_element_list(
			application_name,
			filter_element_name_list,
			(char *)0 /* fund_name */,
			as_of_date,
			0 /* not omit_subclassification */ );

	heading_list = list_new();
	list_append_string( heading_list, "Element" );
	list_append_string( heading_list, "Subclassification" );
	list_append_string( heading_list, "Account" );
	list_append_string( heading_list, "Debit" );
	list_append_string( heading_list, "Credit" );
	list_append_string( heading_list, "Property Amount" );
	
	html_table = new_html_table(
			title,
			sub_title );

	html_table->number_left_justified_columns = 3;
	html_table->number_right_justified_columns = 3;
	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	if ( !list_rewind( element_list ) )
	{
		printf(
		"<p>ERROR: there are no elements for this statement.\n" );
		document_close();
		exit( 1 );
	}

	do {
		element = list_get_pointer( element_list );

		if ( !list_rewind( element->subclassification_list ) )
			continue;

		do {
			subclassification =
				list_get_pointer(
					element->
					   subclassification_list );

			if ( !list_rewind( subclassification->account_list ) )
				continue;

			do {
				account = 
					list_get_pointer(
						subclassification->
							account_list );

				if ( !account->latest_ledger
				||   !account->latest_ledger->balance )
					continue;

				output_journal_ledger_balance(
					html_table->data_list,
					element->element_name,
					subclassification->
						subclassification_name,
					account->account_name,
					account->latest_ledger->balance,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list,
					element->accumulate_debit );

				list_free_container( html_table->data_list );
				html_table->data_list = list_new();

				output_rental_property_summary(
					account->account_name,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list,
					rental_property_string_list,
					rental_ledger_summary_dictionary,
					account->latest_ledger->balance );

				subclassification->
					subclassification_name =
						(char *)0;

				element->element_name = (char *)0;

			} while( list_next( subclassification->account_list ) );

		} while( list_next( element->subclassification_list ) );

	} while( list_next( element_list ) );

	html_table_close();
	document_close();

	return 0;

} /* main() */

void output_journal_ledger_balance(
			LIST *data_list,
			char *element_name,
			char *subclassification_name,
			char *account_name,
			double balance,
			int number_left_justified_columns,
			int number_right_justified_columns,
			boolean background_shaded,
			LIST *justify_list,
			boolean accumulate_debit )
{
	char element_title[ 128 ];
	char subclassification_title[ 128 ];
	char account_title[ 128 ];
	char *debit_string;
	char *credit_string;

	if ( element_name && *element_name )
	{
		format_initial_capital(
			element_title,
			element_name );

		html_table_set_data(
			data_list,
			element_title );
	}
	else
		html_table_set_data( data_list, "" );

	if ( subclassification_name && *subclassification_name )
	{
		format_initial_capital(
			subclassification_title,
			subclassification_name );

		html_table_set_data(
			data_list,
			subclassification_title );
	}
	else
		html_table_set_data( data_list, "" );

	format_initial_capital(
		account_title,
		account_name );

	html_table_set_data(
		data_list,
		account_title );

	if ( accumulate_debit )
	{
		debit_string = place_commas_in_money( balance );
	}
	else
		debit_string = "";

	html_table_set_data(	data_list,
				strdup( debit_string ) );

	if ( !accumulate_debit )
	{
		credit_string = place_commas_in_money( balance );
	}
	else
		credit_string = "";

	html_table_set_data(	data_list,
				strdup( credit_string ) );

	html_table_output_data(
		data_list,
		number_left_justified_columns,
		number_right_justified_columns,
		background_shaded,
		justify_list );

} /* output_journal_ledger_balance() */

DICTIONARY *get_rental_ledger_summary_dictionary(
					int tax_year )
{
	DICTIONARY *rental_ledger_summary_dictionary;
	char sys_string[ 128 ];
	char input_buffer[ 512 ];
	FILE *input_pipe;
	char rental_property_street_address[ 128 ];
	char account_name[ 128 ];
	char property_amount[ 128 ];
	char *key;

	rental_ledger_summary_dictionary = dictionary_medium_new();

	sprintf( sys_string,
		 "select_rental_ledger_summary.sh %d",
		 tax_year );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( character_count( '^', input_buffer ) != 2 )
		{
			pclose( input_pipe );

			fprintf( stderr,
				 "ERROR in %s/%s()/%d: cannot parse = (%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );

			exit( 1 );
		}

		piece(	rental_property_street_address,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	account_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	property_amount,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		key = get_dictionary_key(
			rental_property_street_address,
			account_name );

		dictionary_set_pointer(
			rental_ledger_summary_dictionary,
			strdup( key ),
			strdup( property_amount ) );
	}

	pclose( input_pipe );

	return rental_ledger_summary_dictionary;

} /* get_rental_ledger_summary_dictionary() */

boolean no_rental_property_transactions(
				char *account_name,
				LIST *rental_property_string_list,
				DICTIONARY *rental_ledger_summary_dictionary )
{
	char *rental_property_street_address;
	char *key;

	if ( !list_rewind( rental_property_string_list ) )
		return 1;

	do {
		rental_property_street_address =
			list_get(
				rental_property_string_list );

		key = get_dictionary_key(
			rental_property_street_address,
			account_name );

		if ( dictionary_exists_key(
			rental_ledger_summary_dictionary,
			key ) )
		{
			return 0;
		}

	} while( list_next( rental_property_string_list ) );

	return 1;

} /* no_rental_property_transactions() */


char *get_dictionary_key(	char *rental_property_street_address,
				char *account_name )
{
	static char key[ 256 ];

	sprintf( key,
		 "%s^%s",
		 rental_property_street_address,
		 account_name );

	return key;

} /* get_dictionary_key() */

void output_rental_property_summary(
				char *account_name,
				int number_left_justified_columns,
				int number_right_justified_columns,
				boolean background_shaded,
				LIST *justify_list,
				LIST *rental_property_string_list,
				DICTIONARY *rental_ledger_summary_dictionary,
				double balance )
{
	char property_amount_string[ 16 ];
	char *property_amount_pointer;
	LIST *data_list;
	char *key;
	char *rental_property;
	int length;
	boolean no_transactions;

	if ( !list_length( rental_property_string_list ) ) return;

	*property_amount_string = '\0';

	no_transactions =
		no_rental_property_transactions(
			account_name,
			rental_property_string_list,
			rental_ledger_summary_dictionary );

	if ( no_transactions )
	{
		length = list_length( rental_property_string_list );

		sprintf( property_amount_string,
			 "%.2lf",
			 balance / (double)length );
	}

	list_rewind( rental_property_string_list );

	do {
		rental_property = list_get( rental_property_string_list );

		if ( !no_transactions )
		{
			key = get_dictionary_key(
				rental_property,
				account_name );

			property_amount_pointer =
				dictionary_safe_fetch(
					rental_ledger_summary_dictionary,
					key );

			strcpy(	property_amount_string,
				property_amount_pointer );
		}

		data_list = list_new();
		html_table_set_data( data_list, "" );
		html_table_set_data( data_list, "" );
		html_table_set_data( data_list, rental_property );
		html_table_set_data( data_list, "" );
		html_table_set_data( data_list, "" );
		html_table_set_data( data_list, property_amount_string );

		html_table_output_data(
			data_list,
			number_left_justified_columns,
			number_right_justified_columns,
			background_shaded,
			justify_list );

		list_free_container( data_list );

	} while( list_next( rental_property_string_list ) );

} /* output_rental_property_summary() */

