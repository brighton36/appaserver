/* -------------------------------------------------------------------- */
/* src_accountancymodel/depreciation.c					*/
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
#include "column.h"
#include "date.h"
#include "appaserver_library.h"
#include "ledger.h"
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
		return depreciation_straight_line_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			accumulated_depreciation );
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
	double fraction_of_year;
	double depreciation_amount;

	fraction_of_year =
		depreciation_get_fraction_of_year(
			prior_depreciation_date_string,
			depreciation_date_string );

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
		depreciation_get_fraction_of_year(
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
	char *depreciation_expense_account;
	char *accumulated_depreciation_account;
	LIST *propagate_account_list = {0};
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

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

double depreciation_get_fraction_of_year(
				char *prior_depreciation_date_string,
				char *depreciation_date_string )
{
	int days_between;
	DATE *prior_depreciation_date;
	DATE *depreciation_date;

	prior_depreciation_date =
		date_yyyy_mm_dd_new(
			prior_depreciation_date_string );

	depreciation_date =
		date_yyyy_mm_dd_new(
			depreciation_date_string );

	days_between =
		date_subtract_days(
			depreciation_date /* later_date */,
			prior_depreciation_date /* earlier_date */ );

	return (double)days_between / 365.0;

} /* depreciation_get_fraction_of_year() */

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
		depreciation_get_fraction_of_year(
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

		accumulated_depreciation +=
			depreciation->depreciation_amount;

		prior_depreciation_date_string =
			depreciation->depreciation_date;

	} while( list_next( depreciation_list ) );

	return accumulated_depreciation;

} /* depreciation_list_set() */

void depreciation_list_update(
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

} /* depreciation_list_update() */

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


