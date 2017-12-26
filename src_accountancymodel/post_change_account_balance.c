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
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *account_number;
	char *date_time;
	char *state;
	char *database_string = {0};

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

	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_account_balance_insert(
			application_name,
			full_name,
			street_address,
			account_number,
			date_time );

	}
#ifdef NOT_DEFINED
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_account_balance_update(
				application_name,
				asset_name,
				serial_number,
				preupdate_extension );
	}
	else
	if ( strcmp( state, "predelete" ) == 0 )
	/* -------------------------------------------- */
	/* Need to execute on predelete to get		*/
	/* PRIOR_FIXED_ASSET.transaction_date_time. 	*/
	/* -------------------------------------------- */
	{
		post_change_account_balance_delete(
				application_name,
				asset_name,
				serial_number );
	}
#endif

	return 0;

} /* main() */

#ifdef NOT_DEFINED
void post_change_account_balance_delete(
			char *application_name,
			char *asset_name,
			char *serial_number )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
	LIST *primary_data_list;
	char *full_name = {0};
	char *street_address = {0};
	char *transaction_date_time = {0};
	double extension = {0};

	post_change_account_balance_fetch_row(
			&full_name,
			&street_address,
			&transaction_date_time,
			&extension,
			application_name,
			asset_name,
			serial_number );

	if (	!full_name
	||	!street_address
	||	!transaction_date_time
	||	!*transaction_date_time )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot post_change_account_balance_fetch_row(%s/%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 asset_name,
			 serial_number );
		exit( 1 );
	}

	primary_data_list =
		post_change_account_balance_get_primary_data_list(
			asset_name,
			serial_number );

	subsidiary_transaction =
		subsidiary_new(	application_name,
				ACCOUNT_BALANCE_FOLDER_NAME,
				primary_data_list,
				full_name,
				street_address,
				extension /* transaction_amount */ );

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

	if ( !subsidiary_transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: subsidiary_transaction is null. Can't propagate ledger.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
	}
	else
	{
		ledger_propagate(
			application_name,
			transaction_date_time,
			subsidiary_transaction->process.debit_account_name );

		ledger_propagate(
			application_name,
			transaction_date_time,
			subsidiary_transaction->process.credit_account_name );
	}

} /* post_change_account_balance_delete() */

LIST *post_change_account_balance_get_primary_data_list(
				char *asset_name,
				char *serial_number )
{
	LIST *primary_data_list = list_new();
	list_append_pointer( primary_data_list, asset_name );
	list_append_pointer( primary_data_list, serial_number );

	return primary_data_list;

} /* post_change_account_balance_get_primary_data_list() */

void account_balance_transaction_date_time_update(
				char *application_name,
				char *asset_name,
				char *serial_number,
				char *transaction_date_time,
				char *full_name,
				char *street_address )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *table_name;
	char *key;
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	if ( !transaction_date_time || !*transaction_date_time ) return;

	key = "asset_name,serial_number";

	table_name =
		get_table_name(
			application_name,
			ACCOUNT_BALANCE_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s	|"
		 "sql.e					 ",
		 table_name,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 "%s^%s|full_name=%s|street_address=%s|transaction_date_time=%s\n",
	 	escape_character(	buffer1,
					asset_name,
					'\'' ),
	 	serial_number,
	 	escape_character(	buffer2,
					full_name,
					'\'' ),
		street_address,
	 	transaction_date_time );

	pclose( output_pipe );

} /* account_balance_transaction_date_time_update() */

void post_change_account_balance_fetch_row(
			char **full_name,
			char **street_address,
			char **transaction_date_time,
			double *extension,
			char *application_name,
			char *asset_name,
			char *serial_number )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char buffer[ 128 ];
	char *results;
	char *select;
	char piece_buffer[ 128 ];

	select = "full_name,street_address,transaction_date_time,extension";

	sprintf( where,
		 "asset_name = '%s' and serial_number = '%s'",
	 	 escape_character(	buffer,
					asset_name,
					'\'' ),
		 serial_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
	*transaction_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 3 );
	*extension = atof( piece_buffer );

} /* post_change_account_balance_fetch_row() */

void post_change_prior_fixed_extension_update(
				char *application_name,
				char *asset_name,
				char *serial_number )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
	LIST *primary_data_list;
	char *full_name = {0};
	char *street_address = {0};
	char *transaction_date_time = {0};
	double extension = {0};

	post_change_account_balance_fetch_row(
			&full_name,
			&street_address,
			&transaction_date_time,
			&extension,
			application_name,
			asset_name,
			serial_number );

	if (	!full_name
	||	!street_address
	||	!transaction_date_time
	||	!*transaction_date_time )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot post_change_account_balance_fetch_row(%s/%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 asset_name,
			 serial_number );
		exit( 1 );
	}

	primary_data_list =
		post_change_account_balance_get_primary_data_list(
			asset_name,
			serial_number );

	subsidiary_transaction =
		subsidiary_new(	application_name,
				ACCOUNT_BALANCE_FOLDER_NAME,
				primary_data_list,
				full_name,
				street_address,
				extension /* transaction_amount */ );

	if ( !subsidiary_transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot build transaction for (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address );
		exit( 1 );
	}

	ledger_transaction_amount_update(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		extension,
		0.0 /* database_amount */ );

	ledger_debit_credit_update(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		subsidiary_transaction->process.debit_account_name,
		subsidiary_transaction->process.credit_account_name,
		extension /* transaction_amount  */ );

	ledger_propagate(
		application_name,
		transaction_date_time,
		subsidiary_transaction->process.debit_account_name );

	ledger_propagate(
		application_name,
		transaction_date_time,
		subsidiary_transaction->process.credit_account_name );

} /* post_change_account_balance_amount_update() */

void post_change_account_balance_update(
				char *application_name,
				char *asset_name,
				char *serial_number,
				char *preupdate_extension )
{
	enum preupdate_change_state extension_change_state;

	extension_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_extension,
			(char *)0 /* postupdate_data */,
			"preupdate_extension" );

	if (	extension_change_state ==
		from_something_to_something_else )
	{
		post_change_prior_fixed_extension_update(
			application_name,
			asset_name,
			serial_number );
	}


} /* post_change_account_balance_update() */

#endif

void post_change_account_balance_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time )
{
	ACCOUNT_BALANCE *account_balance;

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
	char *unrealized_gain = {0};
	char *realized_loss = {0};
	char *unrealized_loss = {0};
	char *checking_account = {0};
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	char *prior_date_time;
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char memo[ 128 ];

	ledger_get_investment_account_names(
		&investment_account,
		&fair_value_adjustment,
		&realized_gain,
		&unrealized_gain,
		&realized_loss,
		&unrealized_loss,
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
				unrealized_gain_balance );

	if ( timlib_double_virtually_same(
		new_account_balance->unrealized_gain_change, 0.0 ) )
	{
		return 1;
	}

	transaction =
		ledger_transaction_new(
			new_account_balance->full_name,
			new_account_balance->street_address,
			new_account_balance->date_time
				/* transaction_date_time */,
			strdup( format_initial_capital(
				   memo,
				   ACCOUNT_BALANCE_FOLDER_NAME ) ) );

	transaction->journal_ledger_list = list_new();

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
			new_account_balance->book_value_change;

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
			new_account_balance->book_value_change;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	if ( !timlib_double_virtually_same(
		new_account_balance->unrealized_gain_change, 0.0 ) )
	{
		if ( new_account_balance->unrealized_gain_change < 0.0 )
		{
			journal_ledger =
				journal_ledger_new(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					unrealized_loss );
	
			journal_ledger->debit_amount =
				0.0 -
				new_account_balance->unrealized_gain_change;
		}
		else
		{
			journal_ledger =
				journal_ledger_new(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					unrealized_gain );
	
			journal_ledger->credit_amount =
				new_account_balance->unrealized_gain_change;
		}
	
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	new_account_balance->transaction_date_time =
		ledger_transaction_journal_ledger_insert(
			application_name,
			new_account_balance->full_name,
			new_account_balance->street_address,
			new_account_balance->date_time,
			new_account_balance->book_value_change
				/* transaction_amount */,
			strdup( format_initial_capital(
					memo,
					ACCOUNT_BALANCE_FOLDER_NAME ) ),
			0 /* check_number */,
			1 /* lock_transaction */,
			transaction->journal_ledger_list );

	investment_account_balance_update(
		application_name,
		new_account_balance,
		account_balance );

	return 1;


	return 1;

} /* post_change_account_balance_insert_time_passage() */

void post_change_account_balance_insert_purchase(
				char *application_name,
				ACCOUNT_BALANCE *account_balance )
{
	char *investment_account = {0};
	char *fair_value_adjustment = {0};
	char *realized_gain = {0};
	char *unrealized_gain = {0};
	char *realized_loss = {0};
	char *unrealized_loss = {0};
	char *checking_account = {0};
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	char *prior_date_time;

	ledger_get_investment_account_names(
		&investment_account,
		&fair_value_adjustment,
		&realized_gain,
		&unrealized_gain,
		&realized_loss,
		&unrealized_loss,
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
				0.0 /* prior_unrealized_gain_balance */ );
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
					unrealized_gain_balance );
	}

	if ( new_account_balance->share_quantity_change > 0.0 )
	{
		char memo[ 128 ];

		new_account_balance->transaction_date_time =
			ledger_transaction_binary_insert(
				application_name,
				new_account_balance->full_name,
				new_account_balance->street_address,
				new_account_balance->date_time,
				new_account_balance->book_value_change
					/* transaction_amount */,
				strdup( format_initial_capital(
						memo,
						ACCOUNT_BALANCE_FOLDER_NAME ) ),
				0 /* check_number */,
				1 /* lock_transaction */,
				investment_account
					/* debit_account_name */,
				checking_account
					/* credit_account_name */ );
	}

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
	char *unrealized_gain = {0};
	char *realized_loss = {0};
	char *unrealized_loss = {0};
	char *checking_account = {0};
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	char *prior_date_time;
	char memo[ 128 ];
	JOURNAL_LEDGER *journal_ledger;
	TRANSACTION *transaction;

	ledger_get_investment_account_names(
		&investment_account,
		&fair_value_adjustment,
		&realized_gain,
		&unrealized_gain,
		&realized_loss,
		&unrealized_loss,
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
			prior_account_balance->
				unrealized_gain_balance );

	if ( new_account_balance->share_quantity_change >= 0.0 ) return 0;

	transaction =
		ledger_transaction_new(
			new_account_balance->full_name,
			new_account_balance->street_address,
			new_account_balance->date_time
				/* transaction_date_time */,
			strdup( format_initial_capital(
				   memo,
				   ACCOUNT_BALANCE_FOLDER_NAME ) ) );

	transaction->journal_ledger_list = list_new();

	/* Debit accounts */
	/* -------------- */
	journal_ledger =
		journal_ledger_new(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			checking_account );

	journal_ledger->debit_amount =
		new_account_balance->share_price *
		new_account_balance->share_quantity_change;

	list_append_pointer(
		transaction->journal_ledger_list,
		journal_ledger );

	/* If sale realized a gain/loss */
	/* ---------------------------- */
	if ( !timlib_double_virtually_same(
			new_account_balance->unrealized_gain_change, 0.0 ) )
	{
		if ( new_account_balance->unrealized_gain_change < 0.0 )
		{
			/* Credit account = realized_gain */
			/* ------------------------------ */
			journal_ledger =
				journal_ledger_new(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					realized_gain );

			journal_ledger->credit_amount =
				0.0 -
				new_account_balance->unrealized_gain_change;
		}
		else
		{
			/* Debit account = realized_loss */
			/* ----------------------------- */
			journal_ledger =
				journal_ledger_new(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					realized_loss );

			journal_ledger->debit_amount =
				new_account_balance->unrealized_gain_change;
		}
			
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
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
		new_account_balance->book_value_change;

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
			strdup( format_initial_capital(
					memo,
					ACCOUNT_BALANCE_FOLDER_NAME ) ),
			0 /* check_number */,
			1 /* lock_transaction */,
			transaction->journal_ledger_list );

	investment_account_balance_update(
		application_name,
		new_account_balance,
		account_balance );

	return 1;

} /* post_change_account_balance_insert_sale() */

