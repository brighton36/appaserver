/* -----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/ledger_debit_credit_audit.c	*/
/* -----------------------------------------------------------	*/
/* 								*/
/* This process outputs:					*/
/* 	full_name^						*/
/*	street_address^						*/
/*	transaction_date_time^					*/
/*	transaction_difference^					*/
/*	difference_type						*/
/* 								*/
/* Note: difference_type = {	no_journal_ledger,		*/
/*				debit_credit_difference,	*/
/*				balance_difference }		*/
/*								*/
/* 							 	*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

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
void ledger_debit_credit_audit(		char *application_name,
					char *begin_date );

double ledger_debit_credit_difference(	double *balance_difference,
					char *application_name,
					LIST *journal_ledger_list );

int main( int argc, char **argv )
{
	char *application_name;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 2 )
	{
		fprintf( stderr,
			 "Usage: %s begin_date\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	ledger_debit_credit_audit( application_name, argv[ 1 ] );

	return 0;

} /* main() */

void ledger_debit_credit_audit( char *application_name,
				char *begin_date )
{
	LIST *transaction_list;
	TRANSACTION *transaction;
	double difference;
	double balance_difference;
	char where_clause[ 128 ];
	char *difference_type;
	double report_difference;

	if ( *begin_date && strcmp( begin_date, "begin_date" ) != 0 )
	{
		sprintf( where_clause,
			 "%s.transaction_date_time >= '%s'",
			 TRANSACTION_FOLDER_NAME,
			 begin_date );
	}
	else
	{
		strcpy( where_clause, "1 = 1" );
	}

	transaction_list =
		ledger_fetch_transaction_list(
			application_name,
			where_clause );

	if ( !list_rewind( transaction_list ) ) return;

	do {
		transaction = list_get( transaction_list );

		difference_type = (char *)0;

		if ( !list_length( transaction->journal_ledger_list ) )
		{
			difference_type = "no_journal_ledger";
			report_difference = 0.0;
		}

		balance_difference = 0.0;

		difference =
			ledger_debit_credit_difference(
				&balance_difference,
				application_name,
				transaction->journal_ledger_list );

		if ( !difference_type
		&&   !timlib_double_virtually_same(
			difference, 0.0 ) )
		{
			difference_type = "debit_credit_difference";
			report_difference = difference;
		}

		if ( !difference_type
		&&   !double_virtually_same(
			balance_difference, 0.0 ) )
		{
			difference_type = "balance_difference";
			report_difference = balance_difference;
		}

		if ( difference_type )
		{
			printf( "%s^%s^%s^%.2lf^%s\n",
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				report_difference,
				difference_type );
		}

	} while ( list_next( transaction_list ) );

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

