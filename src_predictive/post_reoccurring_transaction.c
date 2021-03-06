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

/* Prototypes */
/* ---------- */
void post_reoccurring_transaction_batch(
			FILE *output_pipe,
			char *application_name,
			char *transaction_date_time,
			boolean execute );

void post_reoccurring_transaction_entity(
			FILE *output_pipe,
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			boolean execute );

char *get_last_transaction_date(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *transaction_description );

int get_days_between_last_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *transaction_description );

TRANSACTION *post_reoccurring_get_accrued_monthly_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_monthly_amount );

TRANSACTION *post_reoccurring_get_accrued_daily_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_daily_amount );

TRANSACTION *post_reoccurring_get_recent_transaction(
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
			FILE *output_pipe,
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			double transaction_amount,
			char *memo );

/* Global variables */
/* ---------------- */
enum bank_upload_exception bank_upload_exception = {0};

int main( int argc, char **argv )
{
	char *application_name;
	char *session;
	char *role_name;
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
	char *heading = {0};
	char *justify = {0};
	char sys_string[ 512 ];
	FILE *output_pipe = {0};

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 12 )
	{
		fprintf( stderr,
"Usage: %s session role process full_name street_address transaction_description transaction_date transaction_amount memo execute_yn with_html_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session = argv[ 1 ];
	role_name = argv[ 2 ];
	process_name = argv[ 3 ];
	full_name = argv[ 4 ];
	street_address = argv[ 5 ];
	transaction_description = argv[ 6 ];
	transaction_date = argv[ 7 ];
	transaction_amount = atof( argv[ 8 ] );
	memo = argv[ 9 ];
	execute = (*argv[ 10 ] == 'y');
	with_html = (*argv[ 11 ] == 'y');

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

	if ( !*transaction_date
	||   strcmp( transaction_date, "transction_date" ) == 0 )
	{
		transaction_date =
			date_get_now_yyyy_mm_dd(
				date_get_utc_offset() );
	}

	transaction_date_time =
		ledger_get_transaction_date_time(
			transaction_date );

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

	/* If doing for a single entity */
	/* ---------------------------- */
	if ( strcmp( full_name, "full_name" ) != 0 )
	{
		post_reoccurring_transaction_entity(
				output_pipe,
				application_name,
				full_name,
				street_address,
				transaction_description,
				transaction_date_time,
				transaction_amount,
				memo,
				execute );
	}
	else
	/* --------------------- */
	/* If doing for in batch */
	/* --------------------- */
	if ( strcmp(	process_name,
			"post_reoccurring_transaction_accrual" ) == 0 )
	{
		post_reoccurring_transaction_batch(
				output_pipe,
				application_name,
				transaction_date_time,
				execute );
	}

	if ( output_pipe ) pclose( output_pipe );

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

		if ( with_html )
		{
			printf( "<h3>Process complete.</h3>\n" );
		}
	}

	exit( 0 );

} /* main() */

void post_reoccurring_transaction_batch(
				FILE *output_pipe,
				char *application_name,
				char *transaction_date_time,
				boolean execute )
{
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_description[ 128 ];

	input_pipe =
		popen( "populate_reoccurring_transaction_accrual.sh", "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( character_count(
			FOLDER_DATA_DELIMITER,
			input_buffer ) != 2 )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: not two delimiters in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			pclose( input_pipe );
			exit( 1 );
		}

		piece( full_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( street_address, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		piece(	transaction_description,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		post_reoccurring_transaction_entity(
				output_pipe,
				application_name,
				full_name,
				street_address,
				transaction_description,
				transaction_date_time,
				0.0 /* transaction_amount */,
				(char *)0 /* memo */,
				execute );
	}

} /* post_reoccurring_transaction_batch() */

void post_reoccurring_transaction_entity(
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
		post_reoccurring_transaction_display(
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
			post_reoccurring_transaction(
				application_name,
				full_name,
				street_address,
				transaction_description,
				transaction_date_time,
				transaction_amount,
				memo );
	}

} /* post_reoccurring_transaction_entity() */

void post_reoccurring_transaction_display(
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

	if ( ! ( reoccurring_transaction =
			reoccurring_transaction_new(
					application_name,
					full_name,
					street_address,
					transaction_description,
					transaction_amount ) ) )
	{
		printf(
		"<h3>Warning: no valid reoccurring transaction found.</h3>\n" );
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

	ledger_transaction_output_pipe_display(
		output_pipe,
		transaction->full_name,
		transaction->street_address,
		transaction->transaction_date_time,
		(memo && *memo) ? memo : transaction->memo,
		transaction->journal_ledger_list );

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

} /* post_reoccurring_transaction() */

TRANSACTION *post_reoccurring_get_recent_transaction(
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

	if ( days_between < 0.0 ) return (TRANSACTION *)0;

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

