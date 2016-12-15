/* ---------------------------------------------------------------	*/
/* src_capitolpops/post_change_donation_program.c			*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "donation.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_donation_amount_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date,
				char *account_name );

void post_change_donation_account_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date,
				char *preupdate_account_name );

void post_change_donation_program_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date,
				char *account_name,
				char *preupdate_account_name,
				char *preupdate_donation_amount );

void post_change_donation_program_delete(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date );

void post_change_donation_program_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *donation_date;
	char *account_name;
	char *state;
	char *preupdate_account_name;
	char *preupdate_donation_amount;
	char *database_string = {0};

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s application full_name street_address donation_date account_name state preupdate_account_name preupdate_donation_amount\n",
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

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	donation_date = argv[ 4 ];
	account_name = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_account_name = argv[ 7 ];
	preupdate_donation_amount = argv[ 8 ];

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	/* ------------------------------------------------------------ */
	/* If executed via propagate, then everything is		*/
	/* handled by the one_to_many folder's post_change_procecss. 	*/
	/* ------------------------------------------------------------ */
	if ( strcmp( full_name, "full_name" ) == 0 ) exit( 0 );
	if ( strcmp( donation_date, "donation_date" ) == 0 ) exit( 0 );
	if ( strcmp( account_name, "account" ) == 0 ) exit( 0 );

	/* There's no DONATION_PROGRAM.transaction_date_time. */
	/* -------------------------------------------------- */
	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_donation_program_insert(
				application_name,
				full_name,
				street_address,
				donation_date );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_donation_program_update(
				application_name,
				full_name,
				street_address,
				donation_date,
				account_name,
				preupdate_account_name,
				preupdate_donation_amount );
	}
	else
	if ( strcmp( state, "delete" ) == 0 )
	{
		post_change_donation_program_delete(
				application_name,
				full_name,
				street_address,
				donation_date );
	}

	return 0;

} /* main() */

void post_change_donation_program_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date )
{
	DONATION *donation;

	if ( !( donation =
			donation_fetch(
				application_name,
				full_name,
				street_address,
				donation_date ) ) )
	{
		return;
	}

	donation->total_donation_amount =
		donation_get_total_donation_amount(
			donation->donation_program_list );

	donation_update(
			application_name,
			donation->full_name,
			donation->street_address,
			donation->donation_date,
			donation->total_donation_amount,
			donation->database_total_donation_amount,
			donation->transaction_date_time,
			donation->database_transaction_date_time );

	ledger_transaction_amount_update(
		application_name,
		donation->full_name,
		donation->street_address,
		donation->transaction_date_time,
		donation->total_donation_amount,
		donation->database_total_donation_amount );

	donation_journal_ledger_refresh_and_propagate(
			application_name,
			donation->full_name,
			donation->street_address,
			donation->transaction_date_time,
			donation->donation_fund_list,
			0 /* not propagate_only */ );

} /* post_change_donation_program_insert() */

void post_change_donation_program_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *donation_date )
{
	DONATION *donation;

	if ( !( donation =
			donation_fetch(
				application_name,
				full_name,
				street_address,
				donation_date ) ) )
	{
		return;
	}

	donation->total_donation_amount =
		donation_get_total_donation_amount(
			donation->donation_program_list );

	donation_update(
			application_name,
			donation->full_name,
			donation->street_address,
			donation->donation_date,
			donation->total_donation_amount,
			donation->database_total_donation_amount,
			donation->transaction_date_time,
			donation->database_transaction_date_time );

	donation_journal_ledger_refresh_and_propagate(
			application_name,
			donation->full_name,
			donation->street_address,
			donation->transaction_date_time,
			donation->donation_fund_list,
			0 /* not propagate_only */ );

} /* post_change_donation_program_delete() */

void post_change_donation_program_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date,
				char *account_name,
				char *preupdate_account_name,
				char *preupdate_donation_amount )
{
	enum preupdate_change_state account_name_change_state;
	enum preupdate_change_state donation_amount_change_state;

	account_name_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_account_name,
			account_name /* postupdate_data */,
			"preupdate_account" );

	donation_amount_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_donation_amount,
			(char *)0 /* postupdate_data */,
			"preupdate_donation_amount" );

	if (	account_name_change_state ==
		from_something_to_something_else )
	{
		post_change_donation_account_update(
			application_name,
			full_name,
			street_address,
			donation_date,
			preupdate_account_name );
	}

	if (	donation_amount_change_state ==
		from_something_to_something_else )
	{
		post_change_donation_amount_update(
			application_name,
			full_name,
			street_address,
			donation_date,
			account_name );
	}

} /* post_change_donation_program_update() */

void post_change_donation_account_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date,
				char *preupdate_account_name )
{
	DONATION *donation;

	donation =
		donation_fetch(
			application_name,
			full_name,
			street_address,
			donation_date );

	if ( !donation )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find donation.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !donation->transaction )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty transaction.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	donation_journal_ledger_refresh_and_propagate(
			application_name,
			donation->full_name,
			donation->street_address,
			donation->transaction_date_time,
			donation->donation_fund_list,
			0 /* not propagate_only */ );

#ifdef NOT_DEFINED
	ledger_journal_ledger_list_propagate(
			application_name,
			donation->transaction->journal_ledger_list,
			donation->transaction_date_time
				/* propagate_transaction_date_time */ );
#endif

	ledger_propagate(
		application_name,
		donation->transaction_date_time
			/* propagate_transaction_date_time */,
		preupdate_account_name );

} /* post_change_donation_account_update() */

void post_change_donation_amount_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date,
				char *account_name )
{
	DONATION *donation;
	DONATION_PROGRAM *donation_program;

	donation =
		donation_fetch(
			application_name,
			full_name,
			street_address,
			donation_date );

	if ( !donation )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find donation.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( donation_program =
			donation_program_seek(
				donation->donation_program_list,
				account_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find donation_program.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	donation->total_donation_amount =
		donation_get_total_donation_amount(
			donation->donation_program_list );

	/* Update DONATION.total_donation_amount */
	/* ------------------------------------- */
	donation_update(
			application_name,
			donation->full_name,
			donation->street_address,
			donation->donation_date,
			donation->total_donation_amount,
			donation->database_total_donation_amount,
			donation->transaction_date_time,
			donation->database_transaction_date_time );

	/* Update TRANSACTION.transaction_amount */
	/* ------------------------------------- */
	ledger_transaction_amount_update(
		application_name,
		donation->full_name,
		donation->street_address,
		donation->transaction_date_time,
		donation->total_donation_amount,
		donation->database_total_donation_amount );

	/* Reset JOURNAL_LEDGER.debit_amount and credit_amount */
	/* --------------------------------------------------- */
	if ( !ledger_journal_ledger_list_reset_amount(
		donation->transaction->journal_ledger_list,
		donation_program->donation_amount ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot reset amount.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Update JOURNAL_LEDGER.debit_amount and credit_amount */
	/* ---------------------------------------------------- */
	if ( !ledger_journal_ledger_list_amount_update(
		application_name,
		donation->transaction->journal_ledger_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot update amount.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Propagate JOURNAL_LEDGER */
	/* ------------------------ */
	if ( !ledger_propagate_journal_ledger_list(
			application_name,
			donation->transaction->journal_ledger_list,
			donation->transaction_date_time
				/* propagate_transaction_date_time */ ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot propagate ledger.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

} /* post_change_donation_amount_update() */

