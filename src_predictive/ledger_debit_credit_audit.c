/* -----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/ledger_debit_credit_audit.c		*/
/* -----------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* -----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "ledger.h"

/* Constants */
/* --------- */
#define DEBUG_MODE	0

/* Prototypes */
/* ---------- */
void ledger_debit_credit_audit(		char *application_name );

double ledger_debit_credit_difference(	double *balance_difference,
					char *application_name,
					LIST *journal_ledger_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};

	if ( argc != 2 )
	{
		fprintf( stderr, 
			 "Usage: %s application\n",
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
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	ledger_debit_credit_audit( application_name );

	return 0;
} /* main() */

void ledger_debit_credit_audit( char *application_name )
{
	LIST *transaction_list;
	TRANSACTION *transaction;
	double difference;
	double balance_difference;

	transaction_list = ledger_get_transaction_list( application_name );

	if ( !list_rewind( transaction_list ) ) return;

	do {
		transaction = list_get( transaction_list );

		if ( !list_length( transaction->journal_ledger_list ) )
		{
			printf( "Empty transaction: (%s/%s/%s)\n",
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time );
			continue;
		}

		balance_difference = 0.0;

		difference =
			ledger_debit_credit_difference(
				&balance_difference,
				application_name,
				transaction->journal_ledger_list );

		if ( !timlib_double_virtually_same(
			difference, 0.0 ) )
		{
			printf( "Transaction difference: (%s/%s/%s) = %.2lf\n",
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				difference );
		}

		if ( !double_virtually_same( balance_difference, 0.0 ) )
		{
			printf( "Balance difference: (%s/%s/%s) = %.2lf\n",
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				balance_difference );
		}

	} while( list_next( transaction_list ) );

} /* ledger_debit_credit_audit() */

double ledger_debit_credit_difference(	double *balance_difference,
					char *application_name,
					LIST *journal_ledger_list )
{
	JOURNAL_LEDGER *journal_ledger;
	double sum_debit_amount = 0.0;
	double sum_credit_amount = 0.0;
	double balance;
	double local_balance_difference = 0.0;

	if ( !list_rewind( journal_ledger_list ) ) return -1.0;

	do {
		journal_ledger = list_get( journal_ledger_list );

		sum_debit_amount += journal_ledger->debit_amount;
		sum_credit_amount += journal_ledger->credit_amount;

if ( DEBUG_MODE )
{
printf( "account = %s; previous_balance = %.2lf; debit_amount = %.2lf; credit_amount = %.2lf; balance = %.2lf\n",
journal_ledger->account_name,
journal_ledger->previous_balance,
journal_ledger->debit_amount,
journal_ledger->credit_amount,
journal_ledger->balance );
}

		if ( ledger_account_get_accumulate_debit(
			application_name,
			journal_ledger->account_name ) )
		{
			balance =	journal_ledger->previous_balance +
					journal_ledger->debit_amount -
					journal_ledger->credit_amount;
		}
		else
		{
			balance =	journal_ledger->previous_balance +
					journal_ledger->credit_amount -
					journal_ledger->debit_amount;
		}

/*
printf( "Got journal_ledger->balance - balance = %.2lf\n",
journal_ledger->balance - balance );
*/
 
		local_balance_difference +=
			( journal_ledger->balance - balance );

	} while( list_next( journal_ledger_list ) );

	*balance_difference = local_balance_difference;

	return sum_debit_amount - sum_credit_amount;

} /* ledger_debit_credit_difference() */

