/* ----------------------------------------------------------------	*/
/* src_predictive/post_reoccurring_transaction.c			*/
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
#include "date.h"
#include "boolean.h"
#include "bank_upload.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
int get_days_between_last_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *transaction_description );

TRANSACTION *post_reoccurring_get_accrued_daily_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_daily_amount );

TRANSACTION *post_reoccurring_get_now_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double transaction_amount,
			char *memo );

char *post_reoccurring_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			double transaction_amount,
			char *memo );

void post_reoccurring_transaction_display(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			boolean with_html );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *full_name;
	char *street_address;
	char *transaction_description;
	char *transaction_date;
	char *transaction_date_time;
	double transaction_amount;
	char *memo;
	boolean execute;
	boolean with_html;
	char title[ 128 ];
	DOCUMENT *document = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};

	if ( argc > 0 )
	{
		application_name = argv[ 1 ];

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
	}

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s application process full_name street_address transaction_description transaction_date transaction_amount memo execute_yn with_html_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	full_name = argv[ 3 ];
	street_address = argv[ 4 ];
	transaction_description = argv[ 5 ];
	transaction_date = argv[ 6 ];
	transaction_amount = atof( argv[ 7 ] );
	memo = argv[ 8 ];
	execute = (*argv[ 9 ] == 'y');
	with_html = (*argv[ 10 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( with_html )
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

	/* Works even if transaction_date is empty or is "transaction_date" */
	/* ---------------------------------------------------------------- */
	transaction_date_time =
		ledger_get_transaction_date_time(
			transaction_date );

	post_reoccurring_transaction_display(
		application_name,
		full_name,
		street_address,
		transaction_description,
		transaction_date_time,
		transaction_amount,
		memo,
		with_html );

	if ( execute )
	{
		transaction_date_time =
			post_reoccurring_transaction(
				application_name,
				full_name,
				street_address,
				transaction_description,
				transaction_date_time,
				transaction_amount,
				memo );

		if ( with_html )
		{
			if ( transaction_date_time )
			{
				printf( "<h3>Process complete.</h3>\n" );
			}
			else
			{
				printf(
		      "<h3>Warning: no reoccurring transaction found.</h3>\n" );
			}
		}
	} /* if execute */

	if ( with_html ) document_close();

	exit( 0 );

} /* main() */

void post_reoccurring_transaction_display(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			boolean with_html )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char *heading = {0};
	char *justify = {0};
	char sys_string[ 512 ];
	FILE *output_pipe;
	char buffer[ 128 ];

	if ( ! ( reoccurring_transaction =
			bank_upload_reoccurring_transaction_new(
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

	if ( !reoccurring_transaction->accrued_daily_amount )
	{
		transaction =
			post_reoccurring_get_now_transaction(
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				transaction_amount,
				memo );
	}
	else
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

	if ( !transaction ) return;

	if ( with_html )
	{
		heading = "Transaction,Account,Debit,Credit";
		justify = "left,left,right,right";

		sprintf(sys_string,
			"html_table.e '' %s '^' %s",
			heading,
			justify );
	}
	else
	{
		strcpy( sys_string, "cat" );
	}

	output_pipe = popen( sys_string, "w" );

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
		return;
	}

	journal_ledger = list_get_pointer( transaction->journal_ledger_list );

	fprintf( output_pipe,
		 "%s^%s^%.2lf^\n",
		 transaction->transaction_date_time,
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

	pclose( output_pipe );

} /* post_reoccurring_transaction_display() */

/* Returns transaction_date_time */
/* ----------------------------- */
char *post_reoccurring_transaction(
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
			bank_upload_reoccurring_transaction_new(
					application_name,
					full_name,
					street_address,
					transaction_description,
					transaction_amount ) ) )
	{
		return (char *)0;
	}

	if ( !reoccurring_transaction->accrued_daily_amount )
	{
		transaction =
			post_reoccurring_get_now_transaction(
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				transaction_amount,
				memo );
	}
	else
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

} /* post_reoccurring_transaction() */

TRANSACTION *post_reoccurring_get_now_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
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

	transaction->journal_ledger_list = list_new();

	journal_ledger =
		journal_ledger_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			debit_account );

	journal_ledger->debit_amount = transaction_amount;

	list_append_pointer( transaction->journal_ledger_list, journal_ledger );

	journal_ledger =
		journal_ledger_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			credit_account );

	journal_ledger->credit_amount = transaction_amount;

	list_append_pointer( transaction->journal_ledger_list, journal_ledger );

	return transaction;

} /* post_reoccurring_get_now_transaction() */

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
	char *today;
	int days_between;

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

	max_transaction_date = pipe2string( sys_string );

	today = pipe2string( "now.sh ymd" );

	if ( !timlib_strlen( max_transaction_date ) )
	{
		sprintf( name_buffer,
			 "%d-01-01",
			 current_year );

		days_between =
			date_days_between(
				name_buffer /* from_date */,
				today /* to_date */,
				date_get_utc_offset() ) + 1;
	}
	else
	{
		days_between =
			date_days_between(
				max_transaction_date /* from_date */,
				today /* to_date */,
				date_get_utc_offset() ) + 1;
	}

	return days_between;

} /* get_days_between_last_transaction() */

