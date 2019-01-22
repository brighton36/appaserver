/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/transaction_balance_report.c		*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "environ.h"
#include "date.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "ledger.h"
#include "application_constants.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "date_convert.h"
#include "boolean.h"
#include "bank_upload.h"
#include "transaction_balance.h"

/* Constants */
/* --------- */
#define ROWS_BETWEEN_HEADING		10
#define PROMPT				"Press here to view report."

/* Prototypes */
/* ---------- */
void transaction_balance_report_anomaly(
			char *application_name,
			TRANSACTION_BALANCE_BLOCK *last_outbalance_block );

char *transaction_balance_report_get_duplicated_transaction_message(
					double transaction_amount,
					double bank_amount,
					double balance,
					double bank_running_balance );

double transaction_balance_report_get_duplicated_transaction_amount(
					char *message,
					char **duplicated_full_name,
					char **duplicated_transaction_date_time,
					double balance,
					double bank_running_balance );

TRANSACTION_BALANCE *transaction_balance_report_html_table(
					char *application_name,
					char *title,
					char *sub_title,
					char *begin_date_string,
					double cash_ending_balance );

/* Global variables */
/* ---------------- */
enum bank_upload_exception bank_upload_exception = {0};

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 256 ];
	char sub_title[ 256 ];
	char *output_medium;
	char *begin_date_string;
	double cash_ending_balance;
	TRANSACTION_BALANCE *transaction_balance;

	/* Exits if fails */
	/* -------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
	"Usage: %s process begin_date cash_ending_balance output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	begin_date_string = argv[ 2 ];
	cash_ending_balance = atof( argv[ 3 ] );
	output_medium = argv[ 4 ];

	if ( !*begin_date_string
	||   strcmp( begin_date_string, "begin_date" ) == 0 )
	{
		begin_date_string = "2000-01-01";
	}

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

	appaserver_parameter_file = appaserver_parameter_file_new();

	document = document_new( title, application_name );
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

	format_initial_capital( title, process_name );
	sprintf( sub_title, "Starting %s", begin_date_string );

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		transaction_balance =
			transaction_balance_report_html_table(
				application_name,
				title,
				sub_title,
				begin_date_string,
				cash_ending_balance );

		if ( list_length( transaction_balance->outbalance_block_list ) )
		{
			TRANSACTION_BALANCE_BLOCK *last_outbalance_block;

			last_outbalance_block =
				list_get_last_pointer( 
					transaction_balance->
						outbalance_block_list );

			transaction_balance_report_anomaly(
				application_name,
				last_outbalance_block );
		}
	}
	else
	{
		printf( "<h3>Error: invalid output_medium.</h3>\n" );
	}

	document_close();
	exit( 0 );

} /* main() */

TRANSACTION_BALANCE *transaction_balance_report_html_table(
			char *application_name,
			char *title,
			char *sub_title,
			char *begin_date_string,
			double cash_ending_balance )
{
	TRANSACTION_BALANCE *transaction_balance;
	TRANSACTION_BALANCE_BLOCK *block;
	HTML_TABLE *html_table;
	LIST *heading_list;
	LIST *merged_block_list;
	char buffer[ 128 ];
	int count = 0;

	if ( ! ( transaction_balance =
			transaction_balance_new(
				application_name,
				begin_date_string,
				cash_ending_balance ) ) )
	{
		printf( "<h3>Error occurred. Check log.</h3>\n" );
		document_close();
		exit( 1 );
	}

	if ( !list_length(
		transaction_balance->
			input.
			transaction_balance_row_list ) )
	{
		printf(
		"<h3>Warning: empty BANK_UPLOAD_TRANSACTION_BALANCE</h3>\n" );
		document_close();
		exit( 1 );
	}

	transaction_balance->inbalance_block_list =
		transaction_balance_get_inbalance_block_list(
			transaction_balance->
				input.
				transaction_balance_row_list );

	transaction_balance->outbalance_block_list =
		transaction_balance_get_outbalance_block_list(
			transaction_balance->
				input.
				transaction_balance_row_list );

	transaction_balance->merged_block_list =
	merged_block_list =
		transaction_balance_get_merged_block_list(
				transaction_balance->inbalance_block_list,
				transaction_balance->outbalance_block_list );

	heading_list = list_new();
	list_append_string( heading_list, "in/out" );
	list_append_string( heading_list, "begin_transaction" );
	list_append_string( heading_list, "full_name" );
	list_append_string( heading_list, "amount" );
	list_append_string( heading_list, "end_transaction" );
	list_append_string( heading_list, "full_name" );
	list_append_string( heading_list, "amount" );
	list_append_string( heading_list, "block_count" );

	html_table = new_html_table(
			title,
			strdup( sub_title ) );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 7;

	html_table_set_heading_list( html_table, heading_list );

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->justify_list );

	if ( !list_rewind( merged_block_list ) )
	{
		html_table_close();
		document_close();
		exit( 1 );
	}

	do {
		block = list_get( merged_block_list );

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
			(block->is_inbalance)
				? strdup( "In" )
				: strdup( "Out" ) );

		/* Begin block section */
		/* ------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( block->
					begin_transaction_balance->
					transaction_date_time ) );

		html_table_set_data(
			html_table->data_list,
			strdup( block->
					begin_transaction_balance->
					full_name ) );

		sprintf(buffer,
			"%.2lf",
			block->
				begin_transaction_balance->
				transaction_amount );

		list_append_pointer( html_table->data_list, strdup( buffer ) );

		/* End block section */
		/* ----------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( block->
					end_transaction_balance->
					transaction_date_time ) );

		html_table_set_data(
			html_table->data_list,
			strdup( block->
					end_transaction_balance->
					full_name ) );

		sprintf(buffer,
			"%.2lf",
			block->
				end_transaction_balance->
				transaction_amount );

		list_append_pointer( html_table->data_list, strdup( buffer ) );

		sprintf(buffer,
			"%d",
			block->block_count );

		list_append_pointer( html_table->data_list, strdup( buffer ) );

		/* Output */
		/* ------ */
		html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		list_free( html_table->data_list );
		html_table->data_list = list_new();

		html_table->background_shaded =
			1 - html_table->background_shaded;

	} while( list_next( merged_block_list ) );

	html_table_close();

	return transaction_balance;

} /* transaction_balance_report_html_table() */

void transaction_balance_report_anomaly(
			char *application_name,
			TRANSACTION_BALANCE_BLOCK *last_outbalance_block )
{
	char *anomaly_transaction_date_time;
	TRANSACTION_BALANCE_ROW *row;
	char *duplicated_transaction_message;

	if ( !last_outbalance_block )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty last_outbalance_block.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	anomaly_transaction_date_time =
		last_outbalance_block->
			begin_transaction_balance->
				transaction_date_time;

	row = transaction_balance_transaction_date_time_fetch(
			application_name,
			anomaly_transaction_date_time );

	if ( !row )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot fetch anomaly_transaction_date_time = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			anomaly_transaction_date_time );
		exit( 1 );
	}

	duplicated_transaction_message =
		transaction_balance_report_get_duplicated_transaction_message(
			row->transaction_amount,
			row->bank_amount,
			row->balance,
			row->bank_running_balance );

	if ( duplicated_transaction_message
	&&   *duplicated_transaction_message )
	{
		printf( "<h3>%s</h3>\n",
			duplicated_transaction_message );
	}

} /* transaction_balance_report_anomaly() */

char *transaction_balance_report_get_duplicated_transaction_message(
			double transaction_amount,
			double bank_amount,
			double balance,
			double bank_running_balance )
{
	double duplicated_transaction_amount;
	char *duplicated_full_name = {0};
	char *duplicated_transaction_date_time = {0};
	char message[ 1024 ];

	/* ------------------------------------------------------------- */
	/* BANK_UPLOAD_TRANSACTION_BALANCE.transaction_amount must equal */
	/* 0.0 - BANK_UPLOAD_TRANSACTION_BALANCE.bank_amount.		 */
	/* ------------------------------------------------------------- */
	if ( !timlib_dollar_virtually_same(
		transaction_amount,
		0.0 - bank_amount ) )
	{
		return (char *)0;
	}

	*message = '\0';

	duplicated_transaction_amount =
		transaction_balance_report_get_duplicated_transaction_amount(
			message,
			&duplicated_full_name,
			&duplicated_transaction_date_time,
			balance,
			bank_running_balance );

	if ( *message ) return strdup( message );

	if ( timlib_dollar_virtually_same(
		duplicated_transaction_amount,
		0.0 ) )
	{
		return (char *)0;
	}

	sprintf( message,
"Duplicated transaction: (%s/%s) amount = %.2lf.",
		duplicated_full_name,
		duplicated_transaction_date_time,
		duplicated_transaction_amount );

	return strdup( message );

} /* transaction_balance_report_get_duplicated_transaction_message() */

double transaction_balance_report_get_duplicated_transaction_amount(
			char *message,
			char **duplicated_full_name,
			char **duplicated_transaction_date_time,
			double balance,
			double bank_running_balance )
{
	double anomaly_balance_difference;
	double duplicated_transaction_amount;

	*duplicated_full_name = (char *)0;
	*duplicated_transaction_date_time = (char *)0;

	anomaly_balance_difference =
		transaction_balance_calculate_anomaly_balance_difference(
			balance,
			bank_running_balance );

	if ( timlib_dollar_virtually_same(
		anomaly_balance_difference,
		0.0 ) )
	{
		return 0.0;
	}

	duplicated_transaction_amount = 0.0 - anomaly_balance_difference;

	*duplicated_transaction_date_time =
		bank_upload_pending_amount_transaction(
			message,
			duplicated_full_name,
			duplicated_transaction_amount
				/* amount */ );

	/* It returned a warning message. */
	/* ------------------------------ */
	if ( *message ) return 0.0;

	/* It found a duplicated transaction to delete. */
	/* -------------------------------------------- */
	return duplicated_transaction_amount;

} /* transaction_balance_report_get_duplicated_transaction_amount() */

