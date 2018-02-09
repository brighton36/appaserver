/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/depreciation.c			*/
/* -------------------------------------------------------------------- */
/* This is the appaserver depreciation ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "appaserver_library.h"
#include "ledger.h"
#include "entity.h"
#include "purchase.h"
#include "column.h"
#include "depreciation.h"

DEPRECIATION *depreciation_calloc( void )
{
	DEPRECIATION *p =
		(DEPRECIATION *)
			calloc( 1, sizeof( DEPRECIATION ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;

} /* depreciation_calloc() */

char *depreciation_get_select( void )
{
	char *select =
"full_name,street_address,purchase_date_time,asset_name,serial_number,depreciation_date,units_produced,depreciation_amount,transaction_date_time";

	return select;
}

DEPRECIATION *depreciation_parse(	char *application_name,
					char *input_buffer )
{
	DEPRECIATION *depreciation;
	char piece_buffer[ 256 ];

	depreciation = depreciation_calloc();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	depreciation->full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	depreciation->street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	depreciation->purchase_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	depreciation->asset_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	depreciation->serial_number = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	depreciation->depreciation_date = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *piece_buffer )
		depreciation->units_produced =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	if ( *piece_buffer )
		depreciation->depreciation_amount =
		depreciation->database_depreciation_amount =
			atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	if ( *piece_buffer )
		depreciation->transaction_date_time =
		depreciation->database_transaction_date_time =
			strdup( piece_buffer );

	if ( depreciation->transaction_date_time )
	{
		depreciation->transaction =
			ledger_transaction_fetch(
				application_name,
				depreciation->full_name,
				depreciation->street_address,
				depreciation->transaction_date_time );
	}

	return depreciation;

} /* depreciation_parse() */

boolean depreciation_date_prior_exists(
			char *application_name,
			char *depreciation_date )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *folder;
	char *results;

	folder = "prior_fixed_asset_depreciation";

	sprintf( where,
		 "depreciation_date = '%s'",
		 depreciation_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=count			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( results )
		return atoi( results );
	else
		return 0;

} /* depreciation_date_prior_exists() */

boolean depreciation_date_exists(
			char *application_name,
			char *fund_name,
			char *depreciation_date )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *fund_where;
	char *folder;
	char *results;

	folder = "purchase_order,depreciation";

	fund_where =
		ledger_get_fund_where(
			application_name,
			fund_name );

	sprintf( where,
"depreciation.full_name = purchase_order.full_name and depreciation.street_address = purchase_order.street_address and depreciation.purchase_date_time = purchase_order.purchase_date_time and %s and depreciation_date = '%s'",
		 fund_where,
		 depreciation_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=count			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( results )
		return atoi( results );
	else
		return 0;

} /* depreciation_date_exists() */

char *depreciation_prior_fetch_max_depreciation_date(
			char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;

	select = "max(depreciation_date)";

	folder = "prior_fixed_asset_depreciation";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			",
		 application_name,
		 select,
		 folder );

	return pipe2string( sys_string );

} /* depreciation_prior_fetch_max_depreciation_date() */

char *depreciation_fetch_max_depreciation_date(
			char *application_name,
			char *fund_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char *fund_where;
	char where[ 512 ];

	select = "max(depreciation_date)";

	folder = "purchase_order,depreciation";

	fund_where =
		ledger_get_fund_where(
			application_name,
			fund_name );

	sprintf( where,
"depreciation.full_name = purchase_order.full_name and depreciation.street_address = purchase_order.street_address and depreciation.purchase_date_time = purchase_order.purchase_date_time and %s",
		 fund_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	return pipe2string( sys_string );

} /* depreciation_fetch_max_depreciation_date() */

char *depreciation_fetch_prior_depreciation_date(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date )
{
	char sys_string[ 1024 ];
	char *ledger_where;
	char buffer[ 128 ];
	char where[ 256 ];
	char *select;

	select = "max(depreciation_date)";

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( where,
"%s and asset_name = '%s' and serial_number = '%s' and depreciation_date < '%s'",
		 ledger_where,
		 escape_character(	buffer,
					asset_name,
					'\'' ),
		 serial_number,
		 depreciation_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=depreciation		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	return pipe2string( sys_string );

} /* depreciation_fetch_prior_depreciation_date() */

DEPRECIATION *depreciation_fetch(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date )
{
	char sys_string[ 1024 ];
	char *ledger_where;
	char buffer[ 128 ];
	char where[ 256 ];
	char *select;
	char *results;

	select = depreciation_get_select();

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( where,
"%s and asset_name = '%s' and serial_number = '%s' and depreciation_date = '%s'",
		 ledger_where,
		 escape_character(	buffer,
					asset_name,
					'\'' ),
		 serial_number,
		 depreciation_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=depreciation		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		return (DEPRECIATION *)0;
	}

	return depreciation_parse( application_name, results );

} /* depreciation_fetch() */

void depreciation_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date,
			double depreciation_amount,
			double database_depreciation_amount,
			char *transaction_date_time,
			char *database_transaction_date_time )
{
	char *sys_string;
	FILE *output_pipe;

	sys_string = depreciation_get_update_sys_string( application_name );
	output_pipe = popen( sys_string, "w" );

	if ( !dollar_virtually_same(
			depreciation_amount,
			database_depreciation_amount ) )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^%s^%s^%s^depreciation_amount^%.2lf\n",
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			serial_number,
			depreciation_date,
			depreciation_amount );
	}

	if ( timlib_strcmp(	transaction_date_time,
				database_transaction_date_time ) != 0 )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^%s^%s^%s^transaction_date_time^%s\n",
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			serial_number,
			depreciation_date,
			(transaction_date_time)
				? transaction_date_time
				: "" );
	}

	pclose( output_pipe );

} /* depreciation_update() */

char *depreciation_get_update_sys_string(
				char *application_name )
{
	static char sys_string[ 256 ];
	char *table_name;
	char *key;

	table_name = get_table_name( application_name, "depreciation" );
	key =
"full_name,street_address,purchase_date_time,asset_name,serial_number,depreciation_date";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	return sys_string;

} /* depreciation_get_update_sys_string() */

double depreciation_get_amount(
			char *depreciation_method,
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double accumulated_depreciation,
			char *arrived_date_string,
			int units_produced )
{
	if ( !depreciation_method || !*depreciation_method )
	{
		/* Land is not depreciated. */
		/* ------------------------ */
		return 0.0;
/*
		return depreciation_straight_line_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			accumulated_depreciation );
*/
	}
	else
	if ( strcmp( depreciation_method, "straight_line" ) == 0 )
	{
		return depreciation_straight_line_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			accumulated_depreciation );
	}
	else
	if ( strcmp( depreciation_method, "units_of_production" ) == 0 )
	{
		return depreciation_units_of_production_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_units,
			units_produced,
			accumulated_depreciation );
	}
	else
	if ( strcmp( depreciation_method, "double_declining_balance" ) == 0 )
	{
		return depreciation_n_declining_balance_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			accumulated_depreciation,
			2 /* n */ );
	}
	else
	if ( strcmp( depreciation_method, "n_declining_balance" ) == 0 )
	{
		return depreciation_n_declining_balance_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			accumulated_depreciation,
			declining_balance_n );
	}
	else
	if ( strcmp( depreciation_method, "sum_of_years_digits" ) == 0 )
	{
		return depreciation_sum_of_years_digits_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			accumulated_depreciation,
			arrived_date_string );
	}
	else
	{
		return 0.0;
	}

} /* depreciation_get_amount() */

double depreciation_units_of_production_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_units,
			int units_produced,
			double accumulated_depreciation )
{
	double depreciation_rate_per_unit = 0.0;
	double depreciation_base;
	double depreciation_amount;

	depreciation_base = extension - (double)estimated_residual_value;

	if ( estimated_useful_life_units )
	{
		depreciation_rate_per_unit =
			depreciation_base /
			(double)estimated_useful_life_units;
	}

	depreciation_amount = 
		depreciation_rate_per_unit *
		(double)units_produced;


	if (	accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			accumulated_depreciation;
	}

	return depreciation_amount;

} /* depreciation_units_of_production_get_amount() */

double depreciation_straight_line_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double accumulated_depreciation )
{
	double depreciation_base;
	double annual_depreciation_amount;
	double fraction_of_year = 0.0;
	double depreciation_amount;

	if ( prior_depreciation_date_string
	&&   *prior_depreciation_date_string )
	{
		fraction_of_year =
			ledger_get_fraction_of_year(
				prior_depreciation_date_string,
				depreciation_date_string );
	}

	depreciation_base = extension - (double)estimated_residual_value;

	if ( estimated_useful_life_years )
	{
		annual_depreciation_amount =
			depreciation_base /
			(double) estimated_useful_life_years;
	}
	else
	{
		annual_depreciation_amount = 0.0;
	}

	depreciation_amount = annual_depreciation_amount * fraction_of_year;

	/* If depreciation_date is past the useful life. */
	/* --------------------------------------------- */
	if (	accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			accumulated_depreciation;
	}

	return depreciation_amount;

} /* depreciation_straight_line_get_amount() */

double depreciation_sum_of_years_digits_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double accumulated_depreciation,
			char *arrived_date_string )
{
	double denominator;
	double depreciation_base;
	double annual_depreciation_amount;
	double fraction_of_year;
	double depreciation_fraction;
	double depreciation_amount;
	int current_age_years;
	int remaining_life_years;

	if ( estimated_useful_life_years <= 0 ) return 0.0;

	depreciation_base = extension - (double)estimated_residual_value;

	denominator =
		( (double)( estimated_useful_life_years *
			  ( estimated_useful_life_years + 1 ) ) )
			  / 2.0;

	current_age_years =
		date_years_between(
			depreciation_date_string,
			arrived_date_string );

	remaining_life_years = estimated_useful_life_years - current_age_years;

	if ( remaining_life_years < 0 ) remaining_life_years = 0;

	depreciation_fraction =
		(double)remaining_life_years /
		(double)denominator;

	annual_depreciation_amount =
		depreciation_base *
		depreciation_fraction;

	fraction_of_year =
		ledger_get_fraction_of_year(
			prior_depreciation_date_string,
			depreciation_date_string );

	depreciation_amount = annual_depreciation_amount * fraction_of_year;

	/* If depreciation_date is past the useful life. */
	/* --------------------------------------------- */
	if (	accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			accumulated_depreciation;
	}

	return depreciation_amount;

} /* depreciation_sum_of_years_digits_get_amount() */

/* Returns propagate_account_list */
/* ------------------------------ */
LIST *depreciation_journal_ledger_refresh(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double depreciation_amount )
{
	char *depreciation_expense_account = {0};
	char *accumulated_depreciation_account = {0};
	LIST *propagate_account_list = {0};
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	/* Error with an exit if failure. */
	/* ------------------------------ */
	ledger_get_depreciation_account_names(
		&depreciation_expense_account,
		&accumulated_depreciation_account,
		application_name,
		fund_name );

	if ( depreciation_amount )
	{
		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			depreciation_expense_account,
			depreciation_amount,
			1 /* is_debit */ );

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			accumulated_depreciation_account,
			depreciation_amount,
			0 /* not is_debit */ );
	}

	propagate_account_list = list_new();

	account = ledger_account_new( depreciation_expense_account );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				depreciation_expense_account );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			depreciation_expense_account );

	list_append_pointer( propagate_account_list, account );

	account = ledger_account_new( accumulated_depreciation_account );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				accumulated_depreciation_account );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			accumulated_depreciation_account );

	list_append_pointer( propagate_account_list, account );

	return propagate_account_list;

} /* depreciation_journal_ledger_refresh() */

double depreciation_n_declining_balance_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double accumulated_depreciation,
			int n )
{
	double annual_depreciation_amount;
	double fraction_of_year;
	double depreciation_amount;
	double book_value;
	double maximum_depreciation;

	book_value = extension - accumulated_depreciation;

	fraction_of_year =
		ledger_get_fraction_of_year(
			prior_depreciation_date_string,
			depreciation_date_string );

	annual_depreciation_amount =
		( book_value * (double)n ) /
			       (double)estimated_useful_life_years;

	depreciation_amount = annual_depreciation_amount * fraction_of_year;

	maximum_depreciation = book_value - (double)estimated_residual_value;

	if ( depreciation_amount > maximum_depreciation )
		depreciation_amount = maximum_depreciation;

	return depreciation_amount;

} /* depreciation_n_declining_balance_get_amount() */

LIST *depreciation_fetch_list(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number )
{
	DEPRECIATION *depreciation;
	char *select;
	LIST *depreciation_list;
	char sys_string[ 1024 ];
	char *ledger_where;
	char buffer[ 128 ];
	char where[ 256 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];

	select = depreciation_get_select();

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( where,
		 "%s and asset_name = '%s' and serial_number = '%s'",
		 ledger_where,
		 escape_character(	buffer,
					asset_name,
					'\'' ),
		 serial_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=depreciation		"
		 "			where=\"%s\"			"
		 "			order=depreciation_date		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	depreciation_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		depreciation =
			depreciation_parse(
				application_name,
				input_buffer );

		list_append_pointer( depreciation_list, depreciation );
	}

	pclose( input_pipe );

	return depreciation_list;

} /* depreciation_fetch_list() */

/* Returns new accumulated_depreciation */
/* ------------------------------------ */
double depreciation_list_set(
		LIST *depreciation_list,
		char *depreciation_method,
		double extension,
		int estimated_residual_value,
		int estimated_useful_life_years,
		int estimated_useful_life_units,
		int declining_balance_n,
		char *arrived_date_string )
{
	double accumulated_depreciation;
	DEPRECIATION *depreciation;
	char *prior_depreciation_date_string;

	if ( !list_rewind( depreciation_list ) ) return 0.0;

	accumulated_depreciation = 0.0;
	prior_depreciation_date_string = arrived_date_string;

	do {
		depreciation = list_get( depreciation_list );

		depreciation->depreciation_amount =
			depreciation_get_amount(
				depreciation_method,
				extension,
				estimated_residual_value,
				estimated_useful_life_years,
				estimated_useful_life_units,
				declining_balance_n,
				prior_depreciation_date_string,
				depreciation->depreciation_date,
				accumulated_depreciation,
				arrived_date_string,
				depreciation->units_produced );

		if ( timlib_dollar_virtually_same(
			depreciation->depreciation_amount,
			0.0 ) )
		{
			continue;
		}

		accumulated_depreciation +=
			depreciation->depreciation_amount;

		prior_depreciation_date_string =
			depreciation->depreciation_date;

	} while( list_next( depreciation_list ) );

	return accumulated_depreciation;

} /* depreciation_list_set() */

void depreciation_list_update_and_transaction_propagate(
		LIST *depreciation_list,
		char *application_name,
		char *fund_name )
{
	DEPRECIATION *depreciation;
	char *depreciation_expense_account;
	char *accumulated_depreciation_account;
	char *propagate_transaction_date_time = {0};

	if ( !list_rewind( depreciation_list ) ) return;

	do {
		depreciation = list_get( depreciation_list );

		if ( timlib_dollar_virtually_same(
			depreciation->depreciation_amount,
			0.0 ) )
		{
			continue;
		}

		if ( !depreciation->transaction )
		{
			depreciation->transaction_date_time =
				ledger_get_transaction_date_time(
					depreciation->depreciation_date );
	
			depreciation->transaction =
				ledger_transaction_new(
					depreciation->full_name,
					depreciation->street_address,
					depreciation->transaction_date_time,
					DEPRECIATION_MEMO );
	
			depreciation->transaction_date_time =
			depreciation->transaction->transaction_date_time =
			ledger_transaction_insert(
				application_name,
				depreciation->transaction->full_name,
				depreciation->transaction->street_address,
				depreciation->
					transaction->
					transaction_date_time,
				depreciation->depreciation_amount,
				depreciation->transaction->memo,
				0 /* check_number */,
				1 /* lock_transaction */ );
		}

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
				depreciation->transaction_date_time;
		}

		depreciation_update(
			application_name,
			depreciation->full_name,
			depreciation->street_address,
			depreciation->purchase_date_time,
			depreciation->asset_name,
			depreciation->serial_number,
			depreciation->depreciation_date,
			depreciation->depreciation_amount,
			depreciation->database_depreciation_amount,
			depreciation->transaction_date_time,
			depreciation->database_transaction_date_time );

		depreciation_journal_ledger_refresh(
			application_name,
			fund_name,
			depreciation->transaction->full_name,
			depreciation->transaction->street_address,
			depreciation->
				transaction->
				transaction_date_time,
			depreciation->depreciation_amount );

	} while( list_next( depreciation_list ) );

	/* Error with an exit if failure. */
	/* ------------------------------ */
	ledger_get_depreciation_account_names(
		&depreciation_expense_account,
		&accumulated_depreciation_account,
		application_name,
		fund_name );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		depreciation_expense_account );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		accumulated_depreciation_account );

} /* depreciation_list_update_and_transaction_propagate() */

void depreciation_list_delete(
			LIST *depreciation_list,
			char *application_name,
			char *fund_name )
{
	DEPRECIATION *depreciation;
	char *depreciation_expense_account;
	char *accumulated_depreciation_account;
	char *propagate_transaction_date_time = {0};

	if ( !list_rewind( depreciation_list ) ) return;

	do {
		depreciation = list_get( depreciation_list );

		depreciation_delete(
			application_name,
			depreciation->full_name,
			depreciation->street_address,
			depreciation->purchase_date_time,
			depreciation->asset_name,
			depreciation->serial_number,
			depreciation->depreciation_date );

		if ( !depreciation->transaction )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: expecting a transaction = (%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	depreciation->transaction_date_time );
			exit( 1 );
		}

		ledger_delete(
			application_name,
			TRANSACTION_FOLDER_NAME,
			depreciation->transaction->full_name,
			depreciation->transaction->street_address,
			depreciation->transaction->transaction_date_time );

		ledger_delete(
			application_name,
			LEDGER_FOLDER_NAME,
			depreciation->transaction->full_name,
			depreciation->transaction->street_address,
			depreciation->transaction->transaction_date_time );

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
				depreciation->
					transaction->
					transaction_date_time;
		}

	} while( list_next( depreciation_list ) );

	/* Error with an exit if failure. */
	/* ------------------------------ */
	ledger_get_depreciation_account_names(
		&depreciation_expense_account,
		&accumulated_depreciation_account,
		application_name,
		fund_name );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		depreciation_expense_account );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		accumulated_depreciation_account );

} /* depreciation_list_delete() */

void depreciation_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;
	char *table_name;
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	field=
"full_name,street_address,purchase_date_time,asset_name,serial_number,depreciation_date";

	table_name = get_table_name( application_name, "depreciation" );

	sprintf( sys_string,
		 "delete_statement table=%s field=%s delimiter='^'	|"
		 "sql.e							 ",
		 table_name,
		 field );

	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
			"%s^%s^%s^%s^%s^%s\n",
		 	escape_character(	buffer1,
						full_name,
						'\'' ),
			street_address,
			purchase_date_time,
		 	escape_character(	buffer2,
						asset_name,
						'\'' ),
			serial_number,
			depreciation_date );

	pclose( output_pipe );

} /* depreciation_delete() */

DEPRECIATION *depreciation_list_seek(
			LIST *depreciation_list,
			char *depreciation_date )
{
	DEPRECIATION *depreciation;

	if ( !list_rewind( depreciation_list ) ) return (DEPRECIATION *)0;

	do {
		depreciation = list_get( depreciation_list );

		if ( strcmp(	depreciation->depreciation_date,
				depreciation_date ) == 0 )
		{
			return depreciation;
		}

	} while( list_next( depreciation_list ) );

	return (DEPRECIATION *)0;

} /* depreciation_list_seek() */

char *deprecation_get_prior_depreciation_date(
				LIST *depreciation_list )
{
	DEPRECIATION *depreciation;
	char *prior_depreciation_date;

	if ( !list_length( depreciation_list )
	||   list_at_head( depreciation_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty list or at beginning of list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	list_previous( depreciation_list );
	depreciation = list_get( depreciation_list );
	prior_depreciation_date = depreciation->depreciation_date;

	return prior_depreciation_date;

} /* deprecation_get_prior_depreciation_date() */

LIST *depreciation_get_depreciable_fixed_purchase_record_list(
				char *application_name,
				char *fund_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "depreciate_select.sh %s '%s'",
		 application_name,
		 fund_name );

	return pipe2list( sys_string );

} /* depreciation_get_depreciable_fixed_purchase_record_list() */

void depreciation_fetch_purchase_fixed_asset_depreciation_list(
				LIST *depreciable_fixed_asset_purchase_list,
				char *application_name )
{
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;

	if ( !list_rewind( depreciable_fixed_asset_purchase_list ) )
		return;

	do {
		purchase_fixed_asset =
			list_get_pointer(
				depreciable_fixed_asset_purchase_list );

		purchase_fixed_asset->depreciation_list =
			depreciation_fetch_list(
				application_name,
				purchase_fixed_asset->full_name,
				purchase_fixed_asset->street_address,
				purchase_fixed_asset->
					purchase_date_time,
				purchase_fixed_asset->asset_name,
				purchase_fixed_asset->serial_number );

	} while( list_next( depreciable_fixed_asset_purchase_list ) );

} /* depreciation_fetch_purchase_fixed_asset_depreciation_list() */

void depreciation_fixed_asset_purchase_list_table_display(
				FILE *output_pipe,
				char *full_name,
				char *street_address,
				double depreciation_amount,
				LIST *depreciable_fixed_asset_purchase_list )
{
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	DEPRECIATION *depreciation;
	char buffer[ 1024 ];

	if ( !list_rewind( depreciable_fixed_asset_purchase_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty depreciable_fixed_asset_purchase_list for (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address );
		return;
	}

	do {
		purchase_fixed_asset =
			list_get_pointer(
				depreciable_fixed_asset_purchase_list );

		fprintf(output_pipe,
			"%s/%s",
			full_name,
			street_address );

		fprintf(output_pipe,
			"^%.2lf",
			depreciation_amount );

		fprintf(output_pipe,
			"^%s/%s",
			format_initial_capital(
				buffer, purchase_fixed_asset->asset_name ),
			purchase_fixed_asset->serial_number );

		fprintf(output_pipe,
			"^%.2lf",
			purchase_fixed_asset->extension );

		fprintf(output_pipe,
			"^%.2lf",
			purchase_fixed_asset->
				database_accumulated_depreciation );

		if ( list_length( purchase_fixed_asset->
					depreciation_list ) != 1 )
		{
			fprintf( output_pipe, "^Error occurred" );
		}
		else
		do {
			depreciation =
				list_get_first_pointer(
					purchase_fixed_asset->
						depreciation_list );

			fprintf(output_pipe,
				"^%.2lf",
				depreciation->depreciation_amount );

		} while( list_next( purchase_fixed_asset->depreciation_list ) );

		fprintf(output_pipe,
			"^%.2lf\n",
			purchase_fixed_asset->accumulated_depreciation );

	} while( list_next( depreciable_fixed_asset_purchase_list ) );

} /* depreciation_fixed_asset_purchase_list_table_display() */

void depreciation_fixed_asset_purchase_list_tree_display(
				LIST *depreciable_fixed_asset_purchase_list )
{
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	DEPRECIATION *depreciation;

	if ( !list_rewind( depreciable_fixed_asset_purchase_list ) )
		return;

	do {
		purchase_fixed_asset =
			list_get_pointer(
				depreciable_fixed_asset_purchase_list );

		printf( "%c%cFixed asset = %s/%s:\n",
			9, 9,
			purchase_fixed_asset->asset_name,
			purchase_fixed_asset->serial_number );

		printf( "%c%c%cExtension = %.2lf\n",
			9, 9, 9,
			purchase_fixed_asset->extension );

		printf( "%c%c%cDatabase accumulated depreciation = %.2lf\n",
			9, 9, 9,
			purchase_fixed_asset->
				database_accumulated_depreciation );

		if ( list_length( purchase_fixed_asset->
					depreciation_list ) != 1 )
		{
			printf( "Error occurred.\n" );
		}
		else
		do {
			depreciation =
				list_get_first_pointer(
					purchase_fixed_asset->
						depreciation_list );

			printf( "%c%c%cDepreciation amount = %.2lf\n",
				9, 9, 9,
				depreciation->depreciation_amount );

		} while( list_next( purchase_fixed_asset->depreciation_list ) );

		printf( "%c%c%cAccumulated depreciation = %.2lf\n",
			9, 9, 9,
			purchase_fixed_asset->accumulated_depreciation );

	} while( list_next( depreciable_fixed_asset_purchase_list ) );

} /* depreciation_fixed_asset_purchase_list_tree_display() */

void depreciation_fixed_asset_depreciation_table_display(
				char *process_name,
				LIST *entity_list )
{
	ENTITY *entity;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *heading;
	char *justification;
	char buffer[ 128 ];

	if ( !list_rewind( entity_list ) ) return;

	heading =
"Entity,Entity Depreciation,Asset,Extension,Prior Accumulated,Depreciation,Post Accumulated";

	justification = "left,right,left,right";

	sprintf( sys_string,
		 "group_trim.e '^' 2			|"
		 "html_table.e '%s' '%s' '^' '%s'	 ",
		 format_initial_capital( buffer, process_name ),
		 heading,
		 justification );
		 
	output_pipe = popen( sys_string, "w" );

	do {
		entity = list_get_pointer( entity_list );

		depreciation_fixed_asset_purchase_list_table_display(
			output_pipe,
			entity->full_name,
			entity->street_address,
			entity->depreciation_amount,
			entity->depreciable_fixed_asset_purchase_list );

	} while( list_next( entity_list ) );

	pclose( output_pipe );

} /* depreciation_fixed_asset_depreciation_table_display() */

void depreciation_fixed_asset_depreciation_tree_display(
				LIST *entity_list )
{
	ENTITY *entity;

	if ( !list_rewind( entity_list ) ) return;

	do {
		entity = list_get_pointer( entity_list );

		printf( "Entity = %s/%s:\n",
			entity->full_name,
			entity->street_address );

		printf( "%cdepreciation_amount = %.2lf\n",
			9,
			entity->depreciation_amount );

		depreciation_fixed_asset_purchase_list_tree_display(
			entity->depreciable_fixed_asset_purchase_list );

	} while( list_next( entity_list ) );

} /* depreciation_fixed_asset_depreciation_tree_display() */

void depreciation_fixed_asset_entity_set_depreciation(
				double *entity_depreciation_amount,
				LIST *depreciable_fixed_asset_purchase_list,
				char *depreciation_date )
{
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	DEPRECIATION *depreciation;
	char arrived_date[ 16 ];

	*entity_depreciation_amount = 0.0;

	if ( !list_rewind( depreciable_fixed_asset_purchase_list ) )
		return;

	do {
		purchase_fixed_asset =
			list_get_pointer(
				depreciable_fixed_asset_purchase_list );

		if ( strcmp(
			purchase_fixed_asset->depreciation_method,
			"units_of_production" ) == 0 )
		{
			continue;
		}

		if ( purchase_fixed_asset->depreciation_list )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: expecting empty depreciation_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		purchase_fixed_asset->depreciation_list = list_new();

		depreciation = depreciation_calloc();

		depreciation->full_name = purchase_fixed_asset->full_name;

		depreciation->street_address =
			purchase_fixed_asset->street_address;

		depreciation->purchase_date_time =
			purchase_fixed_asset->purchase_date_time;

		depreciation->asset_name =
			purchase_fixed_asset->asset_name;

		depreciation->serial_number =
			purchase_fixed_asset->serial_number;

		depreciation->depreciation_date = depreciation_date;

		depreciation->depreciation_amount =
			depreciation_get_amount(
				purchase_fixed_asset->depreciation_method,
				purchase_fixed_asset->extension,
				purchase_fixed_asset->estimated_residual_value,
				purchase_fixed_asset->
					estimated_useful_life_years,
				purchase_fixed_asset->
					estimated_useful_life_units,
				purchase_fixed_asset->declining_balance_n,
				purchase_fixed_asset->prior_depreciation_date,
				depreciation->depreciation_date,
				purchase_fixed_asset->accumulated_depreciation,
				column( arrived_date,
					0, 
					purchase_fixed_asset->
						arrived_date_time ),
				0 /* units_produced */ );

		purchase_fixed_asset->accumulated_depreciation +=
			depreciation->depreciation_amount;

		*entity_depreciation_amount +=
			depreciation->depreciation_amount;

		list_append_pointer(
			purchase_fixed_asset->depreciation_list,
			depreciation );

	} while( list_next( depreciable_fixed_asset_purchase_list ) );

} /* depreciation_fixed_asset_entity_set_depreciation() */

void depreciation_fixed_asset_set_depreciation(
				LIST *entity_list,
				char *depreciation_date )
{
	ENTITY *entity;

	if ( !list_rewind( entity_list ) ) return;

	do {
		entity = list_get_pointer( entity_list );

		depreciation_fixed_asset_entity_set_depreciation(
			&entity->depreciation_amount,
			entity->depreciable_fixed_asset_purchase_list,
			depreciation_date );

	} while( list_next( entity_list ) );

} /* depreciation_fixed_asset_set_depreciation() */

LIST *depreciation_get_depreciable_fixed_asset_purchase_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *fund_name,
				char *depreciation_date )
{
	static LIST *depreciable_fixed_asset_purchase_record_list = {0};
	LIST *depreciable_fixed_asset_purchase_list;
	char *record;
	char local_full_name[ 128 ];
	char local_street_address[ 128 ];
	char buffer[ 128 ];
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	boolean found_one = 0;

	if ( !depreciable_fixed_asset_purchase_record_list )
	{
		depreciable_fixed_asset_purchase_record_list =
			depreciation_get_depreciable_fixed_purchase_record_list(
				application_name,
				fund_name );
	}

	if ( !list_rewind( depreciable_fixed_asset_purchase_record_list ) )
		return (LIST *)0;

	depreciable_fixed_asset_purchase_list = list_new();

	do {
		record =
			list_get_pointer(
				depreciable_fixed_asset_purchase_record_list );

		piece(	local_full_name,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	local_street_address,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		if ( strcmp( local_full_name, full_name ) == 0
		&&   strcmp( local_street_address, street_address ) == 0 )
		{
			purchase_fixed_asset = purchase_fixed_asset_new();

			purchase_fixed_asset->full_name = full_name;
			purchase_fixed_asset->street_address = street_address;

/*
select="full_name,street_address,purchase_date_time,asset_name,serial_number,estimated_useful_life_years,estimated_useful_life_units,estimated_residual_value,declining_balance_n,depreciation_method,accumualated_depreciation,arrived_date_time,extension"
*/
			piece( buffer, FOLDER_DATA_DELIMITER, record, 2 );

			purchase_fixed_asset->purchase_date_time =
				strdup( buffer );

			piece( buffer, FOLDER_DATA_DELIMITER, record, 3 );

			purchase_fixed_asset->asset_name = strdup( buffer );

			piece( buffer, FOLDER_DATA_DELIMITER, record, 4 );

			purchase_fixed_asset->serial_number = strdup( buffer );

			piece( buffer, FOLDER_DATA_DELIMITER, record, 5 );

			purchase_fixed_asset->estimated_useful_life_years =
				atoi( buffer );

			piece( buffer, FOLDER_DATA_DELIMITER, record, 6 );

			purchase_fixed_asset->estimated_useful_life_units =
				atoi( buffer );

			piece( buffer, FOLDER_DATA_DELIMITER, record, 7 );

			purchase_fixed_asset->estimated_residual_value =
				atof( buffer );

			piece( buffer, FOLDER_DATA_DELIMITER, record, 8 );

			purchase_fixed_asset->declining_balance_n =
				atoi( buffer );

			piece( buffer, FOLDER_DATA_DELIMITER, record, 9 );

			purchase_fixed_asset->depreciation_method =
				strdup( buffer );

			piece( buffer, FOLDER_DATA_DELIMITER, record, 10 );

			purchase_fixed_asset->accumulated_depreciation =
			purchase_fixed_asset->
				database_accumulated_depreciation =
					atof( buffer );

			piece( buffer, FOLDER_DATA_DELIMITER, record, 11 );

			purchase_fixed_asset->arrived_date_time =
				strdup( buffer );

			piece( buffer, FOLDER_DATA_DELIMITER, record, 12 );

			purchase_fixed_asset->extension =
				atof( buffer );

			purchase_fixed_asset->prior_depreciation_date =
				depreciation_fetch_prior_depreciation_date(
					application_name,
					purchase_fixed_asset->full_name,
					purchase_fixed_asset->street_address,
					purchase_fixed_asset->
						purchase_date_time,
					purchase_fixed_asset->asset_name,
					purchase_fixed_asset->serial_number,
					depreciation_date );

			list_append_pointer(
				depreciable_fixed_asset_purchase_list,
				purchase_fixed_asset );

			found_one = 1;
		}
		else
		{
			if ( found_one )
			{
				return depreciable_fixed_asset_purchase_list;
			}
		}

	} while( list_next( depreciable_fixed_asset_purchase_record_list ) );

	return depreciable_fixed_asset_purchase_list;

} /* depreciation_get_depreciable_fixed_asset_purchase_list() */

LIST *depreciation_fixed_asset_get_entity_list(
			char *application_name,
			char *fund_name,
			char *depreciation_date )
{
	LIST *entity_list;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char full_name[ 128 ];
	char street_address[ 128 ];
	ENTITY *entity;

	sprintf( sys_string,
		 "depreciate_entity_select.sh %s '%s'",
		 application_name,
		 fund_name );

	input_pipe = popen( sys_string, "r" );

	entity_list = list_new();

	while( timlib_get_line( input_buffer, input_pipe, 1024 ) )
	{
		piece( full_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( street_address, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		entity = entity_new(	strdup( full_name ),
					strdup( street_address ) );

		entity->depreciable_fixed_asset_purchase_list =
			depreciation_get_depreciable_fixed_asset_purchase_list(
				application_name,
				entity->full_name,
				entity->street_address,
				fund_name,
				depreciation_date );

		list_append_pointer( entity_list, entity );
	}

	pclose( input_pipe );

	return entity_list;

} /* depreciation_fixed_asset_get_entity_list() */

DEPRECIATE_FIXED_ASSET *depreciation_fixed_asset_depreciation_new(
				char *application_name,
				char *fund_name,
				char *depreciation_date )
{
	DEPRECIATE_FIXED_ASSET *p =
		(DEPRECIATE_FIXED_ASSET *)
			calloc( 1, sizeof( DEPRECIATE_FIXED_ASSET ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	p->entity_list =
		depreciation_fixed_asset_get_entity_list(
			application_name,
			fund_name,
			depreciation_date );

	return p;

} /* depreciation_fixed_asset_depreciation_new() */

void depreciation_fixed_asset_set_transaction(
				LIST *entity_list )
{
	ENTITY *entity;
	DATE *transaction_date_time;

	transaction_date_time = date_now_new( date_get_utc_offset() );

	if ( !list_rewind( entity_list ) ) return;

	do {
		entity = list_get_pointer( entity_list );

		entity->depreciation_transaction =
			ledger_transaction_new(
				entity->full_name,
				entity->street_address,
				date_display_yyyy_mm_dd_colon_hms(
					transaction_date_time ),
				DEPRECIATION_MEMO );

		date_increment_seconds(
			transaction_date_time,
			1,
			date_get_utc_offset() );

	} while( list_next( entity_list ) );

} /* depreciation_fixed_asset_set_transaction() */

void depreciation_fixed_asset_insert_depreciation(
				FILE *output_pipe,
				char *full_name,
				char *street_address,
				LIST *depreciable_fixed_asset_purchase_list,
				char *depreciation_date,
				char *transaction_date_time )
{
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	DEPRECIATION *depreciation;

	if ( !list_rewind( depreciable_fixed_asset_purchase_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: for (%s/%s), empty depreciable_fixed_asset_purchase_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address );
		return;
	}

	do {
		purchase_fixed_asset =
			list_get_pointer(
				depreciable_fixed_asset_purchase_list );

		if ( list_length( purchase_fixed_asset->
					depreciation_list ) != 1 )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: for (%s/%s), invalid list_length = %d.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	full_name,
			 	street_address,
				list_length( purchase_fixed_asset->
						depreciation_list ) );
			return;
		}

		depreciation =
			list_get_first_pointer(
				purchase_fixed_asset->
					depreciation_list );

		fprintf( output_pipe,
			 "%s^%s^%s^%s^%s^%s^%.2lf^%s\n",
			 full_name,
			 street_address,
			 purchase_fixed_asset->purchase_date_time,
			 purchase_fixed_asset->asset_name,
			 purchase_fixed_asset->serial_number,
			 depreciation_date,
			 depreciation->depreciation_amount,
			 transaction_date_time );

	} while( list_next( depreciable_fixed_asset_purchase_list ) );

} /* depreciation_fixed_asset_insert_depreciation() */

void depreciation_fixed_asset_insert_depreciation_entity_list(
				LIST *entity_list,
				char *depreciation_date )
{
	ENTITY *entity;
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;

	field = "full_name,street_address,purchase_date_time,asset_name,serial_number,depreciation_date,depreciation_amount,transaction_date_time";

	sprintf( sys_string,
		 "insert_statement.e table=depreciation field='%s' del='^' |"
		 "sql.e",
		 field );

	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( entity_list ) ) return;

	do {
		entity = list_get_pointer( entity_list );

		depreciation_fixed_asset_insert_depreciation(
				output_pipe,
				entity->full_name,
				entity->street_address,
				entity->depreciable_fixed_asset_purchase_list,
				depreciation_date,
				entity->
					depreciation_transaction->
					transaction_date_time );

	} while( list_next( entity_list ) );

	pclose( output_pipe );

} /* depreciation_fixed_asset_insert_depreciation_entity_list() */

void depreciation_fixed_asset_insert_transaction_entity_list(
				char *application_name,
				LIST *entity_list )
{
	ENTITY *entity;
	FILE *output_pipe;

	output_pipe =
		ledger_transaction_insert_open_stream(
			application_name );

	if ( !list_rewind( entity_list ) ) return;

	do {
		entity = list_get_pointer( entity_list );

		if ( !entity->depreciation_transaction )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: expecting a transaction = (%s/%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	entity->full_name,
				entity->street_address );
			exit( 1 );
		}

		ledger_transaction_insert_stream(
			output_pipe,
			entity->full_name,
			entity->street_address,
			entity->
				depreciation_transaction->
				transaction_date_time,
			entity->depreciation_amount
				/* transaction_amount */,
			entity->depreciation_transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */ );

	} while( list_next( entity_list ) );

	pclose( output_pipe );

} /* depreciation_fixed_asset_insert_transaction_entity_list() */

void depreciation_fixed_asset_insert_ledger_entity_list(
				char *application_name,
				char *fund_name,
				LIST *entity_list )
{
	char *depreciation_expense_account = {0};
	char *accumulated_depreciation_account = {0};
	ENTITY *entity;
	FILE *debit_account_pipe = {0};
	FILE *credit_account_pipe = {0};
	char *propagate_transaction_date_time = {0};

	if ( !list_rewind( entity_list ) ) return;

	/* Error with an exit if failure. */
	/* ------------------------------ */
	ledger_get_depreciation_account_names(
		&depreciation_expense_account,
		&accumulated_depreciation_account,
		application_name,
		fund_name );

	ledger_journal_insert_open_stream(
		&debit_account_pipe,
		&credit_account_pipe,
		application_name );

	do {
		entity = list_get_pointer( entity_list );

		if ( !entity->depreciation_transaction )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: expecting a transaction = (%s/%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	entity->full_name,
				entity->street_address );
			exit( 1 );
		}

		ledger_journal_insert_stream(
			debit_account_pipe,
			credit_account_pipe,
			entity->full_name,
			entity->street_address,
			entity->
				depreciation_transaction->
				transaction_date_time,
			entity->depreciation_amount,
			depreciation_expense_account
				/* debit_account_name */,
			accumulated_depreciation_account
				/* credit_account_name */ );

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
				entity->
					depreciation_transaction->
					transaction_date_time;
		}
	} while( list_next( entity_list ) );

	pclose( debit_account_pipe );
	pclose( credit_account_pipe );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		depreciation_expense_account );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		accumulated_depreciation_account );

} /* depreciation_fixed_asset_insert_ledger_entity_list() */

void depreciation_fixed_asset_update_accumulated_depreciation(
				char *application_name,
				LIST *entity_list )
{
	ENTITY *entity;
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	FILE *update_pipe;

	if ( !list_rewind( entity_list ) ) return;

	update_pipe =
		purchase_fixed_asset_get_update_pipe(
			application_name );

	do {
		entity = list_get_pointer( entity_list );

		if ( !list_rewind(
			entity->
				depreciable_fixed_asset_purchase_list ) )
		{
			fprintf( stderr,
"Warning in %s/%s()/%d: for (%s/%s), empty depreciable_fixed_asset_purchase_list.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	entity->full_name,
			 	entity->street_address );
			continue;
		}

		do {
			purchase_fixed_asset =
				list_get_pointer(
					entity->
					depreciable_fixed_asset_purchase_list );

			if ( double_virtually_same(
					purchase_fixed_asset->
					   accumulated_depreciation,
					purchase_fixed_asset->
					   database_accumulated_depreciation ) )
			{
				continue;
			}

			purchase_fixed_asset_update_stream(
				update_pipe,
				purchase_fixed_asset->full_name,
				purchase_fixed_asset->street_address,
				purchase_fixed_asset->purchase_date_time,
				purchase_fixed_asset->asset_name,
				purchase_fixed_asset->serial_number,
				purchase_fixed_asset->
					accumulated_depreciation );

		} while( list_next( 
				entity->
				depreciable_fixed_asset_purchase_list ) );

	} while( list_next( entity_list ) );

	pclose( update_pipe );

} /* depreciation_fixed_asset_update_accumulated_depreciation() */

void depreciation_fixed_asset_execute(
				LIST *entity_list,
				char *application_name,
				char *fund_name,
				char *depreciation_date )
{
	depreciation_fixed_asset_insert_depreciation_entity_list(
			entity_list,
			depreciation_date );

	depreciation_fixed_asset_insert_transaction_entity_list(
			application_name,
			entity_list );

	depreciation_fixed_asset_insert_ledger_entity_list(
			application_name,
			fund_name,
			entity_list );

	depreciation_fixed_asset_update_accumulated_depreciation(
			application_name,
			entity_list );

} /* depreciation_fixed_asset_execute() */

DEPRECIATE_PRIOR_FIXED_ASSET *depreciate_prior_fixed_asset_new( void )
{
	DEPRECIATE_PRIOR_FIXED_ASSET *p =
		(DEPRECIATE_PRIOR_FIXED_ASSET *)
			calloc( 1, sizeof( DEPRECIATE_PRIOR_FIXED_ASSET ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;

} /* depreciate_fixed_asset_depreciation_new() */

char *depreciate_prior_fixed_asset_get_select( void )
{
	char *select =
"asset_name,serial_number,recorded_date,extension,estimated_useful_life_years,estimated_useful_life_units,estimated_residual_value,declining_balance_n,depreciation_method,accumulated_depreciation";

	return select;
}

DEPRECIATE_PRIOR_FIXED_ASSET *depreciate_prior_fixed_asset_parse(
					char *input_buffer )
{
	DEPRECIATE_PRIOR_FIXED_ASSET *depreciate_prior_fixed_asset;
	char piece_buffer[ 256 ];

	depreciate_prior_fixed_asset = depreciate_prior_fixed_asset_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	depreciate_prior_fixed_asset->asset_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	depreciate_prior_fixed_asset->serial_number = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *piece_buffer )
		depreciate_prior_fixed_asset->recorded_date =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *piece_buffer )
		depreciate_prior_fixed_asset->extension =
			atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	if ( *piece_buffer )
		depreciate_prior_fixed_asset->estimated_useful_life_years =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *piece_buffer )
		depreciate_prior_fixed_asset->estimated_useful_life_units =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *piece_buffer )
		depreciate_prior_fixed_asset->estimated_residual_value =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	if ( *piece_buffer )
		depreciate_prior_fixed_asset->declining_balance_n =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	if ( *piece_buffer )
		depreciate_prior_fixed_asset->depreciation_method =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
	if ( *piece_buffer )
		depreciate_prior_fixed_asset->accumulated_depreciation =
		depreciate_prior_fixed_asset->
			database_accumulated_depreciation =
			atof( piece_buffer );

	return depreciate_prior_fixed_asset;

} /* depreciate_prior_fixed_asset_parse() */

LIST *depreciate_prior_fixed_asset_get_list(
					char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char input_buffer[ 2048 ];
	FILE *input_pipe;
	DEPRECIATE_PRIOR_FIXED_ASSET *depreciate_prior_fixed_asset;
	LIST *depreciate_prior_fixed_asset_list;

	select = depreciate_prior_fixed_asset_get_select();

	folder = "prior_fixed_asset";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			",
		 application_name,
		 select,
		 folder );

	input_pipe = popen( sys_string, "r" );

	depreciate_prior_fixed_asset_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		depreciate_prior_fixed_asset =
			depreciate_prior_fixed_asset_parse(
				input_buffer );

		list_append_pointer(	depreciate_prior_fixed_asset_list,
					depreciate_prior_fixed_asset );
	}

	pclose( input_pipe );
	return depreciate_prior_fixed_asset_list;

} /* depreciate_prior_fixed_asset_get_list() */

DEPRECIATE_PRIOR_FIXED_ASSET_DEPRECIATION *
	depreciate_prior_fixed_asset_depreciation_new(
			char *application_name,
			char *self_full_name,
			char *self_street_address )
{
	DEPRECIATE_PRIOR_FIXED_ASSET_DEPRECIATION *p =
		(DEPRECIATE_PRIOR_FIXED_ASSET_DEPRECIATION *)
			calloc( 1,
				sizeof(
				DEPRECIATE_PRIOR_FIXED_ASSET_DEPRECIATION ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	p->self_full_name = self_full_name;
	p->self_street_address = self_street_address;

	p->depreciate_prior_fixed_asset_list =
		depreciate_prior_fixed_asset_get_list(
			application_name );

	return p;

} /* depreciate_prior_fixed_asset_depreciation_new() */

void depreciation_prior_fixed_asset_table_display(
				char *process_name,
				LIST *depreciate_prior_fixed_asset_list )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *heading;
	char *justification;
	char buffer[ 128 ];
	DEPRECIATE_PRIOR_FIXED_ASSET *depreciate_prior_fixed_asset;

	if ( !list_rewind( depreciate_prior_fixed_asset_list ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: empty depreciate_prior_fixed_asset_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	heading =
"Asset,Serial,Extension,Prior Accumulated,Depreciation,Post Accumulated";

	justification = "left,left,right";

	sprintf( sys_string,
		 "html_table.e '%s' '%s' '^' '%s'",
		 format_initial_capital( buffer, process_name ),
		 heading,
		 justification );

	output_pipe = popen( sys_string, "w" );

	do {
		depreciate_prior_fixed_asset =
			list_get_pointer(
				depreciate_prior_fixed_asset_list );

		fprintf(output_pipe,
			"%s^%s",
			format_initial_capital(
				buffer,
				depreciate_prior_fixed_asset->asset_name ),
			depreciate_prior_fixed_asset->serial_number );

		fprintf(output_pipe,
			"^%.2lf",
			depreciate_prior_fixed_asset->extension );

		fprintf(output_pipe,
			"^%.2lf",
			depreciate_prior_fixed_asset->
				database_accumulated_depreciation );

		fprintf(output_pipe,
			"^%.2lf",
			depreciate_prior_fixed_asset->
				depreciation_amount );

		fprintf(output_pipe,
			"^%.2lf\n",
			depreciate_prior_fixed_asset->
				accumulated_depreciation );

	} while( list_next( depreciate_prior_fixed_asset_list ) );

	pclose( output_pipe );

} /* depreciation_prior_fixed_asset_table_display() */

void depreciation_prior_fixed_asset_set_depreciation(
				double *depreciation_amount,
				LIST *depreciate_prior_fixed_asset_list,
				char *prior_depreciation_date,
				char *depreciation_date )
{
	DEPRECIATE_PRIOR_FIXED_ASSET *depreciate_prior_fixed_asset;
	char *local_prior_depreciation_date;

	if ( !list_rewind( depreciate_prior_fixed_asset_list ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: empty depreciate_prior_fixed_asset_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	*depreciation_amount = 0.0;

	do {
		depreciate_prior_fixed_asset =
			list_get_pointer(
				depreciate_prior_fixed_asset_list );

		if ( timlib_strcmp(
			depreciate_prior_fixed_asset->depreciation_method,
			"units_of_production" ) == 0 )
		{
			continue;
		}

		if ( prior_depreciation_date
		&&   *prior_depreciation_date )
		{
			local_prior_depreciation_date =
				prior_depreciation_date;
		}
		else
		{
			local_prior_depreciation_date =
				depreciate_prior_fixed_asset->
					recorded_date;
		}

		depreciate_prior_fixed_asset->depreciation_amount =
			depreciation_get_amount(
				depreciate_prior_fixed_asset->
					depreciation_method,
				depreciate_prior_fixed_asset->extension,
				depreciate_prior_fixed_asset->
					estimated_residual_value,
				depreciate_prior_fixed_asset->
					estimated_useful_life_years,
				depreciate_prior_fixed_asset->
					estimated_useful_life_units,
				depreciate_prior_fixed_asset->
					declining_balance_n,
				local_prior_depreciation_date,
				depreciation_date,
				depreciate_prior_fixed_asset->
					accumulated_depreciation,
				depreciate_prior_fixed_asset->
					recorded_date,
				0 /* units_produced */ );

		depreciate_prior_fixed_asset->accumulated_depreciation +=
			depreciate_prior_fixed_asset->depreciation_amount;

		*depreciation_amount +=
			depreciate_prior_fixed_asset->
				depreciation_amount;

	} while( list_next( depreciate_prior_fixed_asset_list ) );

} /* depreciation_prior_fixed_asset_set_depreciation() */

void depreciation_prior_fixed_asset_insert_depreciation(
			char *full_name,
			char *street_address,
			LIST *depreciate_prior_fixed_asset_list,
			char *depreciation_date,
			char *transaction_date_time )
{
	DEPRECIATE_PRIOR_FIXED_ASSET *
		depreciate_prior_fixed_asset;
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;
	char *table;

	if ( !list_rewind( depreciate_prior_fixed_asset_list ) )
	{
		return;
	}

	field = "full_name,street_address,asset_name,serial_number,depreciation_date,depreciation_amount,transaction_date_time";

	table = "prior_fixed_asset_depreciation";

	sprintf( sys_string,
		 "insert_statement.e table=%s field='%s' del='^' |"
		 "sql.e",
		 table,
		 field );

	output_pipe = popen( sys_string, "w" );

	do {
		depreciate_prior_fixed_asset =
			list_get_pointer(
				depreciate_prior_fixed_asset_list );

		fprintf( output_pipe,
			 "%s^%s^%s^%s^%s^%.2lf^%s\n",
			 full_name,
			 street_address,
			 depreciate_prior_fixed_asset->asset_name,
			 depreciate_prior_fixed_asset->serial_number,
			 depreciation_date,
			 depreciate_prior_fixed_asset->depreciation_amount,
			 transaction_date_time );

	} while( list_next( depreciate_prior_fixed_asset_list ) );

	pclose( output_pipe );

} /* depreciation_prior_fixed_asset_insert_depreciation() */

