/* --------------------------------------------------------------------	 */
/* $APPASERVER_HOME/src_accountancymodel/post_change_account_balance.c   */
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
#include "inventory.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "ledger.h"
#include "investment.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_account_balance_POR(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *account_number );

void post_change_account_balance_delete(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time );

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
	char *database_string = {0};
	ACCOUNT_BALANCE *account_balance;

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s application fund full_name street_address account_number date_time state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	fund_name = argv[ 2 ];
	full_name = argv[ 3 ];
	street_address = argv[ 4 ];
	account_number = argv[ 5 ];
	date_time = argv[ 6 ];
	state = argv[ 7 ];

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

	/* -------------------------------------------- */
	/* Need to execute on predelete to get		*/
	/* ACCOUNT_BALANCE.transaction_date_time. 	*/
	/* -------------------------------------------- */
	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	/* If running a batch. */
	/* ------------------- */
	if ( strcmp( state, "update" ) == 0
	&&   strcmp( date_time, "date_time" ) == 0 )
	{
		post_change_account_balance_POR(
			application_name,
			fund_name,
			full_name,
			street_address,
			account_number );

		exit( 0 );
	}

	if ( ! ( account_balance =
			investment_account_balance_fetch(
				application_name,
				full_name,
				street_address,
				account_number,
				date_time ) ) )
	{
		fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot account_balance_fetch()\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ( account_balance->is_latest
	||     strcmp( state, "update" ) == 0 ) )
	{
		post_change_account_balance_POR(
			application_name,
			fund_name,
			full_name,
			street_address,
			account_number );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_account_balance_insert(
			application_name,
			fund_name,
			account_balance );
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
			account_balance->transaction_date_time );
	}

	return 0;

} /* main() */

void post_change_account_balance_delete(
			char *application_name,
			char *fund_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	char *investment_account = {0};
	char *fair_value_adjustment = {0};
	char *realized_gain = {0};
	char *unrealized_investment = {0};
	char *realized_loss = {0};
	char *checking_account = {0};

	ledger_get_investment_account_names(
		&investment_account,
		&fair_value_adjustment,
		&realized_gain,
		&unrealized_investment,
		&realized_loss,
		&checking_account,
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
		investment_account );

	ledger_propagate(
		application_name,
		transaction_date_time,
		fair_value_adjustment );

	ledger_propagate(
		application_name,
		transaction_date_time,
		realized_gain );

	ledger_propagate(
		application_name,
		transaction_date_time,
		unrealized_investment );

	ledger_propagate(
		application_name,
		transaction_date_time,
		realized_loss );

	ledger_propagate(
		application_name,
		transaction_date_time,
		checking_account );

} /* post_change_account_balance_delete() */

void post_change_account_balance_insert(
				char *application_name,
				char *fund_name,
				ACCOUNT_BALANCE *account_balance )
{
	if ( strcmp( account_balance->investment_operation, "purchase" ) == 0 )
	{
		post_change_account_balance_insert_purchase(
			application_name,
			fund_name,
			account_balance );
	}
	else
	if ( strcmp( account_balance->investment_operation, "sale" ) == 0 )
	{
		if ( !post_change_account_balance_insert_sale(
				application_name,
				fund_name,
				account_balance ) )
		{
			printf( "<h3>Error: An error occurred.</h3>\n" );
		}
	}
	else
	{
		if ( !post_change_account_balance_insert_time_passage(
				application_name,
				fund_name,
				account_balance ) )
		{
			printf(
		"<h3>Error: The series must start with a purchase.</h3>\n" );
		}
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
				account_balance->share_quantity_balance,
				account_balance->market_value,
				0.0 /* prior_share_quantity_balance */,
				0.0 /* prior_book_value_balance */,
				0.0 /* prior_total_cost_balance */,
				0.0 /* prior_moving_share_price */,
				0.0 /* prior_unrealized_gain_balance */,
				account_balance->investment_operation );
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
				account_balance->share_quantity_balance,
				account_balance->market_value,
				prior_account_balance->share_quantity_balance,
				prior_account_balance->book_value_balance,
				prior_account_balance->total_cost_balance,
				prior_account_balance->moving_share_price,
				prior_account_balance->
					unrealized_gain_balance,
				account_balance->investment_operation );
	}

	new_account_balance->transaction =
		investment_build_transaction(
			application_name,
			fund_name,
			new_account_balance );

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
				account_balance->share_quantity_balance,
				account_balance->market_value,
				0.0 /* prior_share_quantity_balance */,
				0.0 /* prior_book_value_balance */,
				0.0 /* prior_total_cost_balance */,
				0.0 /* prior_moving_share_price */,
				0.0 /* prior_unrealized_gain_balance */,
				account_balance->investment_operation );
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
				account_balance->share_quantity_balance,
				account_balance->market_value,
				prior_account_balance->share_quantity_balance,
				prior_account_balance->book_value_balance,
				prior_account_balance->total_cost_balance,
				prior_account_balance->moving_share_price,
				prior_account_balance->
					unrealized_gain_balance,
				account_balance->investment_operation );
	}

	new_account_balance->transaction =
		investment_build_transaction(
			application_name,
			fund_name,
			new_account_balance );

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

} /* post_change_account_balance_insert_purchase() */

boolean post_change_account_balance_insert_sale(
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
			"ERROR in %s/%s()/%d: cannot account_balance_fetch()\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}

	if ( !prior_account_balance ) return 0;

	new_account_balance =
		investment_account_balance_calculate(
			account_balance->full_name,
			account_balance->street_address,
			account_balance->account_number,
			account_balance->date_time,
			account_balance->share_price,
			account_balance->share_quantity_change,
			account_balance->share_quantity_balance,
			account_balance->market_value,
			prior_account_balance->share_quantity_balance,
			prior_account_balance->book_value_balance,
			prior_account_balance->total_cost_balance,
			prior_account_balance->moving_share_price,
			prior_account_balance->unrealized_gain_balance,
			account_balance->investment_operation );

	/* The sale needs a negative share quantity change. */
	/* ------------------------------------------------ */
	if ( new_account_balance->share_quantity_change >= 0.0 ) return 0;

	new_account_balance->transaction =
		investment_build_transaction(
			application_name,
			fund_name,
			new_account_balance );

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

void post_change_account_balance_POR(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *account_number )
{
	ACCOUNT_BALANCE *account_balance;
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	LIST *account_balance_list;

	account_balance_list =
		investment_fetch_account_balance_list(
			application_name,
			full_name,
			street_address,
			account_number );

	if ( !list_rewind( account_balance_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty account_balance_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

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
				account_balance->share_quantity_balance,
				account_balance->market_value,
				0.0 /* prior_share_quantity_balance */,
				0.0 /* prior_book_value_balance */,
				0.0 /* prior_total_cost_balance */,
				0.0 /* prior_moving_share_price */,
				0.0 /* prior_unrealized_gain_balance */,
				account_balance->investment_operation );
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
				account_balance->share_quantity_balance,
				account_balance->market_value,
				prior_account_balance->share_quantity_balance,
				prior_account_balance->book_value_balance,
				prior_account_balance->total_cost_balance,
				prior_account_balance->moving_share_price,
				prior_account_balance->
					unrealized_gain_balance,
				account_balance->investment_operation );
		}

		new_account_balance->transaction =
			investment_build_transaction(
				application_name,
				fund_name,
				new_account_balance );

		if ( !new_account_balance->transaction )
		{
			fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty transaction.\n",
		 		 __FILE__,
				 __FUNCTION__,
		 		 __LINE__ );
			exit( 1 );
		}

		if ( !list_length(	new_account_balance->
						transaction->
						journal_ledger_list ) )
		{
			fprintf( stderr,
	"Warning in %s/%s()/%d: empty journal ledger list for (%s/%s/%s/%s).\n",
		 		 __FILE__,
				 __FUNCTION__,
		 		 __LINE__,
				 new_account_balance->full_name,
				 new_account_balance->street_address,
				 new_account_balance->account_number,
				 new_account_balance->date_time );
		}

		/* If now no transaction */
		/* --------------------- */
		if ( account_balance->transaction_date_time
		&&   *account_balance->transaction_date_time
		&&   !list_length(	new_account_balance->
						transaction->
						journal_ledger_list ) )
		{
			ledger_delete(	application_name,
					TRANSACTION_FOLDER_NAME,
					account_balance->full_name,
					account_balance->street_address,
					account_balance->
						transaction_date_time );

			ledger_delete(	application_name,
					LEDGER_FOLDER_NAME,
					account_balance->full_name,
					account_balance->street_address,
					account_balance->
						transaction_date_time );
		}
		else
		/* -------------------- */
		/* If a new transaction */
		/* -------------------- */
		if ( !account_balance->transaction_date_time
		||   !*account_balance->transaction_date_time )
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
		&&   *account_balance->transaction_date_time )
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

} /* post_change_account_balance_POR() */

