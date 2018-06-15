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
#include "fixed_asset.h"
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

DEPRECIATION *depreciation_new(
			char *asset_name,
			char *serial_number )
{
	DEPRECIATION *p = depreciation_calloc();

	p->asset_name = asset_name;
	p->serial_number = serial_number;

	return p;

} /* depreciation_new() */

boolean depreciation_date_exists(
			char *application_name,
			char *folder_name,
			char *depreciation_date )
{
	char sys_string[ 1024 ];
	char where[ 32 ];
	char *results;

	sprintf( where,
		 "depreciation_date = '%s'",
		 depreciation_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=count			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 folder_name,
		 where );

	results = pipe2string( sys_string );

	if ( atoi( results ) )
		return 1;
	else
		return 0;

} /* depreciation_date_exists() */

char *depreciation_fetch_max_depreciation_date(
			char *application_name,
			char *folder_name )
{
	char sys_string[ 1024 ];
	char *select;

	select = "max(depreciation_date)";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			",
		 application_name,
		 select,
		 folder_name );

	return pipe2string( sys_string );

} /* depreciation_fetch_max_depreciation_date() */

double depreciation_calculate_amount(
			char *depreciation_method,
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation,
			char *service_placement_date,
			int units_produced )
{
	if ( !depreciation_method || !*depreciation_method )
	{
		/* Land is not depreciated. */
		/* ------------------------ */
		return 0.0;
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
			finance_accumulated_depreciation );
	}
	else
	if ( strcmp( depreciation_method, "units_of_production" ) == 0 )
	{
		return depreciation_units_of_production_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_units,
			units_produced,
			finance_accumulated_depreciation );
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
			finance_accumulated_depreciation,
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
			finance_accumulated_depreciation,
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
			finance_accumulated_depreciation,
			service_placement_date );
	}
	else
	{
		return 0.0;
	}

} /* depreciation_calculate_amount() */

double depreciation_units_of_production_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_units,
			int units_produced,
			double finance_accumulated_depreciation )
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


	if (	finance_accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			finance_accumulated_depreciation;
	}

	return depreciation_amount;

} /* depreciation_units_of_production_get_amount() */

double depreciation_straight_line_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation )
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
	if (	finance_accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			finance_accumulated_depreciation;
	}

	return depreciation_amount;

} /* depreciation_straight_line_get_amount() */

double depreciation_sum_of_years_digits_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation,
			char *service_placement_date )
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
			service_placement_date );

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
	if (	finance_accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			finance_accumulated_depreciation;
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
			double finance_accumulated_depreciation,
			int n )
{
	double annual_depreciation_amount;
	double fraction_of_year;
	double depreciation_amount;
	double book_value;
	double maximum_depreciation;

	book_value = extension - finance_accumulated_depreciation;

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

void depreciation_fund_get_transaction(
		TRANSACTION **purchase_transaction,
		TRANSACTION **prior_transaction,
		char *full_name,
		char *street_address,
		char *depreciation_expense_account,
		char *accumulated_depreciation_account,
		double purchased_fixed_asset_depreciation_amount,
		double prior_fixed_asset_depreciation_amount )
{
	DATE *transaction_date_time;

	transaction_date_time = date_now_new( date_get_utc_offset() );

	/* Build the purchase transaction */
	/* ------------------------------ */
	*purchase_transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			date_display_yyyy_mm_dd_colon_hms(
				transaction_date_time ),
			DEPRECIATION_MEMO );

	(*purchase_transaction)->transaction_amount =
		purchased_fixed_asset_depreciation_amount;

	(*purchase_transaction)->journal_ledger_list =
		ledger_get_binary_ledger_list(
			purchased_fixed_asset_depreciation_amount,
			depreciation_expense_account
				/* debit_account */,
			accumulated_depreciation_account
				/* credit_account */ );

	date_increment_seconds(
		transaction_date_time,
		1 );

	/* Build the prior transaction */
	/* --------------------------- */
	*prior_transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			date_display_yyyy_mm_dd_colon_hms(
				transaction_date_time ),
			DEPRECIATION_MEMO );

	(*prior_transaction)->transaction_amount =
		prior_fixed_asset_depreciation_amount;

	(*prior_transaction)->journal_ledger_list =
		ledger_get_binary_ledger_list(
			prior_fixed_asset_depreciation_amount,
			depreciation_expense_account
				/* debit_account */,
			accumulated_depreciation_account
				/* credit_account */ );

} /* depreciation_fund_get_transaction() */

void depreciation_fixed_asset_list_set_depreciation(
			LIST *fixed_asset_list,
			double *purchased_fixed_asset_depreciation_amount,
			char *depreciation_date,
			char *prior_depreciation_date )
{
	FIXED_ASSET *fixed_asset;
	DEPRECIATION *depreciation;

	if ( !list_rewind( fixed_asset_list ) ) return;

	do {
		fixed_asset = list_get_pointer( fixed_asset_list );

		depreciation =
			depreciation_new(
				fixed_asset->asset_name,
				fixed_asset->serial_number );

		depreciation->depreciation_amount =
			depreciation_calculate_amount(
				fixed_asset->depreciation_method,
				fixed_asset->extension,
				fixed_asset->estimated_residual_value,
				fixed_asset->estimated_useful_life_years,
				fixed_asset->estimated_useful_life_units,
				fixed_asset->declining_balance_n,
				prior_depreciation_date,
				depreciation_date,
				fixed_asset->finance_accumulated_depreciation,
				fixed_asset->service_placement_date,
				0 /* units_produced */ );

		fixed_asset->finance_accumulated_depreciation +=
			depreciation->depreciation_amount;

		*purchased_fixed_asset_depreciation_amount +=
			depreciation->depreciation_amount;

		fixed_asset->depreciation = depreciation;

	} while( list_next( fixed_asset_list ) );

} /* depreciation_fixed_asset_list_set_depreciation() */

void depreciation_fund_list_table_display(
				char *process_name,
				LIST *depreciation_fund_list )
{
	DEPRECIATION_FUND *depreciation_fund;

	if ( !list_rewind( depreciation_fund_list ) ) return;

	do {
		depreciation_fund =
			list_get_pointer(
				depreciation_fund_list );

		depreciation_fixed_asset_list_table_display(
			process_name,
			depreciation_fund->
				fixed_asset_purchased_list );

		depreciation_fixed_asset_list_table_display(
			process_name,
			depreciation_fund->
				fixed_asset_prior_list );

	} while( list_next( depreciation_fund_list ) );

} /* depreciation_fund_list_table_display() */

void depreciation_fixed_asset_list_table_display(
				char *process_name,
				LIST *fixed_asset_list )
{
	FIXED_ASSET *fixed_asset;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *heading;
	char *justification;
	char buffer[ 128 ];

	if ( !list_rewind( fixed_asset_list ) ) return;

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
		fixed_asset = list_get_pointer( fixed_asset_list );

		fprintf(output_pipe,
			"^%s^%s^%.2lf^%.2lf",
			format_initial_capital(
				buffer, fixed_asset->asset_name ),
			fixed_asset->serial_number,
			fixed_asset->extension,
			fixed_asset->
				database_finance_accumulated_depreciation );

		fprintf(output_pipe,
			"^%.2lf",
			fixed_asset->depreciation->depreciation_amount );

		fprintf(output_pipe,
			"^%.2lf\n",
			fixed_asset->
				finance_accumulated_depreciation );

	} while( list_next( fixed_asset_list ) );

	pclose( output_pipe );

} /* depreciation_fixed_asset_list_table_display() */

LIST *depreciation_fetch_fund_list(
				char *application_name,
				char *depreciation_date,
				char *prior_depreciation_date,
				boolean with_load )
{
	DEPRECIATION_FUND *depreciation_fund;
	LIST *depreciation_fund_list = list_new();

	if ( ledger_fund_attribute_exists( application_name ) )
	{
		LIST *fund_name_list;
		char *fund_name;

		fund_name_list =
			ledger_fetch_fund_name_list(
				application_name );

		if ( !list_rewind( fund_name_list ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty fund_name_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		do {
			fund_name = list_get_pointer( fund_name_list );

			depreciation_fund =
				depreciation_fund_new(
					application_name,
					fund_name,
					depreciation_date,
					prior_depreciation_date,
					with_load );

			list_append_pointer(
				depreciation_fund_list,
				depreciation_fund );

		} while( list_next( fund_name_list ) );
	}
	else
	{
		depreciation_fund =
			depreciation_fund_new(
				application_name,
				(char *)0 /* fund_name */,
				depreciation_date,
				prior_depreciation_date,
				with_load );

		list_append_pointer(
			depreciation_fund_list,
			depreciation_fund );
	}

	return depreciation_fund_list;

} /* depreciation_fetch_fund_list() */

DEPRECIATION_FUND *depreciation_fund_new(
					char *application_name,
					char *fund_name,
					char *depreciation_date,
					char *prior_depreciation_date,
					boolean with_load )
{
	DEPRECIATION_FUND *d =
		(DEPRECIATION_FUND *)
			calloc( 1, sizeof( DEPRECIATION_FUND ) );

	if ( !d )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	d->fund_name = fund_name;

	/* Error with an exit if failure. */
	/* ------------------------------ */
	ledger_get_depreciation_account_names(
		&d->depreciation_expense_account,
		&d->accumulated_depreciation_account,
		application_name,
		fund_name );

	if ( with_load )
	{
		/* Purchase order fixed assets */
		/* --------------------------- */
		d->fixed_asset_purchased_list =
			fixed_asset_depreciation_purchase_fetch_list(
				application_name,
				fund_name );
	
		depreciation_fixed_asset_list_set_depreciation(
			d->fixed_asset_purchased_list,
			&d->purchased_fixed_asset_depreciation_amount,
			depreciation_date,
			prior_depreciation_date );
	
		/* Prior purchased fixed assets */
		/* ---------------------------- */
		d->fixed_asset_prior_list =
			fixed_asset_depreciation_prior_fetch_list(
				application_name,
				fund_name );
	
		depreciation_fixed_asset_list_set_depreciation(
			d->fixed_asset_prior_list,
			&d->prior_fixed_asset_depreciation_amount,
			depreciation_date,
			prior_depreciation_date );
	}

	return d;

} /* depreciation_fund_new() */

void depreciation_fund_list_set_transaction(
				LIST *depreciation_fund_list,
				char *full_name,
				char *street_address )
{
	DEPRECIATION_FUND *depreciation_fund;

	if ( !list_rewind( depreciation_fund_list ) ) return;

	do {
		depreciation_fund =
			list_get_pointer(
				depreciation_fund_list );

		depreciation_fund_get_transaction(
			&depreciation_fund->
				purchase_transaction,
			&depreciation_fund->
				prior_transaction,
			full_name,
			street_address,
			depreciation_fund->depreciation_expense_account,
			depreciation_fund->accumulated_depreciation_account,
			depreciation_fund->
				purchased_fixed_asset_depreciation_amount,
			depreciation_fund->
				prior_fixed_asset_depreciation_amount );

	} while( list_next( depreciation_fund_list ) );

} /* depreciation_fund_list_set_transaction() */

boolean depreciation_fund_list_execute(
				LIST *depreciation_fund_list,
				char *application_name,
				char *full_name,
				char *street_address )
{
	DEPRECIATION_FUND *depreciation_fund;
	boolean did_any = 0;

	if ( !list_rewind( depreciation_fund_list ) ) return 0;

	do {
		depreciation_fund =
			list_get_pointer(
				depreciation_fund_list );

		/* Insert the purchase fixed asset depreciations */
		/* --------------------------------------------- */
		depreciation_fund->
			purchase_transaction->
			transaction_date_time =
			     ledger_transaction_journal_ledger_insert(
				application_name,
				full_name,
				street_address,
				depreciation_fund->
					purchase_transaction->
					transaction_date_time,
				depreciation_fund->
					purchase_transaction->
					transaction_amount,
				depreciation_fund->
					purchase_transaction->
					memo,
				0 /* check_number */,
				1 /* lock_transaction */,
				depreciation_fund->
					purchase_transaction->
					journal_ledger_list );

		if ( depreciation_fixed_asset_list_execute(
			depreciation_fund->
				fixed_asset_purchased_list,
			"fixed_asset_depreciation",
			full_name,
			street_address,
			depreciation_fund->
				purchase_transaction->
				transaction_date_time ) )
		{
			did_any = 1;
		}

		/* Insert the prior fixed asset depreciations */
		/* ------------------------------------------ */
		depreciation_fund->
			prior_transaction->
			transaction_date_time =
			     ledger_transaction_journal_ledger_insert(
				application_name,
				full_name,
				street_address,
				depreciation_fund->
					prior_transaction->
					transaction_date_time,
				depreciation_fund->
					prior_transaction->
					transaction_amount,
				depreciation_fund->
					prior_transaction->
					memo,
				0 /* check_number */,
				1 /* lock_transaction */,
				depreciation_fund->
					prior_transaction->
					journal_ledger_list );

		if ( depreciation_fixed_asset_list_execute(
			depreciation_fund->
				fixed_asset_prior_list,
			"prior_fixed_asset_depreciation",
			full_name,
			street_address,
			depreciation_fund->
				prior_transaction->
				transaction_date_time ) )
		{
			did_any = 1;
		}

	} while( list_next( depreciation_fund_list ) );

	return did_any;

} /* depreciation_fund_list_execute() */

boolean depreciation_fixed_asset_list_execute(
				LIST *fixed_asset_list,
				char *folder_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	FIXED_ASSET *fixed_asset;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *field;

	if ( !list_rewind( fixed_asset_list ) ) return 0;

	field =
"asset_name,serial_number,full_name,street_address,transaction_date_time,depreciation_date,depreciation_amount";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s del='^'",
		 folder_name,
		 field );
		 
	output_pipe = popen( sys_string, "w" );

	do {
		fixed_asset = list_get_pointer( fixed_asset_list );

		fprintf(output_pipe,
			"%s^%s^%s^%s^%s^%s^%.2lf\n",
			fixed_asset->asset_name,
			fixed_asset->serial_number,
			full_name,
			street_address,
			transaction_date_time,
			fixed_asset->depreciation->depreciation_date,
			fixed_asset->depreciation->depreciation_amount );

	} while( list_next( fixed_asset_list ) );

	pclose( output_pipe );

	return 1;

} /* depreciation_fixed_asset_list_execute() */

