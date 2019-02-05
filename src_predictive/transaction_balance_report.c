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
char *transaction_balance_report_get_missing_expense_message(
					char *transaction_date_time,
					LIST *transaction_balance_row_list,
					double bank_amount,
					double cash_running_balance,
					double bank_running_balance );

char *transaction_balance_report_get_finished_message(
					char *application_name,
					double cash_running_balance,
					double bank_running_balance );

void transaction_balance_report_summary_inbalance(
			char *application_name,
			TRANSACTION_BALANCE_BLOCK *last_inbalance_block );

void transaction_balance_report_summary_outbalance(
			TRANSACTION_BALANCE_BLOCK *last_outbalance_block,
			LIST *transaction_balance_row_list );

char *transaction_balance_report_get_duplicated_withdrawal_message(
					double transaction_amount,
					double bank_amount,
					double balance,
					double bank_running_balance );

double transaction_balance_report_get_duplicated_withdrawal_amount(
					char *message,
					char **duplicated_full_name,
					char **duplicated_transaction_date_time,
					double balance,
					double bank_running_balance );

char *transaction_balance_report_get_deposit_message(
					double transaction_amount,
					double bank_amount,
					double balance,
					double bank_running_balance );

double transaction_balance_report_get_duplicated_deposit_amount(
					char *message,
					char **duplicated_full_name,
					char **duplicated_transaction_date_time,
					double balance,
					double bank_running_balance );

TRANSACTION_BALANCE *transaction_balance_report_html_table(
					char *application_name,
					char *title,
					char *sub_title,
					char *begin_date_string );

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
	TRANSACTION_BALANCE *transaction_balance;

	/* Exits if fails */
	/* -------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s process begin_date output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	begin_date_string = argv[ 2 ];
	output_medium = argv[ 3 ];

	if ( !*begin_date_string
	||   strcmp( begin_date_string, "begin_date" ) == 0 )
	{
		begin_date_string =
			ledger_get_minimum_transaction_date(
				application_name );

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
				begin_date_string );

		transaction_balance->last_block_inbalance =
			transaction_balance_get_last_block_inbalance(
				transaction_balance->merged_block_list );

		if ( list_length( transaction_balance->inbalance_block_list ) )
		{
			if ( transaction_balance->last_block_inbalance )
			{
				transaction_balance_report_summary_inbalance(
					application_name,
					list_get_last_pointer( 
						transaction_balance->
						     inbalance_block_list ) );
			}
			else
			if ( list_length(
					transaction_balance->
						outbalance_block_list ) )
			{
				transaction_balance_report_summary_outbalance(
					list_get_last_pointer( 
						transaction_balance->
						     outbalance_block_list ),
					transaction_balance->
						input.
						transaction_balance_row_list );
			}
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
			char *begin_date_string )
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
				0.0 /* cash_ending_balance */ ) ) )
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

void transaction_balance_report_summary_inbalance(
			char *application_name,
			TRANSACTION_BALANCE_BLOCK *last_inbalance_block )
{
	TRANSACTION_BALANCE_ROW *row;
	char *finished_message;

	if ( !last_inbalance_block ) return;

	row = last_inbalance_block->end_transaction_balance;

	finished_message =
		transaction_balance_report_get_finished_message(
			application_name,
			row->cash_running_balance,
			row->bank_running_balance );

	if ( finished_message && *finished_message )
	{
		printf( "<h3>%s</h3>\n", finished_message );
		return;
	}

} /* transaction_balance_report_summary_inbalance() */

void transaction_balance_report_summary_outbalance(
			TRANSACTION_BALANCE_BLOCK *last_outbalance_block,
			LIST *transaction_balance_row_list )
{
	TRANSACTION_BALANCE_ROW *first_outbalance_row;
	char *duplicated_transaction_message = {0};
	char *deposit_message = {0};
	char *missing_expense_message = {0};

	if ( !last_outbalance_block ) return;

	first_outbalance_row =
		last_outbalance_block->
			begin_transaction_balance;

	printf( "<h3>First outblance row: %s</h3>\n",
		transaction_balance_row_display(
			first_outbalance_row,
			transaction_balance_row_list,
			first_outbalance_row->bank_amount ) );

	if ( transaction_balance_get_cash_running_balance_wrong(
			first_outbalance_row->transaction_date_time
				/* first_outbalance_transaction_date_time */,
			transaction_balance_row_list,
			first_outbalance_row->bank_amount ) )
	{
		duplicated_transaction_message =
		   transaction_balance_report_get_duplicated_withdrawal_message(
			first_outbalance_row->transaction_amount,
			first_outbalance_row->bank_amount,
			first_outbalance_row->cash_running_balance,
			first_outbalance_row->bank_running_balance );
	}

	if ( duplicated_transaction_message && *duplicated_transaction_message )
	{
		printf( "<h3>%s</h3>\n",
			duplicated_transaction_message );
		return;
	}

	deposit_message =
		transaction_balance_report_get_deposit_message(
			first_outbalance_row->transaction_amount,
			first_outbalance_row->bank_amount,
			first_outbalance_row->cash_running_balance,
			first_outbalance_row->bank_running_balance );

	if ( deposit_message && *deposit_message )
	{
		printf( "<h3>%s</h3>\n", deposit_message );
		return;
	}

	missing_expense_message =
		transaction_balance_report_get_missing_expense_message(
			first_outbalance_row->transaction_date_time,
			transaction_balance_row_list,
			first_outbalance_row->bank_amount,
			first_outbalance_row->cash_running_balance,
			first_outbalance_row->bank_running_balance );

	if ( missing_expense_message && *missing_expense_message )
	{
		printf( "<h3>%s</h3>\n", missing_expense_message );
		return;
	}

} /* transaction_balance_report_summary_outbalance() */

char *transaction_balance_report_get_missing_expense_message(
			char *transaction_date_time,
			LIST *transaction_balance_row_list,
			double bank_amount,
			double cash_running_balance,
			double bank_running_balance )
{
	boolean cash_running_balance_wrong;
	double anomaly_balance_difference;
	char sys_string[ 1024 ];
	int transaction_count;
	char *pending_transaction;
	char message[ 1024 ];

	cash_running_balance_wrong =
		transaction_balance_get_cash_running_balance_wrong(
			transaction_date_time
				/* first_outbalance_transaction_date_time */,
			transaction_balance_row_list,
			bank_amount );

	if ( !cash_running_balance_wrong ) return (char *)0;

	anomaly_balance_difference =
		transaction_balance_calculate_anomaly_balance_difference(
			cash_running_balance,
			bank_running_balance );

	if ( anomaly_balance_difference >= 0.0 ) return (char *)0;

	sprintf( sys_string,
		 "bank_upload_ledger_pending.sh | grep '\\^%.2lf$' | wc -l",
		 abs_float( anomaly_balance_difference ) );

	if ( ! ( transaction_count = atoi( pipe2string( sys_string ) ) ) )
	{
		sprintf( message,
"Warning: the anomaly is now %.2lf, and it's the cash running balance that is off. However, there no unmatched expense transactions of this amount.",
			 anomaly_balance_difference );

		return strdup( message );
	}

	if ( transaction_count > 1 )
	{
		sprintf( message,
"Warning: the anomaly is now %.2lf, and it's the cash running balance that is off. However, there are %d unmatched expense transactions of this amount.",
			 anomaly_balance_difference,
			 transaction_count );

		return strdup( message );
	}

	/* Count must be one. */
	/* ------------------ */
	sprintf( sys_string,
		 "bank_upload_ledger_pending.sh | grep '\\^%.2lf$'",
		 abs_float( anomaly_balance_difference ) );

	pending_transaction = pipe2string( sys_string );

	sprintf( message,
"The anomaly is now %.2lf. This is the unmatched expense transaction:<br>%s",
			 anomaly_balance_difference,
			 pending_transaction );

	return strdup( message );

} /* transaction_balance_report_get_missing_expense_message() */

/* If anomaly_balance_difference > 0 */
/* --------------------------------- */
char *transaction_balance_report_get_deposit_message(
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
		/* Okay so far. */
		/* ------------ */
		return (char *)0;
	}

	*message = '\0';

	duplicated_transaction_amount =
		transaction_balance_report_get_duplicated_deposit_amount(
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
		/* Okay so far. */
		/* ------------ */
		return (char *)0;
	}

	sprintf( message,
"Warning: Unreconciled deposit transaction to %s/%s with amount of %.2lf.",
		duplicated_full_name,
		duplicated_transaction_date_time,
		duplicated_transaction_amount );

	return strdup( message );

} /* transaction_balance_report_get_deposit_message() */

double transaction_balance_report_get_duplicated_deposit_amount(
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

	/* We have a difference. Is it a duplicated deposit? */
	/* ------------------------------------------------- */
	duplicated_transaction_amount = anomaly_balance_difference;

	/* -------------------------------------------------------- */
	/* If found, returns transaction_date_time.		    */
	/* If not found, returns null and message may be populated. */
	/* -------------------------------------------------------- */
	*duplicated_transaction_date_time =
		bank_upload_pending_amount_deposit(
			message,
			duplicated_full_name,
			duplicated_transaction_amount
				/* amount */ );

	/* It returned a warning message. */
	/* ------------------------------ */
	if ( *message ) return 0.0;

	if ( !*duplicated_transaction_date_time ) return 0.0;

	/* It found a duplicated transaction to delete. */
	/* -------------------------------------------- */
	return duplicated_transaction_amount;

} /* transaction_balance_report_get_duplicated_deposit_amount() */

char *transaction_balance_report_get_duplicated_withdrawal_message(
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
		transaction_balance_report_get_duplicated_withdrawal_amount(
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
"Duplicated withdrawal: (%s/%s) amount = %.2lf.",
		duplicated_full_name,
		duplicated_transaction_date_time,
		duplicated_transaction_amount );

	return strdup( message );

} /* transaction_balance_report_get_duplicated_withdrawal_message() */

double transaction_balance_report_get_duplicated_withdrawal_amount(
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

	/* --------------------------------------------------- */
	/* Returns transaction_date_time or null if not found. */
	/* Message will help to explain not found.	       */
	/* --------------------------------------------------- */
	*duplicated_transaction_date_time =
		bank_upload_pending_amount_withdrawal(
			message,
			duplicated_full_name,
			duplicated_transaction_amount
				/* amount */ );

	/* It returned a warning message. */
	/* ------------------------------ */
	if ( *message ) return 0.0;

	if ( !*duplicated_transaction_date_time )
	{
		return 0.0;
	}
	else
	{
		/* It found a duplicated transaction to delete. */
		/* -------------------------------------------- */
		return duplicated_transaction_amount;
	}

} /* transaction_balance_report_get_duplicated_withdrawal_amount() */

char *transaction_balance_report_get_finished_message(
			char *application_name,
			double cash_running_balance,
			double bank_running_balance )
{
	char message[ 1024 ];
	double bank_upload_balance;
	char *ptr = message;

	*message = '\0';

	if ( ! ( bank_upload_balance =
			bank_upload_archive_fetch_latest_running_balance(
				application_name ) ) )
	{
		sprintf( message,
			 "<h3>Warning: empty bank upload balance.</h3>\n" );

		return strdup( message );
	}

	if ( !timlib_dollar_virtually_same(
		cash_running_balance,
		bank_running_balance ) )
	{
		ptr += sprintf(	ptr,
				"<h3>Not finished:</h3>\n" );

		ptr += sprintf( ptr,
				"<table>\n" );

		ptr += sprintf( ptr,
				"<tr><td>Cash running balance<td>%.2lf\n",
				cash_running_balance );

		ptr += sprintf( ptr,
				"<tr><td>Bank running balance<td>%.2lf\n",
				bank_running_balance );

		ptr += sprintf( ptr,
				"<tr><td>Difference<td>%.2lf\n",
			 	cash_running_balance - bank_running_balance );

		ptr += sprintf( ptr,
				"</table>\n" );

		return strdup( message );
	}

	if ( timlib_dollar_virtually_same(
		cash_running_balance,
		bank_upload_balance ) )
	{
		/* Finished! */
		/* --------- */
		ptr += sprintf(	ptr,
				"<h3>Finished</h3>\n" );

		ptr += sprintf( ptr,
				"<table>\n" );

		ptr += sprintf( ptr,
				"<tr><td>Cash running balance:<td>%.2lf\n",
				cash_running_balance );

		ptr += sprintf( ptr,
				"<tr><td>Bank running balance:<td>%.2lf\n",
				bank_running_balance );

		ptr += sprintf( ptr,
			      "<tr><td>Bank upload archive balance:<td>%.2lf\n",
				bank_upload_balance );

		ptr += sprintf( ptr,
				"</table>\n" );

		return strdup( message );
	}

	ptr += sprintf(	ptr,
			"<h3>Not finished:</h3>\n" );

	ptr += sprintf( ptr,
			"<h3>Cash running balance: %.2lf</h3>\n",
			cash_running_balance );

	ptr += sprintf( ptr,
			"<h3>Bank running balance: %.2lf</h3>\n",
			bank_running_balance );

	ptr += sprintf( ptr,
			"<h3>Bank upload balance:  %.2lf</h3>\n",
			bank_upload_balance );

	return strdup( message );

} /* transaction_balance_report_get_finished_message() */

