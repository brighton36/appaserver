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
char *get_memo(			char *investment_operation );

void post_change_account_balance_POR(
				char *application_name,
				char *full_name,
				char *street_address,
				char *account_number );

void post_change_account_balance_delete(
				char *application_name,
				char *transaction_date_time );

boolean post_change_account_balance_insert_time_passage(
				char *application_name,
				ACCOUNT_BALANCE *account_balance );

void post_change_account_balance_insert_purchase(
				char *application_name,
				ACCOUNT_BALANCE *account_balance );

boolean post_change_account_balance_insert_sale(
				char *application_name,
				ACCOUNT_BALANCE *account_balance );

void post_change_account_balance_insert(
				char *application_name,
				ACCOUNT_BALANCE *account_balance );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *account_number;
	char *date_time;
	char *state;
	char *database_string = {0};
	ACCOUNT_BALANCE *account_balance;

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s application full_name street_address account_number date_time investment_opertion state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	account_number = argv[ 4 ];
	date_time = argv[ 5 ];
	state = argv[ 6 ];

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
			full_name,
			street_address,
			account_number );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_account_balance_insert(
			application_name,
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
				account_balance->transaction_date_time );
	}

	return 0;

} /* main() */

void post_change_account_balance_delete(
			char *application_name,
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
		(char *)0 /* fund_name */ );

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
				ACCOUNT_BALANCE *account_balance )
{
	if ( strcmp( account_balance->investment_operation, "purchase" ) == 0 )
	{
		post_change_account_balance_insert_purchase(
			application_name,
			account_balance );
	}
	else
	if ( strcmp( account_balance->investment_operation, "sale" ) == 0 )
	{
		if ( !post_change_account_balance_insert_sale(
				application_name,
				account_balance ) )
		{
			printf( "<h3>Error: An error occurred.</h3>\n" );
		}
	}
	else
	{
		if ( !post_change_account_balance_insert_time_passage(
				application_name,
				account_balance ) )
		{
			printf(
		"<h3>Error: The series must start with a purchase.</h3>\n" );
		}
	}

} /* post_change_account_balance_insert() */

boolean post_change_account_balance_insert_time_passage(
				char *application_name,
				ACCOUNT_BALANCE *account_balance )
{
	char *investment_account = {0};
	char *fair_value_adjustment = {0};
	char *realized_gain = {0};
	char *unrealized_investment = {0};
	char *realized_loss = {0};
	char *checking_account = {0};
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	char *prior_date_time;
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;

	ledger_get_investment_account_names(
		&investment_account,
		&fair_value_adjustment,
		&realized_gain,
		&unrealized_investment,
		&realized_loss,
		&checking_account,
		application_name,
		(char *)0 /* fund_name */ );

	if ( ! ( prior_date_time =
			investment_account_balance_fetch_prior_date_time(
				application_name,
				account_balance->full_name,
				account_balance->street_address,
				account_balance->account_number,
				account_balance->date_time ) ) )
	{
		return 0;
	}

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
			prior_account_balance->
				unrealized_gain_balance,
			account_balance->investment_operation,
			account_balance->transaction,
			account_balance->transaction_date_time );

	transaction =
		ledger_transaction_new(
			new_account_balance->full_name,
			new_account_balance->street_address,
			new_account_balance->date_time
				/* transaction_date_time */,
			get_memo( account_balance->investment_operation ) );

	transaction->journal_ledger_list = list_new();

	/* If reinvested dividends, then realized a gain. */
	/* ---------------------------------------------- */
	if ( new_account_balance->share_quantity_change > 0.0 )
	{
		/* Debit investment */
		/* ---------------- */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				investment_account );

		journal_ledger->debit_amount =
			new_account_balance->realized_gain;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		/* Credit realized gain */
		/* -------------------- */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				realized_gain );

		journal_ledger->credit_amount =
			new_account_balance->realized_gain;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	if ( !timlib_double_virtually_same(
		new_account_balance->unrealized_gain_change, 0.0 ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			investment_get_fair_value_adjustment_ledger_list(
				new_account_balance->unrealized_gain_change,
				unrealized_investment,
				fair_value_adjustment ) );
	}

	new_account_balance->transaction_date_time =
		ledger_transaction_journal_ledger_insert(
			application_name,
			new_account_balance->full_name,
			new_account_balance->street_address,
			new_account_balance->date_time,
			new_account_balance->book_value_change
				/* transaction_amount */,
			transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			transaction->journal_ledger_list );

	investment_account_balance_update(
		application_name,
		new_account_balance,
		account_balance );

	return 1;

} /* post_change_account_balance_insert_time_passage() */

void post_change_account_balance_insert_purchase(
				char *application_name,
				ACCOUNT_BALANCE *account_balance )
{
	char *investment_account = {0};
	char *fair_value_adjustment = {0};
	char *realized_gain = {0};
	char *unrealized_investment = {0};
	char *realized_loss = {0};
	char *checking_account = {0};
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	char *prior_date_time;
	JOURNAL_LEDGER *journal_ledger;
	TRANSACTION *transaction;

	ledger_get_investment_account_names(
		&investment_account,
		&fair_value_adjustment,
		&realized_gain,
		&unrealized_investment,
		&realized_loss,
		&checking_account,
		application_name,
		(char *)0 /* fund_name */ );

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
				account_balance->investment_operation,
				account_balance->transaction,
				account_balance->transaction_date_time );
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
				account_balance->investment_operation,
				account_balance->transaction,
				account_balance->transaction_date_time );
	}

	transaction =
		ledger_transaction_new(
			new_account_balance->full_name,
			new_account_balance->street_address,
			new_account_balance->date_time
				/* transaction_date_time */,
			get_memo( account_balance->investment_operation ) );

	transaction->transaction_amount =
		new_account_balance->book_value_change;

	transaction->journal_ledger_list = list_new();

	if ( new_account_balance->share_quantity_change > 0.0 )
	{
		/* Debit investment account */
		/* ------------------------ */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				investment_account );
	
		journal_ledger->debit_amount =
			new_account_balance->book_value_change;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		/* Credit checking account */
		/* ----------------------- */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				checking_account );
	
		journal_ledger->credit_amount =
			new_account_balance->book_value_change;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	if ( !timlib_double_virtually_same(
		new_account_balance->unrealized_gain_change, 0.0 ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			investment_get_fair_value_adjustment_ledger_list(
				new_account_balance->unrealized_gain_change,
				unrealized_investment,
				fair_value_adjustment ) );
	}

	new_account_balance->transaction_date_time =
		ledger_transaction_journal_ledger_insert(
			application_name,
			new_account_balance->full_name,
			new_account_balance->street_address,
			new_account_balance->date_time,
			transaction->transaction_amount,
			transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			transaction->journal_ledger_list );

	investment_account_balance_update(
		application_name,
		new_account_balance,
		account_balance );

} /* post_change_account_balance_insert_purchase() */

boolean post_change_account_balance_insert_sale(
				char *application_name,
				ACCOUNT_BALANCE *account_balance )
{
	char *investment_account = {0};
	char *fair_value_adjustment = {0};
	char *realized_gain = {0};
	char *unrealized_investment = {0};
	char *realized_loss = {0};
	char *checking_account = {0};
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	char *prior_date_time;
	JOURNAL_LEDGER *journal_ledger;
	TRANSACTION *transaction;

	ledger_get_investment_account_names(
		&investment_account,
		&fair_value_adjustment,
		&realized_gain,
		&unrealized_investment,
		&realized_loss,
		&checking_account,
		application_name,
		(char *)0 /* fund_name */ );

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
			account_balance->investment_operation,
			account_balance->transaction,
			account_balance->transaction_date_time );

	/* The sale needs a negative share quantity change. */
	/* ------------------------------------------------ */
	if ( new_account_balance->share_quantity_change >= 0.0 ) return 0;

	transaction =
		ledger_transaction_new(
			new_account_balance->full_name,
			new_account_balance->street_address,
			new_account_balance->date_time
				/* transaction_date_time */,
			get_memo( account_balance->investment_operation ) );

	transaction->journal_ledger_list = list_new();

	/* Debit checking for cash in */
	/* -------------------------- */
	journal_ledger =
		journal_ledger_new(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			checking_account );

	journal_ledger->debit_amount =
		0.0 -
		( new_account_balance->share_price *
		  new_account_balance->share_quantity_change );

	list_append_pointer(
		transaction->journal_ledger_list,
		journal_ledger );

	/* If sale realized a gain */
	/* ------------------------ */
	if ( !timlib_double_virtually_same(
		new_account_balance->realized_gain, 0.0 )
	&&   new_account_balance->realized_gain > 0.0 )
	{
		/* Credit account = realized gain */
		/* ------------------------------ */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				realized_gain );

		journal_ledger->credit_amount =
			new_account_balance->realized_gain;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* If sale realized a loss */
	/* ----------------------- */
	if ( !timlib_double_virtually_same(
		new_account_balance->realized_gain, 0.0 )
	&&   new_account_balance->realized_gain < 0.0 )
	{
		/* Debit account = realized loss */
		/* ----------------------------- */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				realized_loss );

		journal_ledger->debit_amount =
			-new_account_balance->realized_gain;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	if ( !timlib_double_virtually_same(
		new_account_balance->unrealized_gain_change, 0.0 ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			investment_get_fair_value_adjustment_ledger_list(
				new_account_balance->unrealized_gain_change,
				unrealized_investment,
				fair_value_adjustment ) );
	}

	/* Credit investment */
	/* ----------------- */
	journal_ledger =
		journal_ledger_new(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			investment_account );

	journal_ledger->credit_amount =
		0.0 - new_account_balance->book_value_change;

	list_append_pointer(
		transaction->journal_ledger_list,
		journal_ledger );

	new_account_balance->transaction_date_time =
		ledger_transaction_journal_ledger_insert(
			application_name,
			new_account_balance->full_name,
			new_account_balance->street_address,
			new_account_balance->date_time,
			new_account_balance->book_value_change
				/* transaction_amount */,
			transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			transaction->journal_ledger_list );

	investment_account_balance_update(
		application_name,
		new_account_balance,
		account_balance );

	return 1;

} /* post_change_account_balance_insert_sale() */

void post_change_account_balance_POR(
				char *application_name,
				char *full_name,
				char *street_address,
				char *account_number )
{
	char *investment_account = {0};
	char *fair_value_adjustment = {0};
	char *realized_gain = {0};
	char *unrealized_investment = {0};
	char *realized_loss = {0};
	char *checking_account = {0};
	ACCOUNT_BALANCE *account_balance;
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	JOURNAL_LEDGER *journal_ledger;
	TRANSACTION *transaction;
	LIST *account_balance_list;

	ledger_get_investment_account_names(
		&investment_account,
		&fair_value_adjustment,
		&realized_gain,
		&unrealized_investment,
		&realized_loss,
		&checking_account,
		application_name,
		(char *)0 /* fund_name */ );

	account_balance_list =
		investment_account_balance_fetch_list(
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
				account_balance->investment_operation,
				account_balance->transaction,
				account_balance->transaction_date_time );
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
				account_balance->investment_operation,
				account_balance->transaction,
				account_balance->transaction_date_time );
		}

		if ( !account_balance->transaction_date_time )
		{
			new_account_balance->transaction =
				investment_build_transaction(
					account_balance );

#ifdef NOT_DEFINED
			new_account_balance->transaction =
				ledger_transaction_new(
					new_account_balance->full_name,
					new_account_balance->street_address,
					new_account_balance->date_time
						/* transaction_date_time */,
					get_memo(
						account_balance->
						investment_operation ) );

			new_account_balance->transaction->transaction_amount =
				new_account_balance->book_value_change;

			new_account_balance->
				transaction->
				journal_ledger_list = list_new();

	if ( new_account_balance->share_quantity_change > 0.0 )
	{
		/* Debit investment account */
		/* ------------------------ */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				investment_account );
	
		journal_ledger->debit_amount =
			new_account_balance->book_value_change;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		/* Credit checking account */
		/* ----------------------- */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				checking_account );
	
		journal_ledger->credit_amount =
			new_account_balance->book_value_change;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	if ( !timlib_double_virtually_same(
		new_account_balance->unrealized_gain_change, 0.0 ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			investment_get_fair_value_adjustment_ledger_list(
				new_account_balance->unrealized_gain_change,
				unrealized_investment,
				fair_value_adjustment ) );
	}
#endif

		/* If a new transaction */
		/* -------------------- */
		if ( !account_balance->transaction_date_time )
		{
			new_account_balance->transaction_date_time =
				ledger_transaction_journal_ledger_insert(
					application_name,
					new_account_balance->full_name,
					new_account_balance->street_address,
					new_account_balance->date_time,
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

		if ( memcmp(	account_balance,
				new_account_balance,
				sizeof( ACCOUNT_BALANCE ) != 0 ) )
		{
			new_account_balance->transaction_date_time =
				ledger_transaction_refresh(
					application_name,
					new_account_balance->full_name,
					new_account_balance->street_address,
					new_account_balance->date_time,
					new_account_balance->
						transaction->
						transaction_amount,
					get_memo( new_account_balance->
							investment_operation ),
					0 /* check_number */,
					1 /* lock_transaction */,
					new_account_balance->
						transaction->
						journal_ledger_list );

			investment_account_balance_update(
				application_name,
				new_account_balance,
				account_balance );
		}

		prior_account_balance_list = account_balance_list;

	} while( list_next( account_balance_list ) );

} /* post_change_account_balance_POR() */

char *get_memo( char *investment_operation )
{
	char *memo;

	if ( strcmp( investment_operation, "time_passage" ) == 0 )
		memo = "Investment Time Passage";
	else
	if ( strcmp( investment_operation, "purchase" ) == 0 )
		memo = "Purchase Investment";
	else
	if ( strcmp( investment_operation, "sale" ) == 0 )
		memo = "Sell Investment";
	else
		memo = "";

	return memo;
} /* get_memo() */
