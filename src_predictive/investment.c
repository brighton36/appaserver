/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/investment.c		*/
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

char *investment_account_balance_get_join( void )
{
	char join[ 1024 ];

	sprintf(join,
		"%s.full_name = %s.full_name and		"
		"%s.street_address = %s.street_address and	"
		"%s.account_number = %s.account_number		",
		ACCOUNT_BALANCE_FOLDER_NAME,
		INVESTMENT_ACCOUNT_FOLDER_NAME,
		ACCOUNT_BALANCE_FOLDER_NAME,
		INVESTMENT_ACCOUNT_FOLDER_NAME,
		ACCOUNT_BALANCE_FOLDER_NAME,
		INVESTMENT_ACCOUNT_FOLDER_NAME );

	return strdup( join );

} /* investment_account_balance_get_join() */

char *investment_account_balance_get_select( void )
{
	char *select =
		"date_time,			"
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
		"transaction_date_time,		"
		"investment_account,		"
		"fair_value_adjustment_account	";

	return select;
}

LIST *investment_fetch_account_balance_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number )
{
	LIST *account_balance_list;
	char sys_string[ 1024 ];
	char where[ 512 ];
	char input_buffer[ 512 ];
	char buffer[ 128 ];
	char *select;
	char folder[ 256 ];
	ACCOUNT_BALANCE *account_balance;
	FILE *input_pipe;

	select = investment_account_balance_get_select();

	sprintf( folder,
		 "%s,%s",
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 INVESTMENT_ACCOUNT_FOLDER_NAME );

	sprintf( where,
		 "%s.full_name = '%s' and		"
		 "%s.street_address = '%s' and		"
		 "%s.account_number = '%s' and		"
		 "%s					",
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 street_address,
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 account_number,
		 investment_account_balance_get_join() );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	"
		 "			order=date_time	",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );

	account_balance_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		account_balance =
			investment_account_balance_parse(
				full_name,
				street_address,
				account_number,
				input_buffer );

		list_append_pointer( account_balance_list, account_balance );
	}

	pclose( input_pipe );

	return account_balance_list;

} /* investment_fetch_account_balance_list() */

ACCOUNT_BALANCE *investment_account_balance_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char buffer[ 128 ];
	char *select;
	char folder[ 256 ];
	ACCOUNT_BALANCE *account_balance;
	char *input_buffer;

	select = investment_account_balance_get_select();

	sprintf( folder,
		 "%s,%s",
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 INVESTMENT_ACCOUNT_FOLDER_NAME );

	sprintf( where,
		 "%s.full_name = '%s' and		"
		 "%s.street_address = '%s' and		"
		 "%s.account_number = '%s' and		"
		 "date_time = '%s' and			"
		 "%s					",
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 street_address,
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 account_number,
		 date_time,
		 investment_account_balance_get_join() );

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

	account_balance =
		investment_account_balance_parse(
			full_name,
			street_address,
			account_number,
			input_buffer );

	if ( !*account_balance->investment_account
	||   !*account_balance->fair_value_adjustment_account )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty chart of accounts in INVESTMENT_ACCOUNT (%s/%s/%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address,
			 account_number );
		exit( 1 );
	}

	account_balance->is_latest =
		investment_account_balance_is_latest(
			application_name,
			date_time );

	return account_balance;

} /* investment_account_balance_fetch() */

ACCOUNT_BALANCE *investment_account_balance_parse(
				char *full_name,
				char *street_address,
				char *account_number,
				char *input_buffer )
{
	ACCOUNT_BALANCE *account_balance;
	char buffer[ 128 ];
	char *date_time;

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	date_time = strdup( buffer );

	account_balance =
		investment_account_balance_new(
			full_name,
			street_address,
			account_number,
			date_time );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	account_balance->investment_operation = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	account_balance->share_price = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	account_balance->share_quantity_change = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	account_balance->share_quantity_balance = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	account_balance->book_value_change = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	account_balance->book_value_balance = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	account_balance->moving_share_price = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	account_balance->total_cost_balance = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
	account_balance->market_value = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 10 );
	account_balance->unrealized_gain_balance = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 11 );
	account_balance->unrealized_gain_change = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 12 );
	account_balance->realized_gain = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 13 );
	account_balance->transaction_date_time = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 14 );
	account_balance->investment_account = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 15 );
	account_balance->fair_value_adjustment_account = strdup( buffer );

	return account_balance;

} /* investment_account_balance_parse() */

boolean investment_account_balance_is_latest(
					char *application_name,
					char *date_time )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char *results;

	select = "max( date_time )";

	folder = ACCOUNT_BALANCE_FOLDER_NAME;

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder );

	results = pipe2string( sys_string );

	if ( !results || !*results )
		return 0;
	else
		return ( strcmp( results, date_time ) == 0 );

} /* investment_account_balance_is_latest() */

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
				double prior_unrealized_gain_balance,
				char *investment_operation,
				char *investment_account,
				char *fair_value_adjustment_account )
{
	ACCOUNT_BALANCE *a;

	a = investment_account_balance_new(
			full_name,
			street_address,
			account_number,
			date_time );

	a->investment_operation = investment_operation;
	a->investment_account = investment_account;
	a->fair_value_adjustment_account = fair_value_adjustment_account;

	if ( market_value
	&&   !share_price
	&&   !share_quantity_change
	&&   !share_quantity_balance )
	{
		share_price = 1.0;
		share_quantity_balance = market_value;
	}

	if ( share_quantity_balance
	&&   !share_price
	&&   !share_quantity_change
	&&   !market_value )
	{
		share_price = 1.0;
	}

	if ( share_quantity_change
	&&   !share_price
	&&   !share_quantity_balance
	&&   !market_value )
	{
		share_price = 1.0;
	}

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
	if ( timlib_strcmp( investment_operation, "sale" ) == 0 )
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

	if ( timlib_strcmp( investment_operation, "time_passage" ) == 0
	&&   !timlib_double_virtually_same(
		a->share_quantity_change, 0.0 ) )
	{
		a->realized_gain = a->book_value_change;
	}

	return a;

} /* investment_account_balance_calculate() */

FILE *investment_open_update_pipe( void )
{
	char *key_column_list;
	char sys_string[ 1024 ];
	FILE *output_pipe;

	key_column_list =
	"full_name,street_address,account_number,date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 key_column_list );

	output_pipe = popen( sys_string, "w" );

	return output_pipe;

} /* investment_open_update_pipe() */

void investment_account_balance_update(	ACCOUNT_BALANCE *new_account_balance,
					ACCOUNT_BALANCE *account_balance )
{
	FILE *output_pipe = {0};

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

	if ( timlib_strcmp(	new_account_balance->investment_operation,
				account_balance->investment_operation ) != 0 )
	{
		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

		fprintf(output_pipe,
			"%s^%s^%s^%s^investment_operation^%s\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		new_account_balance->investment_operation );
	}

	if ( !timlib_double_virtually_same(
			new_account_balance->share_price,
			account_balance->share_price ) )
	{
		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

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
		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

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
		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

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
		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

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
		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

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
		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

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
		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

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
		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

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
		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

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
		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

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
		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

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
		char *transaction_date_time;

		transaction_date_time =
	 		new_account_balance->
				transaction_date_time;

		if ( !output_pipe ) output_pipe = investment_open_update_pipe();

		fprintf(output_pipe,
			"%s^%s^%s^%s^transaction_date_time^%s\n",
	 		new_account_balance->full_name,
	 		new_account_balance->street_address,
	 		new_account_balance->account_number,
	 		new_account_balance->date_time,
	 		(transaction_date_time)
				? transaction_date_time
				: "" );
	}

	if ( output_pipe ) pclose( output_pipe );

} /* investment_account_balance_update() */

LIST *investment_get_fair_value_adjustment_ledger_list(
				double unrealized_gain_change,
				char *unrealized_investment,
				char *fair_value_adjustment )
{
	LIST *journal_ledger_list;
	JOURNAL_LEDGER *journal_ledger;

	if ( !unrealized_investment
	||   !*unrealized_investment
	||   !fair_value_adjustment
	||   !*fair_value_adjustment )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty account name(s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

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

		journal_ledger->debit_amount = unrealized_gain_change;

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

		journal_ledger->credit_amount = unrealized_gain_change;

		list_append_pointer(
			journal_ledger_list,
			journal_ledger );
	}

	return journal_ledger_list;

} /* investment_get_fair_value_adjustment_ledger_list() */

char *investment_get_memo( char *investment_operation )
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

} /* investment_get_memo() */

TRANSACTION *investment_build_transaction(
				char *application_name,
				char *fund_name,
				ACCOUNT_BALANCE *account_balance )
{
	TRANSACTION *transaction;

	if ( timlib_strcmp(
			account_balance->investment_operation,
			"purchase" ) == 0 )
	{
		transaction =
			investment_build_purchase_transaction(
				application_name,
				fund_name,
				account_balance );
	}
	else
	if ( timlib_strcmp(
			account_balance->investment_operation,
			"time_passage" ) == 0 )
	{
		transaction =
			investment_build_time_transaction(
				application_name,
				fund_name,
				account_balance );
	}
	else
	if ( timlib_strcmp(
			account_balance->investment_operation,
			"sale" ) == 0 )
	{
		transaction =
			investment_build_sale_transaction(
				application_name,
				fund_name,
				account_balance );
	}
	else
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: unrecognized investment_operation = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 account_balance->investment_operation );
		exit( 0 );
	}

	transaction->transaction_date_time =
		ledger_fetch_unique_transaction_date_time(
			application_name,
			account_balance->date_time );

	return transaction;

} /* investment_build_transaction() */

TRANSACTION *investment_build_purchase_transaction(
				char *application_name,
				char *fund_name,
				ACCOUNT_BALANCE *account_balance )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char *realized_gain = {0};
	char *unrealized_investment = {0};
	char *realized_loss = {0};
	char *checking_account = {0};

	ledger_get_investment_account_names(
		&realized_gain,
		&unrealized_investment,
		&realized_loss,
		&checking_account,
		application_name,
		fund_name );

	transaction =
		ledger_transaction_new(
			account_balance->full_name,
			account_balance->street_address,
			account_balance->date_time
				/* transaction_date_time */,
			investment_get_memo(
				account_balance->investment_operation ) );

	transaction->transaction_amount =
		account_balance->book_value_change;

	transaction->journal_ledger_list = list_new();

	if ( account_balance->share_quantity_change > 0.0 )
	{
		/* Debit investment account */
		/* ------------------------ */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account_balance->investment_account );
	
		journal_ledger->debit_amount =
			account_balance->book_value_change;

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
			account_balance->book_value_change;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	if ( !timlib_double_virtually_same(
		account_balance->unrealized_gain_change, 0.0 ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			investment_get_fair_value_adjustment_ledger_list(
				account_balance->unrealized_gain_change,
				unrealized_investment,
				account_balance->
					fair_value_adjustment_account ) );
	}

	return transaction;

} /* investment_build_purchase_transaction() */

TRANSACTION *investment_build_sale_transaction(
				char *application_name,
				char *fund_name,
				ACCOUNT_BALANCE *account_balance )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char *realized_gain = {0};
	char *unrealized_investment = {0};
	char *realized_loss = {0};
	char *checking_account = {0};
	double cash_in;

	ledger_get_investment_account_names(
		&realized_gain,
		&unrealized_investment,
		&realized_loss,
		&checking_account,
		application_name,
		fund_name );

	transaction =
		ledger_transaction_new(
			account_balance->full_name,
			account_balance->street_address,
			account_balance->date_time
				/* transaction_date_time */,
			investment_get_memo(
				account_balance->investment_operation ) );

	transaction->journal_ledger_list = list_new();

	cash_in =
		0.0 -
		( account_balance->share_price *
		  account_balance->share_quantity_change );

	transaction->transaction_amount = cash_in;

	/* Debit checking for cash in */
	/* -------------------------- */
	journal_ledger =
		journal_ledger_new(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			checking_account );

	journal_ledger->debit_amount = cash_in;

	list_append_pointer(
		transaction->journal_ledger_list,
		journal_ledger );

	/* If sale realized a gain */
	/* ------------------------ */
	if ( !timlib_double_virtually_same(
		account_balance->realized_gain, 0.0 )
	&&   account_balance->realized_gain > 0.0 )
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
			account_balance->realized_gain;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* If sale realized a loss */
	/* ----------------------- */
	if ( !timlib_double_virtually_same(
		account_balance->realized_gain, 0.0 )
	&&   account_balance->realized_gain < 0.0 )
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
			-account_balance->realized_gain;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	if ( !timlib_double_virtually_same(
		account_balance->unrealized_gain_change, 0.0 ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			investment_get_fair_value_adjustment_ledger_list(
				account_balance->unrealized_gain_change,
				unrealized_investment,
				account_balance->
					fair_value_adjustment_account ) );
	}

	/* Credit investment */
	/* ----------------- */
	journal_ledger =
		journal_ledger_new(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			account_balance->investment_account );

	journal_ledger->credit_amount =
		0.0 - account_balance->book_value_change;

	list_append_pointer(
		transaction->journal_ledger_list,
		journal_ledger );

	return transaction;

} /* investment_build_sale_transaction() */

TRANSACTION *investment_build_time_transaction(
				char *application_name,
				char *fund_name,
				ACCOUNT_BALANCE *account_balance )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char *realized_gain = {0};
	char *unrealized_investment = {0};
	char *realized_loss = {0};
	char *checking_account = {0};

	ledger_get_investment_account_names(
		&realized_gain,
		&unrealized_investment,
		&realized_loss,
		&checking_account,
		application_name,
		fund_name );

	transaction =
		ledger_transaction_new(
			account_balance->full_name,
			account_balance->street_address,
			account_balance->date_time
				/* transaction_date_time */,
			investment_get_memo(
				account_balance->investment_operation ) );

	transaction->journal_ledger_list = list_new();

	transaction->transaction_amount = account_balance->book_value_change;

	/* If reinvested dividends, then realized a gain. */
	/* ---------------------------------------------- */
	if ( account_balance->share_quantity_change > 0.0 )
	{
		/* Debit investment */
		/* ---------------- */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account_balance->investment_account );

		journal_ledger->debit_amount =
			account_balance->realized_gain;

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
			account_balance->realized_gain;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}
	else
	/* ----------------------------------------------------------- */
	/* If for some reason the shares go down, then realize a loss. */
	/* This is probably an unrecorded sale or withdrawal.	       */
	/* ----------------------------------------------------------- */
	if ( account_balance->share_quantity_change < 0.0 )
	{
		/* Debit realized loss */
		/* ------------------- */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				realized_loss );

		journal_ledger->debit_amount =
			-account_balance->realized_gain;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		/* Credit investment */
		/* ----------------- */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account_balance->investment_account );

		journal_ledger->credit_amount =
			-account_balance->realized_gain;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	if ( !timlib_double_virtually_same(
		account_balance->unrealized_gain_change, 0.0 ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			investment_get_fair_value_adjustment_ledger_list(
				account_balance->unrealized_gain_change,
				unrealized_investment,
				account_balance->
					fair_value_adjustment_account ) );
	}

	return transaction;

} /* investment_build_time_transaction() */

