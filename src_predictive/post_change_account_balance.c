/* --------------------------------------------------------------------	 */
/* $APPASERVER_HOME/src_predictive/post_change_account_balance.c   	 */
/* --------------------------------------------------------------------- */
/* 									 */
/* Freely available software: see Appaserver.org			 */
/* --------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "document.h"
#include "inventory.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "ledger.h"
#include "investment.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_investment_operation_update(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time,
				char *preupdate_investment_operation );

void post_change_date_time_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time );

void post_change_account_balance_list(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time );

void post_change_account_balance_delete(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *investment_account,
				char *fair_value_adjustment_account );

boolean post_change_account_balance_insert_time_passage(
				char *application_name,
				char *fund_name,
				ACCOUNT_BALANCE *account_balance );

void post_change_account_balance_insert_purchase(
				char *application_name,
				char *fund_name,
				ACCOUNT_BALANCE *account_balance );

boolean post_change_account_balance_insert_sale(
				char *application_name,
				char *fund_name,
				ACCOUNT_BALANCE *account_balance );

void post_change_account_balance_insert(
				char *application_name,
				char *fund_name,
				ACCOUNT_BALANCE *account_balance );

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *full_name;
	char *street_address;
	char *account_number;
	char *date_time;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_account_number;
	char *preupdate_date_time;
	char *preupdate_investment_operation;
	char *database_string = {0};
	INVESTMENT_EQUITY *investment_equity;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 13 )
	{
		fprintf( stderr,
"Usage: %s ignored fund full_name street_address account_number date_time state preupdate_full_name preupdate_street_address preupdate_account_number preupdate_date_time preupdate_investment_operation\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 2 ];
	full_name = argv[ 3 ];
	street_address = argv[ 4 ];
	account_number = argv[ 5 ];
	date_time = argv[ 6 ];
	state = argv[ 7 ];
	preupdate_full_name = argv[ 8 ];
	preupdate_street_address = argv[ 9 ];
	preupdate_account_number = argv[ 10 ];
	preupdate_date_time = argv[ 11 ];
	preupdate_investment_operation = argv[ 12 ];

	/* -------------------------------------------- */
	/* Need to execute on predelete to get		*/
	/* ACCOUNT_BALANCE.transaction_date_time. 	*/
	/* -------------------------------------------- */
	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( ! ( investment_equity =
			investment_equity_new(
					application_name,
					full_name,
					street_address,
					fund_name,
					account_number,
					date_time,
					state,
					preupdate_full_name,
					preupdate_street_address,
					preupdate_account_number,
					preupdate_date_time,
					preupdate_investment_operation );
	{
		fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot investment_equity_new()\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_account_balance_insert(
			investment_equity,
			application_name );
	}
	else
	if ( strcmp( state, "predelete" ) == 0 )
	/* -------------------------------------------- */
	/* Need to execute on predelete to get		*/
	/* ACCOUNT_BALANCE.transaction_date_time. 	*/
	/* -------------------------------------------- */
	{
		post_change_account_balance_delete(
			application_name,
			fund_name,
			account_balance->full_name,
			account_balance->street_address,
			account_balance->transaction_date_time,
			account_balance->investment_account,
			account_balance->fair_value_adjustment_account );
	}

	return 0;

} /* main() */

void post_change_account_balance_delete(
			char *application_name,
			char *fund_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *investment_account,
			char *fair_value_adjustment_account )
{
	char *realized_gain = {0};
	char *unrealized_investment = {0};
	char *realized_loss = {0};
	char *checking_account = {0};
	char *contribution_capital_account = {0};

	ledger_get_investment_account_names(
		&unrealized_investment,
		&realized_gain,
		&realized_loss,
		&checking_account,
		&contribution_capital_account,
		application_name,
		fund_name );

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	ledger_propagate(
		application_name,
		transaction_date_time,
		unrealized_investment );

	ledger_propagate(
		application_name,
		transaction_date_time,
		realized_gain );

	ledger_propagate(
		application_name,
		transaction_date_time,
		realized_loss );

	ledger_propagate(
		application_name,
		transaction_date_time,
		checking_account );

	ledger_propagate(
		application_name,
		transaction_date_time,
		contribution_capital_account );

	ledger_propagate(
		application_name,
		transaction_date_time,
		investment_account );

	ledger_propagate(
		application_name,
		transaction_date_time,
		fair_value_adjustment_account );

} /* post_change_account_balance_delete() */

void post_change_account_balance_insert(
				INVESTMENT_EQUITY *t,
				char *application_name )
{
	if ( ! ( t->output_account_balance_list =
			investment_calculate_account_balance_list(
				application_name,
				t->investment_account.full_name,
				t->investment_account.street_address,
				t->investment_account.account_number,
				t->input.fund_name,
				t->investment_account.investment_account,
				t->investment_account.
					fair_value_adjustment_account,
				t->input_account_balance_list ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot calculate account balance list (%s,%s,%s,%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 t->investment_account.full_name,
			 t->investment_account.street_address,
			 t->investment_account.account_number,
			 t->input.date_time );
		exit( 1 );
	}

} /* post_change_account_balance_insert() */

boolean post_change_account_balance_insert_time_passage(
				char *application_name,
				char *fund_name,
				ACCOUNT_BALANCE *account_balance )
{
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	char *prior_date_time;

	prior_date_time =
		investment_account_balance_fetch_prior_date_time(
			application_name,
			account_balance->full_name,
			account_balance->street_address,
			account_balance->account_number,
			account_balance->date_time );

	if ( prior_date_time
	&&   *prior_date_time
	&&   ( ! ( prior_account_balance =
			investment_account_balance_fetch(
				application_name,
				account_balance->full_name,
				account_balance->street_address,
				account_balance->account_number,
				prior_date_time ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot account_balance_fetch()\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !prior_account_balance )
	{
		new_account_balance =
			investment_account_balance_calculate(
				account_balance->full_name,
				account_balance->street_address,
				account_balance->account_number,
				account_balance->date_time,
				account_balance->share_price,
				account_balance->share_quantity_change,
				0.0 /* prior_share_quantity_balance */,
				0.0 /* prior_book_value_balance */,
				0.0 /* prior_moving_share_price */,
				0.0 /* prior_unrealized_gain_balance */,
				account_balance->investment_operation,
				1 /* first_row */ );
	}
	else
	{
		new_account_balance =
			investment_account_balance_calculate(
				account_balance->full_name,
				account_balance->street_address,
				account_balance->account_number,
				account_balance->date_time,
				account_balance->share_price,
				account_balance->share_quantity_change,
				prior_account_balance->share_quantity_balance,
				prior_account_balance->book_value_balance,
				prior_account_balance->moving_share_price,
				prior_account_balance->
					unrealized_gain_balance,
				account_balance->investment_operation,
				0 /* not first_row */ );
	}

	if ( !new_account_balance )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot calulate new balance.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	new_account_balance->investment_account =
		account_balance->investment_account;

	new_account_balance->transaction =
		investment_build_transaction(
			application_name,
			fund_name,
			new_account_balance,
			new_account_balance->investment_operation,
			new_account_balance->investment_account );

	if ( !new_account_balance->transaction )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty transaction.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	new_account_balance->transaction_date_time =
		ledger_transaction_journal_ledger_insert(
			application_name,
			new_account_balance->full_name,
			new_account_balance->street_address,
			new_account_balance->
				transaction->
				transaction_date_time,
			new_account_balance->
				transaction->
				transaction_amount,
			new_account_balance->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			new_account_balance->
				transaction->
				journal_ledger_list );

	investment_account_balance_update(
		new_account_balance,
		account_balance );

	return 1;

} /* post_change_account_balance_insert_time_passage() */

void post_change_account_balance_insert_purchase(
				char *application_name,
				char *fund_name,
				ACCOUNT_BALANCE *account_balance )
{
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	char *prior_date_time;

	if ( ( prior_date_time =
			investment_account_balance_fetch_prior_date_time(
				application_name,
				account_balance->full_name,
				account_balance->street_address,
				account_balance->account_number,
				account_balance->date_time ) ) )
	{
		if ( ! ( prior_account_balance =
				investment_account_balance_fetch(
					application_name,
					account_balance->full_name,
					account_balance->street_address,
					account_balance->account_number,
					prior_date_time ) ) )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: cannot fetch prior investment_account_balance_fetch()\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}

	if ( !prior_account_balance )
	{
		new_account_balance =
			investment_account_balance_calculate(
				account_balance->full_name,
				account_balance->street_address,
				account_balance->account_number,
				account_balance->date_time,
				account_balance->share_price,
				account_balance->share_quantity_change,
				0.0 /* prior_share_quantity_balance */,
				0.0 /* prior_book_value_balance */,
				0.0 /* prior_moving_share_price */,
				0.0 /* prior_unrealized_gain_balance */,
				account_balance->investment_operation,
				1 /* first_row */ );
	}
	else
	{
		new_account_balance =
			investment_account_balance_calculate(
				account_balance->full_name,
				account_balance->street_address,
				account_balance->account_number,
				account_balance->date_time,
				account_balance->share_price,
				account_balance->share_quantity_change,
				prior_account_balance->share_quantity_balance,
				prior_account_balance->book_value_balance,
				prior_account_balance->moving_share_price,
				prior_account_balance->
					unrealized_gain_balance,
				account_balance->investment_operation,
				0 /* not first_row */ );
	}

	if ( !new_account_balance )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot calulate new balance.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		printf( "<h3>An error occurred. Check log.</h3>\n" );
		exit( 1 );
	}

	new_account_balance->investment_account =
		account_balance->investment_account;
 
	new_account_balance->transaction =
		investment_build_transaction(
			application_name,
			fund_name,
			new_account_balance,
			new_account_balance->investment_operation,
			new_account_balance->investment_account );

	if ( !new_account_balance->transaction )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty transaction.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		printf( "<h3>An error occurred. Check log.</h3>\n" );
		exit( 1 );
	}

	new_account_balance->transaction_date_time =
		ledger_transaction_journal_ledger_insert(
			application_name,
			new_account_balance->full_name,
			new_account_balance->street_address,
			new_account_balance->
				transaction->
				transaction_date_time,
			new_account_balance->
				transaction->
				transaction_amount,
			new_account_balance->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			new_account_balance->
				transaction->
				journal_ledger_list );

	investment_account_balance_update(
		new_account_balance,
		account_balance );

} /* post_change_account_balance_insert_purchase() */

boolean post_change_account_balance_insert_sale(
				char *application_name,
				char *fund_name,
				ACCOUNT_BALANCE *account_balance )
{
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	char *prior_date_time;
	double share_quantity_change;

	if ( ( prior_date_time =
			investment_account_balance_fetch_prior_date_time(
				application_name,
				account_balance->full_name,
				account_balance->street_address,
				account_balance->account_number,
				account_balance->date_time ) ) )
	{
		if ( ! ( prior_account_balance =
				investment_account_balance_fetch(
					application_name,
					account_balance->full_name,
					account_balance->street_address,
					account_balance->account_number,
					prior_date_time ) ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot account_balance_fetch()\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}

	if ( !prior_account_balance ) return 0;

	/* The sale needs a negative share quantity change. */
	/* ------------------------------------------------ */
	if ( account_balance->share_quantity_change > 0.0 )
		share_quantity_change = -account_balance->share_quantity_change;
	else
		share_quantity_change = account_balance->share_quantity_change;

	new_account_balance =
		investment_account_balance_calculate(
			account_balance->full_name,
			account_balance->street_address,
			account_balance->account_number,
			account_balance->date_time,
			account_balance->share_price,
			share_quantity_change,
			prior_account_balance->share_quantity_balance,
			prior_account_balance->book_value_balance,
			prior_account_balance->moving_share_price,
			prior_account_balance->unrealized_gain_balance,
			account_balance->investment_operation,
			0 /* not first_row */ );

	if ( !new_account_balance )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot calulate new balance.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( new_account_balance->share_quantity_change == 0.0 ) return 0;

	new_account_balance->investment_account =
		account_balance->investment_account;

	new_account_balance->transaction =
		investment_build_transaction(
			application_name,
			fund_name,
			new_account_balance,
			new_account_balance->investment_operation,
			new_account_balance->investment_account );

	if ( !new_account_balance->transaction )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty transaction.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	new_account_balance->transaction_date_time =
		ledger_transaction_journal_ledger_insert(
			application_name,
			new_account_balance->full_name,
			new_account_balance->street_address,
			new_account_balance->
				transaction->
				transaction_date_time,
			new_account_balance->
				transaction->
				transaction_amount,
			new_account_balance->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			new_account_balance->
				transaction->
				journal_ledger_list );

	investment_account_balance_update(
		new_account_balance,
		account_balance );

	return 1;

} /* post_change_account_balance_insert_sale() */

void post_change_account_balance_list(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time )
{
	ACCOUNT_BALANCE *account_balance;
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	LIST *account_balance_list;
	char *begin_date_time = {0};

	if ( date_time && *date_time )
	{
		begin_date_time =
			investment_account_balance_fetch_prior_date_time(
				application_name,
				full_name,
				street_address,
				account_number,
				date_time );
	}

	account_balance_list =
		investment_fetch_account_balance_list(
			application_name,
			full_name,
			street_address,
			account_number,
			begin_date_time );

	if ( !list_rewind( account_balance_list ) ) return;

	do {
		account_balance = list_get_pointer( account_balance_list );

		if ( !prior_account_balance )
		{
			new_account_balance =
			   investment_account_balance_calculate(
				account_balance->full_name,
				account_balance->street_address,
				account_balance->account_number,
				account_balance->date_time,
				account_balance->share_price,
				account_balance->share_quantity_change,
				0.0 /* prior_share_quantity_balance */,
				0.0 /* prior_book_value_balance */,
				0.0 /* prior_moving_share_price */,
				0.0 /* prior_unrealized_gain_balance */,
				account_balance->investment_operation,
				1 /* first_row */ );
		}
		else
		{
			new_account_balance =
			   investment_account_balance_calculate(
				account_balance->full_name,
				account_balance->street_address,
				account_balance->account_number,
				account_balance->date_time,
				account_balance->share_price,
				account_balance->share_quantity_change,
				prior_account_balance->share_quantity_balance,
				prior_account_balance->book_value_balance,
				prior_account_balance->moving_share_price,
				prior_account_balance->
					unrealized_gain_balance,
				account_balance->investment_operation,
				0 /* not first_row */ );
		}

		if ( !new_account_balance )
		{
			fprintf( stderr,
	"Warning in %s/%s()/%d: cannot calculate investment_operation = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 account_balance->investment_operation );

			printf( "<h3>An error occurred. Check log.</h3>\n" );
			continue;
		}

		new_account_balance->investment_account =
			account_balance->investment_account;

		new_account_balance->fair_value_adjustment_account =
			account_balance->fair_value_adjustment_account;

		new_account_balance->transaction =
			investment_build_transaction(
				application_name,
				fund_name,
				new_account_balance,
				new_account_balance->investment_operation,
				new_account_balance->investment_account );

		if ( !new_account_balance->transaction )
		{
			fprintf( stderr,
	"Warning in %s/%s()/%d: empty transaction for (%s/%s/%s/%s/%s).\n",
		 		 __FILE__,
				 __FUNCTION__,
		 		 __LINE__,
				 new_account_balance->full_name,
				 new_account_balance->street_address,
				 new_account_balance->account_number,
				 new_account_balance->date_time,
				 new_account_balance->investment_operation );
			continue;
		}

		if ( !list_length(	new_account_balance->
						transaction->
						journal_ledger_list ) )
		{
			fprintf( stderr,
"Warning in %s/%s()/%d: empty journal ledger list for (%s/%s/%s/%s/%s).\n",
		 		 __FILE__,
				 __FUNCTION__,
		 		 __LINE__,
				 new_account_balance->full_name,
				 new_account_balance->street_address,
				 new_account_balance->account_number,
				 new_account_balance->date_time,
				 new_account_balance->investment_operation );
		}

		/* If now no transaction */
		/* --------------------- */
		if ( account_balance->transaction_date_time
		&&   *account_balance->transaction_date_time
		&&   !list_length(	new_account_balance->
						transaction->
						journal_ledger_list ) )
		{
			post_change_account_balance_delete(
				application_name,
				fund_name,
				account_balance->full_name,
				account_balance->street_address,
				account_balance->transaction_date_time,
				account_balance->investment_account,
				account_balance->
					fair_value_adjustment_account );
		}
		else
		/* -------------------- */
		/* If a new transaction */
		/* -------------------- */
		if ( ( !account_balance->transaction_date_time
		||   !*account_balance->transaction_date_time )
		&&   list_length(	new_account_balance->
						transaction->
						journal_ledger_list ) )
		{
			new_account_balance->transaction_date_time =
				ledger_transaction_journal_ledger_insert(
					application_name,
					new_account_balance->full_name,
					new_account_balance->street_address,
					new_account_balance->
						transaction->
						transaction_date_time,
					new_account_balance->
						transaction->
						transaction_amount,
					new_account_balance->
						transaction->memo,
					0 /* check_number */,
					1 /* lock_transaction */,
					new_account_balance->
						transaction->
						journal_ledger_list );
		}
		else
		/* ------------------------------- */
		/* Refresh an existing transaction */
		/* ------------------------------- */
		if ( account_balance->transaction_date_time
		&&   *account_balance->transaction_date_time
		&&   list_length(	new_account_balance->
						transaction->
						journal_ledger_list ) )
		{
			new_account_balance->transaction_date_time =
				ledger_transaction_refresh(
					application_name,
					new_account_balance->full_name,
					new_account_balance->street_address,
					/* ------------------------------ */
					/* Original transaction_date_time */
					/* ------------------------------ */
					account_balance->
						transaction_date_time,
					new_account_balance->
						transaction->
						transaction_amount,
					investment_get_memo(
						new_account_balance->
							investment_operation ),
					0 /* check_number */,
					1 /* lock_transaction */,
					new_account_balance->
						transaction->
						journal_ledger_list );
		}

		investment_account_balance_update(
			new_account_balance,
			account_balance );

		prior_account_balance = new_account_balance;

	} while( list_next( account_balance_list ) );

} /* post_change_account_balance_list() */

void post_change_date_time_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time )
{
	ACCOUNT_BALANCE *account_balance;
	char *preupdate_transaction_date_time;

	account_balance =
		investment_account_balance_fetch(
			application_name,
			full_name,
			street_address,
			account_number,
			date_time );

	if ( !account_balance )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot fetch account_balance(%s,%s,%s,%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			full_name,
			street_address,
			account_number,
			date_time );
		exit( 1 );
	}

	preupdate_transaction_date_time =
		account_balance->transaction_date_time;

	account_balance->transaction_date_time =
		ledger_transaction_date_time_update(
			account_balance->
				transaction->
				journal_ledger_list,
			application_name,
			full_name,
			street_address,
			date_time /* transaction_date_time */,
			preupdate_transaction_date_time );

	investment_transaction_date_time_update(
		full_name,
		street_address,
		account_number,
		date_time,
		account_balance->transaction_date_time,
		application_name );

} /* post_change_date_time_update() */

void post_change_investment_operation_update(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time,
				char *preupdate_investment_operation )
{
	ACCOUNT_BALANCE *account_balance;

	account_balance =
		investment_account_balance_fetch(
			application_name,
			full_name,
			street_address,
			account_number,
			date_time );

	if ( !account_balance )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot fetch account_balance(%s,%s,%s,%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			full_name,
			street_address,
			account_number,
			date_time );
		exit( 1 );
	}

	if ( !account_balance->transaction )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot fetch transaction(%s,%s,%s,%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			full_name,
			street_address,
			account_number,
			date_time );
		exit( 1 );
	}

	ledger_journal_ledger_list_propagate(
		account_balance->transaction->journal_ledger_list,
		application_name );

	ledger_propagate(
		application_name,
		account_balance->transaction->transaction_date_time,
		investment_fetch_purchase_credit_account_name(
			application_name,
			fund_name,
			preupdate_investment_operation ) );

} /* post_change_investment_operation_update() */

