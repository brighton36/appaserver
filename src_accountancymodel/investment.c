/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/investment.c	*/
/* ---------------------------------------------------- */
/* This is the appaserver investment ADT.		*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "investment.h"

ACCOUNT_BALANCE *investment_account_balance_new(
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time )
{
	ACCOUNT_BALANCE *t;

	if ( ! ( t = calloc( 1, sizeof( ACCOUNT_BALANCE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->full_name = full_name;
	t->street_address = street_address;
	t->account_number = account_number;
	t->date_time = date_time;
	return t;

} /* investment_account_balance_new() */

char *investment_account_balance_get_select( void )
{
	char *select =
		"investment_operation,		"
		"share_price,			"
		"share_quantity_change,		"
		"share_quantity_balance,	"
		"book_value_change,		"
		"book_value_balance,		"
		"moving_share_price,		"
		"total_cost_balance,		"
		"market_value,			"
		"unrealized_gain_balance,	"
		"unrealized_gain_change,	"
		"realized_gain,			"
		"transaction_date_time		";

	return select;
}

ACCOUNT_BALANCE *investment_account_balance_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time )
{
	char sys_string[ 1024 ];
	char where[ 256 ];
	char buffer[ 128 ];
	char *select;
	char *folder;
	ACCOUNT_BALANCE *account_balance;
	char *input_buffer;

	select = investment_account_balance_get_select();

	folder = ACCOUNT_BALANCE_FOLDER_NAME;

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "account_number = '%s' and		"
		 "date_time = '%s'			",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 account_number,
		 date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder,
		 where );

	if ( ! ( input_buffer = pipe2string( sys_string ) ) )
		return (ACCOUNT_BALANCE *)0;

	account_balance = investment_account_balance_parse(
				full_name,
				street_address,
				account_number,
				date_time,
				input_buffer );

	return account_balance;

} /* investment_account_balance_fetch() */

ACCOUNT_BALANCE *investment_account_balance_parse(
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time,
				char *input_buffer )
{
	ACCOUNT_BALANCE *account_balance;
	char buffer[ 128 ];

	account_balance =
		investment_account_balance_new(
			full_name,
			street_address,
			account_number,
			date_time );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	account_balance->investment_operation = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	account_balance->share_price = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	account_balance->share_quantity_change = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	account_balance->share_quantity_balance = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	account_balance->book_value_change = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	account_balance->book_value_balance = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	account_balance->moving_share_price = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	account_balance->total_cost_balance = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	account_balance->market_value = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
	account_balance->unrealized_gain_balance = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 10 );
	account_balance->unrealized_gain_change = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 11 );
	account_balance->realized_gain = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 12 );
	account_balance->transaction_date_time = strdup( buffer );

	return account_balance;

} /* investment_account_balance_parse() */

char *investment_account_balance_fetch_prior_date_time(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time )
{
	char buffer[ 128 ];
	char sys_string[ 1024 ];
	char where[ 256 ];
	char *select;
	char *folder;
	char *results;

	select = "max( date_time )";

	folder = ACCOUNT_BALANCE_FOLDER_NAME;

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "account_number = '%s' and		"
		 "date_time < '%s'			",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 account_number,
		 date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( results && !*results )
		return (char *)0;
	else
		return results;

} /* investment_account_balance_fetch_prior_date_time() */

ACCOUNT_BALANCE *investment_account_balance_calculate(
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time,
				double share_price,
				double share_quantity_change,
				double share_quantity_balance,
				double market_value,
				double prior_share_quantity_balance,
				double prior_book_value_balance,
				double prior_total_cost_balance,
				double prior_moving_share_price,
				double prior_unrealized_gain_balance )
{
	ACCOUNT_BALANCE *a;

	a = investment_account_balance_new(
			full_name,
			street_address,
			account_number,
			date_time );

	if ( !timlib_double_virtually_same( share_price, 0.0 ) )
	{
		a->share_price = share_price;
	}
	else
	if ( !timlib_double_virtually_same( share_quantity_balance, 0.0 ) )
	{
		a->share_price = market_value / share_quantity_balance;
	}

	if ( !timlib_double_virtually_same( share_quantity_change, 0.0 ) )
	{
		a->share_quantity_change = share_quantity_change;
	}
	else
	{
		a->share_quantity_change =
			share_quantity_balance -
			prior_share_quantity_balance;
	}

	if ( !timlib_double_virtually_same( share_quantity_balance, 0.0 ) )
	{
		a->share_quantity_balance = share_quantity_balance;
	}
	else
	{
		a->share_quantity_balance =
			prior_share_quantity_balance +
			a->share_quantity_change;
	}

	a->market_value = a->share_price * a->share_quantity_balance;

	/* If no change in share quantity */
	/* ------------------------------ */
	if ( timlib_double_virtually_same( a->share_quantity_change, 0.0 ) )
	{
		a->moving_share_price = prior_moving_share_price;
		a->total_cost_balance = prior_total_cost_balance;
		a->book_value_balance = prior_book_value_balance;
	}
	else
	/* ------- */
	/* If sale */
	/* ------- */
	if ( a->share_quantity_change < 0.0 )
	{
		double cash_in;

		a->moving_share_price = prior_moving_share_price;

		a->total_cost_balance =
			a->moving_share_price *
			a->share_quantity_balance;

		a->book_value_change =
			a->share_quantity_change *
			a->moving_share_price;

		a->book_value_balance =
			prior_book_value_balance +
			a->book_value_change;

		cash_in = share_price * -share_quantity_change;

		/* Book value change is negative. */
		/* ------------------------------ */
		a->realized_gain = cash_in + a->book_value_change;
	}
	else
	/* ------------------------------------ */
	/* If purchase or dividend reinvestment */
	/* ------------------------------------ */
	{
		a->book_value_change =
			a->share_quantity_change *
			a->share_price;

		a->book_value_balance =
			prior_book_value_balance +
			a->book_value_change;

		a->total_cost_balance =
			prior_total_cost_balance +
			a->book_value_change;

		if ( !timlib_double_virtually_same(
			a->share_quantity_balance, 0.0 ) )
		{
			a->moving_share_price =
				a->total_cost_balance /
				a->share_quantity_balance;
		}

	}

	a->unrealized_gain_balance =
		a->market_value -
		a->book_value_balance;

	a->unrealized_gain_change =
		a->unrealized_gain_balance -
		prior_unrealized_gain_balance;

	return a;

} /* investment_account_balance_calculate() */

void investment_account_balance_update(	char *application_name,
					ACCOUNT_BALANCE *new_account_balance,
					ACCOUNT_BALANCE *account_balance )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_name;
	char *key_column_list;

	if ( !new_account_balance
	||   !account_balance )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: received null input.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	table_name =
		get_table_name(
			application_name,
			ACCOUNT_BALANCE_FOLDER_NAME );

	key_column_list =
	"full_name,street_address,account_number,date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key_column_list );

	output_pipe = popen( sys_string, "w" );

	if ( !timlib_double_virtually_same(
			new_account_balance->share_price,
			account_balance->share_price ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^share_price^%.4lf\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->share_price );
	}

	if ( !timlib_double_virtually_same(
			new_account_balance->share_quantity_change,
			account_balance->share_quantity_change ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^share_quantity_change^%.4lf\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->share_quantity_change );
	}

	if ( !timlib_double_virtually_same(
			new_account_balance->share_quantity_balance,
			account_balance->share_quantity_balance ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^share_quantity_balance^%.4lf\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->share_quantity_balance );
	}

	if ( !timlib_double_virtually_same(
			new_account_balance->book_value_change,
			account_balance->book_value_change ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^book_value_change^%.4lf\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->book_value_change );
	}

	if ( !timlib_double_virtually_same(
			new_account_balance->book_value_balance,
			account_balance->book_value_balance ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^book_value_balance^%.4lf\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->book_value_balance );
	}

	if ( !timlib_double_virtually_same(
			new_account_balance->moving_share_price,
			account_balance->moving_share_price ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^moving_share_price^%.4lf\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->moving_share_price );
	}

	if ( !timlib_double_virtually_same(
			new_account_balance->total_cost_balance,
			account_balance->total_cost_balance ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^total_cost_balance^%.4lf\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->total_cost_balance );
	}

	if ( !timlib_double_virtually_same(
			new_account_balance->market_value,
			account_balance->market_value ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^market_value^%.4lf\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->market_value );
	}

	if ( !timlib_double_virtually_same(
			new_account_balance->unrealized_gain_balance,
			account_balance->unrealized_gain_balance ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^unrealized_gain_balance^%.4lf\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->unrealized_gain_balance );
	}

	if ( !timlib_double_virtually_same(
			new_account_balance->unrealized_gain_change,
			account_balance->unrealized_gain_change ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^unrealized_gain_change^%.4lf\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->unrealized_gain_change );
	}

	if ( !timlib_double_virtually_same(
			new_account_balance->realized_gain,
			account_balance->realized_gain ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^realized_gain^%.4lf\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->realized_gain );
	}

	if ( timlib_strcmp(
			new_account_balance->transaction_date_time,
			account_balance->transaction_date_time ) != 0 )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^transaction_date_time^%s\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->transaction_date_time );
	}

	pclose( output_pipe );

} /* investment_account_balance_update() */

LIST *investment_get_fair_value_adjustment_ledger_list(
				double unrealized_gain_change,
				char *unrealized_investment,
				char *fair_value_adjustment )
{
	LIST *journal_ledger_list;
	JOURNAL_LEDGER *journal_ledger;

	if ( timlib_double_virtually_same(
		unrealized_gain_change, 0.0 ) )
	{
		return (LIST *)0;
	}

	journal_ledger_list = list_new();

	if ( unrealized_gain_change < 0.0 )
	{
		/* Debit unrealized investment */
		/* --------------------------- */
		journal_ledger =
			journal_ledger_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				unrealized_investment );

		journal_ledger->debit_amount =
			0.0 -
			unrealized_gain_change;

		list_append_pointer(
			journal_ledger_list,
			journal_ledger );

		/* Credit fair value adjustment */
		/* ---------------------------- */
		journal_ledger =
			journal_ledger_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				fair_value_adjustment );

		journal_ledger->credit_amount =
			0.0 -
			unrealized_gain_change;

		list_append_pointer(
			journal_ledger_list,
			journal_ledger );
	}
	else
	{
		/* Debit fair value adjustment */
		/* --------------------------- */
		journal_ledger =
			journal_ledger_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				fair_value_adjustment );

		journal_ledger->debit_amount =
			unrealized_gain_change;

		list_append_pointer(
			journal_ledger_list,
			journal_ledger );

		/* Credit unrealized investment */
		/* ---------------------------- */
		journal_ledger =
			journal_ledger_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				unrealized_investment );

		journal_ledger->credit_amount =
			unrealized_gain_change;

		list_append_pointer(
			journal_ledger_list,
			journal_ledger );
	}

	return journal_ledger_list;

} /* investment_get_fair_value_adjustment_ledger_list() */
