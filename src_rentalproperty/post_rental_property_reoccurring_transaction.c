/* -------------------------------------------------------------------	*/
/* src_predictive/post_rental_property_reoccurring_transaction.c	*/
/* -------------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------	*/

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
#include "bank_upload.h"
#include "rental.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_reoccurring_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			char *rental_property_street_address );

void post_reoccurring_transaction_display(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			char *rental_property_street_address );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *full_name;
	char *street_address;
	char *transaction_date;
	char *transaction_date_time;
	char *transaction_description;
	double transaction_amount;
	char *rental_property_street_address;
	char *memo;
	boolean execute;
	char title[ 128 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	int tax_year;
	LIST *rental_property_string_list;

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s application process full_name street_address transaction_description transaction_date transaction_amount memo rental_property_street_address execute_yn\n",
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

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	process_name = argv[ 2 ];
	full_name = argv[ 3 ];
	street_address = argv[ 4 ];
	transaction_description = argv[ 5 ];
	transaction_date = argv[ 6 ];
	transaction_amount = atof( argv[ 7 ] );
	memo = argv[ 8 ];
	rental_property_street_address = argv[ 9 ];
	execute = (*argv[ 10 ] == 'y');

	tax_year = atoi( pipe2string( "now.sh ymd | piece.e '-' 0" ) );

	rental_property_string_list =
		rental_get_rental_property_string_list(
			application_name,
			tax_year );

	if ( ( !rental_property_street_address
	||     strcmp(	rental_property_street_address,
			"rental_property_street_address" ) == 0 )
	&&   ( list_exists_string(	rental_property_string_list,
					street_address ) ) )
	{
		rental_property_street_address = street_address;
	}

	appaserver_parameter_file = new_appaserver_parameter_file();

	format_initial_capital( title, process_name );
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

	printf( "<h1>%s</h1>\n", title );
	fflush( stdout );

	transaction_date_time =
		ledger_get_transaction_date_time(
			transaction_date );

	if ( !execute )
	{
		post_reoccurring_transaction_display(
			application_name,
			full_name,
			street_address,
			transaction_description,
			transaction_date_time,
			transaction_amount,
			memo,
			rental_property_street_address );
	}
	else
	{
		post_reoccurring_transaction(
			application_name,
			full_name,
			street_address,
			transaction_description,
			transaction_date_time,
			transaction_amount,
			memo,
			rental_property_street_address );

		printf( "<h3>Process complete.</h3>\n" );
	}

	document_close();

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
			char *rental_property_street_address )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;
	char *heading;
	char *justify;
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

	heading = "Transaction,Account,Debit,Credit";
	justify = "left,left,right,right";

	sprintf( sys_string,
		"html_table.e '' %s '^' %s",
		 heading,
		 justify );

	output_pipe = popen( sys_string, "w" );

	if ( memo && *memo && strcmp( memo, "memo" ) != 0 )
	{
		fprintf( output_pipe,
		 	 "Memo: %s\n",
		 	 memo );
	}

	if (	rental_property_street_address
	&&	*rental_property_street_address
	&&	strcmp(	rental_property_street_address,
			"rental_property_street_address" ) != 0 )
	{
		fprintf( output_pipe,
		 	 "Property: %s\n",
		 	 rental_property_street_address );
	}

	fprintf( output_pipe,
		 "%s^%s^%.2lf^\n",
		 transaction_date_time,
		 format_initial_capital(
			buffer,
			reoccurring_transaction->debit_account ),
		 reoccurring_transaction->transaction_amount );

	fprintf( output_pipe,
		 "^%s^^%.2lf\n",
		 format_initial_capital(
			buffer,
			reoccurring_transaction->credit_account ),
		 reoccurring_transaction->transaction_amount );

	pclose( output_pipe );

} /* post_reoccurring_transaction_display() */

void post_reoccurring_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			char *rental_property_street_address )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;
	TRANSACTION *transaction;
	JOURNAL_LEDGER *prior_ledger;
	ACCOUNT *account;
	LIST *propagate_account_list = list_new();

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

	if ( timlib_strcmp( memo, "memo" ) == 0 ) memo = (char *)0;

	post_reoccurring_transaction_display(
			application_name,
			full_name,
			street_address,
			transaction_description,
			transaction_date_time,
			transaction_amount,
			memo,
			rental_property_street_address );

	/* Insert the transaction */
	/* ---------------------- */
	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time,
			memo );

	ledger_transaction_insert(
		application_name,
		transaction->full_name,
		transaction->street_address,
		transaction->transaction_date_time,
		reoccurring_transaction->transaction_amount,
		transaction->memo,
		0 /* check_number */,
		0 /* not lock_transaction */ );

	ledger_transaction_generic_update(
		application_name,
		transaction->full_name,
		transaction->street_address,
		transaction->transaction_date_time,
		"rental_property_street_address",
		rental_property_street_address );

	/* Insert the debit account */
	/* ------------------------ */
	ledger_journal_ledger_insert(
			application_name,
			reoccurring_transaction->full_name,
			reoccurring_transaction->street_address,
			transaction->transaction_date_time,
			reoccurring_transaction->debit_account,
			reoccurring_transaction->transaction_amount,
			1 /* is_debit */ );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction->transaction_date_time,
				reoccurring_transaction->debit_account );

	account = ledger_account_new( reoccurring_transaction->debit_account );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			reoccurring_transaction->debit_account );

	list_append_pointer( propagate_account_list, account );

	/* Insert the credit account */
	/* ------------------------- */
	ledger_journal_ledger_insert(
			application_name,
			reoccurring_transaction->full_name,
			reoccurring_transaction->street_address,
			transaction->transaction_date_time,
			reoccurring_transaction->credit_account,
			reoccurring_transaction->transaction_amount,
			0 /* not is_debit */ );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction->transaction_date_time,
				reoccurring_transaction->credit_account );

	account = ledger_account_new( reoccurring_transaction->credit_account );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			reoccurring_transaction->credit_account );

	list_append_pointer( propagate_account_list, account );

	/* Propagate the ledger balances */
	/* ----------------------------- */
	ledger_account_list_propagate(
			propagate_account_list,
			application_name );

} /* post_reoccurring_transaction() */

