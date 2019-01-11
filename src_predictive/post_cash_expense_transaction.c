/* ----------------------------------------------------------------	*/
/* src_predictive/post_cash_expense_transaction.c			*/
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
#include "column.h"
#include "environ.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "ledger.h"
#include "appaserver_parameter_file.h"
#include "date.h"
#include "folder_menu.h"
#include "accrual.h"
#include "boolean.h"
#include "bank_upload.h"
#include "reoccurring.h"

/* Constants */
/* --------- */
#define OUTPUT_MEDIUM_TABLE			"table"
#define OUTPUT_MEDIUM_TEXT			"text"
#define BANK_UPLOAD_TRANSACTION_FOLDER_NAME	"bank_upload_transaction"

/* Prototypes */
/* ---------- */
void post_cash_expense_transaction(
				FILE *output_pipe,
				char *application_name,
				char *full_name,
				char *street_address,
				char *bank_date,
				char *bank_description,
				char *debit_account,
				char *memo,
				boolean execute );

TRANSACTION *post_cash_expense_get_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			double transaction_amount,
			char *memo );

/* Global variables */
/* ---------------- */
enum bank_upload_exception bank_upload_exception = {0};

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *session;
	char *role_name;
	char *full_name;
	char *street_address;
	char *bank_date;
	char *bank_description;
	char *debit_account;
	char *memo;
	char *output_medium;
	boolean execute;
	char title[ 128 ];
	DOCUMENT *document = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *heading = {0};
	char *justify = {0};
	char sys_string[ 512 ];
	FILE *output_pipe = {0};

	/* This exits if it fails. */
	/* ----------------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 12 )
	{
		fprintf( stderr,
"Usage: %s process session role full_name street_address bank_date bank_description debit_account memo output_medium execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	session = argv[ 2 ];
	role_name = argv[ 3 ];
	full_name = argv[ 4 ];
	street_address = argv[ 5 ];
	bank_date = argv[ 6 ];
	bank_description = argv[ 7 ];
	account_name = argv[ 8 ];
	memo = argv[ 9 ];
	output_medium = atof( argv[ 10 ] );
	execute = (*argv[ 11 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = OUTPUT_MEDIUM_TABLE;
	}

	if ( strcmp( output_medium, OUTPUT_MEDIUM_TABLE ) == 0 )
	{
		format_initial_capital( title, process_name );
		document = document_new( title, application_name );
		document->output_content_type = 1;
	
		document_output_heading(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
		document_output_body(	document->application_name,
					document->onload_control_string );
	
		printf( "<h1>%s</h1>\n", title );
		fflush( stdout );
	}

	if ( strcmp( output_medium, OUTPUT_MEDIUM_TABLE ) == 0 )
	{
		heading = "Transaction,Account,Debit,Credit";
		justify = "left,left,right,right";

		sprintf(sys_string,
			"html_table.e '' %s '^' %s",
			heading,
			justify );
	}
	else
	if ( strcmp( output_medium, OUTPUT_MEDIUM_TEXT ) == 0 )
	{
		heading = "Transaction^Account^Debit^Credit";

		sprintf(sys_string,
			"(echo \"%s\"; cat)			|"
			"delimiter2padded_columns.e '^' 2	 ",
			heading );
	}
	else
	{
		strcpy( sys_string, "cat" );
	}

	output_pipe = popen( sys_string, "w" );

	post_cash_expense_transaction(
				output_pipe,
				application_name,
				full_name,
				street_address,
				bank_date,
				bank_description,
				debit_account,
				memo,
				execute );

	if ( execute )
	{
		folder_menu_refresh_row_count(
			application_name,
			TRANSACTION_FOLDER_NAME,
			session,
			appaserver_parameter_file->
				appaserver_data_directory,
			role_name );

		folder_menu_refresh_row_count(
			application_name,
			LEDGER_FOLDER_NAME,
			session,
			appaserver_parameter_file->
				appaserver_data_directory,
			role_name );

		folder_menu_refresh_row_count(
			application_name,
			BANK_UPLOAD_TRANSACTION_FOLDER_NAME,
			session,
			appaserver_parameter_file->
				appaserver_data_directory,
			role_name );

	}

	exit( 0 );

} /* main() */

void post_cash_expense_transaction_entity(
				FILE *output_pipe,
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_description,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				boolean execute )
{
	if ( !execute )
	{
		post_cash_expense_transaction_display(
			output_pipe,
			application_name,
			full_name,
			street_address,
			transaction_description,
			transaction_date_time,
			transaction_amount,
			memo );
	}
	else
	{
		transaction_date_time =
			post_cash_expense_transaction(
				application_name,
				full_name,
				street_address,
				transaction_description,
				transaction_date_time,
				transaction_amount,
				memo );
	}

} /* post_cash_expense_transaction_entity() */

void post_cash_expense_transaction_display(
			FILE *output_pipe,
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			double transaction_amount,
			char *memo )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char buffer[ 128 ];

	if ( ! ( reoccurring_transaction =
			reoccurring_transaction_new(
					application_name,
					full_name,
					street_address,
					transaction_description,
					transaction_amount ) ) )
	{
		printf(
		"<h3>Warning: no reoccurring transaction found.</h3>\n" );
		return;
	}

	if ( reoccurring_transaction->accrued_daily_amount )
	{
		transaction =
			post_reoccurring_get_accrued_daily_transaction(
				application_name,
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				reoccurring_transaction->
					transaction_description,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				reoccurring_transaction->accrued_daily_amount );
	}
	else
	if ( reoccurring_transaction->accrued_monthly_amount )
	{
		transaction =
			post_reoccurring_get_accrued_monthly_transaction(
				application_name,
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				reoccurring_transaction->
					transaction_description,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				reoccurring_transaction->
					accrued_monthly_amount );
	}
	else
	{
		transaction =
			post_reoccurring_get_recent_transaction(
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				reoccurring_transaction->transaction_amount,
				memo );
	}

	if ( !transaction ) return;

	if ( memo && *memo && strcmp( memo, "memo" ) != 0 )
	{
		fprintf( output_pipe,
		 	 "Memo: %s\n",
		 	 transaction->memo );
	}

	if ( !list_rewind( transaction->journal_ledger_list ) )
	{
		fprintf( output_pipe, "Error occurred.\n" );
		pclose( output_pipe );
		exit( 1 );
	}

	journal_ledger = list_get_pointer( transaction->journal_ledger_list );

	fprintf( output_pipe,
		 "%s^%s^%.2lf^\n",
		 transaction->full_name,
		 format_initial_capital(
			buffer,
			journal_ledger->account_name ),
		 journal_ledger->debit_amount );

	list_next( transaction->journal_ledger_list );
	journal_ledger = list_get_pointer( transaction->journal_ledger_list );

	fprintf( output_pipe,
		 "^%s^^%.2lf\n",
		 format_initial_capital(
			buffer,
			journal_ledger->account_name ),
		 journal_ledger->credit_amount );

} /* post_cash_expense_transaction_display() */

/* Returns transaction_date_time */
/* ----------------------------- */
char *post_cash_expense_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			double transaction_amount,
			char *memo )
{
	TRANSACTION *transaction;
	REOCCURRING_TRANSACTION *reoccurring_transaction;

	if ( ! ( reoccurring_transaction =
			reoccurring_transaction_new(
					application_name,
					full_name,
					street_address,
					transaction_description,
					transaction_amount ) ) )
	{
		return (char *)0;
	}

	if ( reoccurring_transaction->accrued_daily_amount )
	{
		transaction =
			post_reoccurring_get_accrued_daily_transaction(
				application_name,
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				reoccurring_transaction->
					transaction_description,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				reoccurring_transaction->accrued_daily_amount );
	}
	else
	if ( reoccurring_transaction->accrued_monthly_amount )
	{
		transaction =
			post_reoccurring_get_accrued_monthly_transaction(
				application_name,
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				reoccurring_transaction->
					transaction_description,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				reoccurring_transaction->
					accrued_monthly_amount );
	}
	else
	{
		transaction =
			post_reoccurring_get_recent_transaction(
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				reoccurring_transaction->transaction_amount,
				memo );
	}

	if ( !transaction ) return (char *)0;

	transaction->transaction_date_time =
		ledger_transaction_journal_ledger_insert(
			application_name,
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->transaction_amount,
			transaction->memo,
			0 /* check_number */,
			0 /* not lock_transaction */,
			transaction->journal_ledger_list );

	return transaction->transaction_date_time;

} /* post_cash_expense_transaction() */

TRANSACTION *post_cash_expense_get_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			double transaction_amount,
			char *memo )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time,
			memo );

	transaction->transaction_amount = transaction_amount;
	transaction->journal_ledger_list = list_new();

	journal_ledger =
		journal_ledger_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			debit_account );

	journal_ledger->debit_amount = transaction->transaction_amount;

	list_append_pointer( transaction->journal_ledger_list, journal_ledger );

	journal_ledger =
		journal_ledger_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			credit_account );

	journal_ledger->credit_amount = transaction->transaction_amount;

	list_append_pointer( transaction->journal_ledger_list, journal_ledger );

	return transaction;

} /* post_reoccurring_get_recent_transaction() */

TRANSACTION *post_reoccurring_get_accrued_daily_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_daily_amount )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	int days_between;
	double accrued_amount;

	if ( ! ( days_between =
			get_days_between_last_transaction(
				application_name,
				full_name,
				street_address,
				transaction_date_time,
				transaction_description ) ) )
	{
		return (TRANSACTION *)0;
	}

	accrued_amount = (double)days_between * accrued_daily_amount;

	if ( timlib_dollar_virtually_same( accrued_amount, 0.0 ) )
	{
		return (TRANSACTION *)0;
	}

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time,
			/* ------------------------------ */
			/* Don't update TRANSACTION.memo! */
			/* ------------------------------ */
			transaction_description /* memo */ );

	transaction->transaction_amount = accrued_amount;

	transaction->journal_ledger_list = list_new();

	journal_ledger =
		journal_ledger_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			debit_account );

	journal_ledger->debit_amount = transaction->transaction_amount;

	list_append_pointer( transaction->journal_ledger_list, journal_ledger );

	journal_ledger =
		journal_ledger_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			credit_account );

	journal_ledger->credit_amount = transaction->transaction_amount;

	list_append_pointer( transaction->journal_ledger_list, journal_ledger );

	return transaction;

} /* post_reoccurring_get_accrued_daily_transaction() */

/* get_last_transaction_date */

int get_days_between_last_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *transaction_description )
{
	char sys_string[ 1024 ];
	int current_year;
	char where[ 256 ];
	char name_buffer[ 256 ];
	char description_buffer[ 256 ];
	char *select;
	char *folder;
	char *max_transaction_date;
	char end_date_string[ 16 ];
	int days_between;

	if ( !transaction_date_time
	||   ! ( current_year = atoi( transaction_date_time ) ) )
	{
		return 0;
	}

	column( end_date_string, 0, transaction_date_time );

	select = "max( transaction_date_time )";
	folder = "transaction";

	sprintf( where,
		 "full_name = '%s' and				"
		 "street_address = '%s' and			"
		 "memo = '%s' and				"
		 "transaction_date_time >= '%d-01-01 00:00:00'	",
		 escape_character(	name_buffer,
					full_name,
					'\'' ),
		 street_address,
		 escape_character(	description_buffer,
					transaction_description,
					'\'' ),
		 current_year );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=\"%s\"			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			|"
		 "column.e 0						 ",
		 application_name,
		 select,
		 folder,
		 where );

	max_transaction_date = pipe2string( sys_string );

	if ( !timlib_strlen( max_transaction_date ) )
	{
		sprintf( name_buffer,
			 "%d-12-31",
			 current_year - 1 );

		days_between =
			date_days_between(
				name_buffer /* from_date */,
				end_date_string /* to_date */ );
	}
	else
	{
		days_between =
			date_days_between(
				max_transaction_date /* from_date */,
				end_date_string /* to_date */ );
	}

	return days_between;

} /* get_days_between_last_transaction() */

TRANSACTION *post_reoccurring_get_accrued_monthly_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_monthly_amount )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	double accrued_amount;
	char *begin_date_string;
	char end_date_string[ 16 ];

	begin_date_string =
			get_last_transaction_date(
				application_name,
				full_name,
				street_address,
				transaction_date_time,
				transaction_description );

	column( end_date_string, 0, transaction_date_time );

	accrued_amount =
		accrual_monthly_accrue(
			begin_date_string,
			end_date_string,
			accrued_monthly_amount
				/* monthly_accrual */ );

	if ( timlib_dollar_virtually_same( accrued_amount, 0.0 ) )
	{
		return (TRANSACTION *)0;
	}

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time,
			/* ------------------------------ */
			/* Don't update TRANSACTION.memo! */
			/* ------------------------------ */
			transaction_description /* memo */ );

	transaction->transaction_amount = accrued_amount;

	transaction->journal_ledger_list = list_new();

	journal_ledger =
		journal_ledger_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			debit_account );

	journal_ledger->debit_amount = transaction->transaction_amount;

	list_append_pointer( transaction->journal_ledger_list, journal_ledger );

	journal_ledger =
		journal_ledger_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			credit_account );

	journal_ledger->credit_amount = transaction->transaction_amount;

	list_append_pointer( transaction->journal_ledger_list, journal_ledger );

	return transaction;

} /* post_reoccurring_get_accrued_monthly_transaction() */

char *get_last_transaction_date(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *transaction_description )
{
	char sys_string[ 1024 ];
	int current_year;
	char where[ 256 ];
	char name_buffer[ 256 ];
	char description_buffer[ 256 ];
	char *select;
	char *folder;
	char *last_transaction_date_time;
	char last_transaction_date[ 16 ];

	if ( !transaction_date_time
	||   ! ( current_year = atoi( transaction_date_time ) ) )
	{
		return 0;
	}

	select = "max( transaction_date_time )";
	folder = "transaction";

	sprintf( where,
		 "full_name = '%s' and				"
		 "street_address = '%s' and			"
		 "memo = '%s' and				"
		 "transaction_date_time >= '%d-01-01 00:00:00'	",
		 escape_character(	name_buffer,
					full_name,
					'\'' ),
		 street_address,
		 escape_character(	description_buffer,
					transaction_description,
					'\'' ),
		 current_year );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=\"%s\"			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			|"
		 "column.e 0						 ",
		 application_name,
		 select,
		 folder,
		 where );

	last_transaction_date_time = pipe2string( sys_string );

	if ( !timlib_strlen( last_transaction_date_time ) )
		return (char *)0;

	column( last_transaction_date, 0, last_transaction_date_time );

	return strdup( last_transaction_date );

} /* get_last_transaction_date() */

void post_cash_expense_transaction(
				FILE *output_pipe,
				char *application_name,
				char *full_name,
				char *street_address,
				char *bank_date,
				char *bank_description,
				char *debit_account,
				char *memo,
				boolean execute )
{
} /* post_cash_expense_transaction() */
