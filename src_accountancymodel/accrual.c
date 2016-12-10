/* -------------------------------------------------------------------- */
/* src_accountancymodel/accrual.c					*/
/* -------------------------------------------------------------------- */
/* This is the appaserver accrual ADT.					*/
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
#include "accrual.h"

ACCRUAL *accrual_calloc( void )
{
	ACCRUAL *p =
		(ACCRUAL *)
			calloc( 1, sizeof( ACCRUAL ) );

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
} /* accrual_calloc() */

char *accrual_get_select( void )
{
	char *select =
"full_name,street_address,purchase_date_time,asset_name,accrual_date,accrual_amount,transaction_date_time";

	return select;
}

ACCRUAL *accrual_parse(	char *application_name,
					char *input_buffer )
{
	ACCRUAL *accrual;
	char piece_buffer[ 256 ];

	accrual = accrual_calloc();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	accrual->full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	accrual->street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	accrual->purchase_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	accrual->asset_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	accrual->accrual_date = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *piece_buffer )
		accrual->accrual_amount =
		accrual->database_accrual_amount =
			atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *piece_buffer )
		accrual->transaction_date_time =
		accrual->database_transaction_date_time =
			strdup( piece_buffer );

	if ( accrual->transaction_date_time )
	{
		accrual->transaction =
			ledger_transaction_fetch(
				application_name,
				accrual->full_name,
				accrual->street_address,
				accrual->transaction_date_time );
	}

	return accrual;

} /* accrual_parse() */

ACCRUAL *accrual_fetch(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date )
{
	char sys_string[ 1024 ];
	char *ledger_where;
	char buffer[ 128 ];
	char where[ 256 ];
	char *select;
	char *results;

	select = accrual_get_select();

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( where,
"%s and asset_name = '%s' and accrual_date = '%s'",
		 ledger_where,
		 escape_character(	buffer,
					asset_name,
					'\'' ),
		 accrual_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=accrual		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		return (ACCRUAL *)0;
	}

	return accrual_parse( application_name, results );

} /* accrual_fetch() */

void accrual_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date,
			double accrual_amount,
			double database_accrual_amount,
			char *transaction_date_time,
			char *database_transaction_date_time )
{
	char *sys_string;
	FILE *output_pipe;

	sys_string = accrual_get_update_sys_string( application_name );
	output_pipe = popen( sys_string, "w" );

	if ( !dollar_virtually_same(
			accrual_amount,
			database_accrual_amount ) )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^%s^%s^accrual_amount^%.2lf\n",
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			accrual_date,
			accrual_amount );
	}

	if ( timlib_strcmp(	transaction_date_time,
				database_transaction_date_time ) != 0 )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^%s^%s^transaction_date_time^%s\n",
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			accrual_date,
			(transaction_date_time)
				? transaction_date_time
				: "" );
	}

	pclose( output_pipe );

} /* accrual_update() */

char *accrual_get_update_sys_string(
				char *application_name )
{
	static char sys_string[ 256 ];
	char *table_name;
	char *key;

	table_name = get_table_name( application_name, "accrual" );
	key =
"full_name,street_address,purchase_date_time,asset_name,accrual_date";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	return sys_string;

} /* accrual_get_update_sys_string() */

double accrual_get_amount(
			double extension,
			double accrual_period_years,
			char *prior_accrual_date_string,
			char *accrual_date_string,
			double accumulated_accrual )
{
	double annual_accrual_amount;
	double fraction_of_year;
	double accrual_amount;

	fraction_of_year =
		ledger_get_fraction_of_year(
			prior_accrual_date_string,
			accrual_date_string );

	if ( accrual_period_years )
	{
		annual_accrual_amount =
			extension /
			(double)accrual_period_years;
	}
	else
	{
		annual_accrual_amount = 0.0;
	}

	accrual_amount = annual_accrual_amount * fraction_of_year;

	/* If finished accruing */
	/* -------------------- */
	if ( accumulated_accrual + accrual_amount > extension )
	{
		accrual_amount =
			extension -
			accumulated_accrual;
	}

	return accrual_amount;

} /* accrual_get_amount() */

#ifdef NOT_DEFINED
/* Returns propagate_account_list */
/* ------------------------------ */
LIST *accrual_journal_ledger_refresh(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double accrual_amount )
{
	char *accrual_expense_account;
	char *accumulated_accrual_account;
	LIST *propagate_account_list = {0};
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	ledger_get_accrual_account_names(
		&accrual_expense_account,
		&accumulated_accrual_account,
		application_name,
		fund_name );

	if ( accrual_amount )
	{
		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			accrual_expense_account,
			accrual_amount,
			1 /* is_debit */ );

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			accumulated_accrual_account,
			accrual_amount,
			0 /* not is_debit */ );
	}

	propagate_account_list = list_new();

	account = ledger_account_new( accrual_expense_account );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				accrual_expense_account );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			accrual_expense_account );

	list_append_pointer( propagate_account_list, account );

	account = ledger_account_new( accumulated_accrual_account );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				accumulated_accrual_account );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			accumulated_accrual_account );

	list_append_pointer( propagate_account_list, account );

	return propagate_account_list;

} /* accrual_journal_ledger_refresh() */
#endif

LIST *accrual_fetch_list(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name )
{
	ACCRUAL *accrual;
	char *select;
	LIST *accrual_list;
	char sys_string[ 1024 ];
	char *ledger_where;
	char buffer[ 128 ];
	char where[ 256 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];

	select = accrual_get_select();

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( where,
		 "%s and asset_name = '%s'",
		 ledger_where,
		 escape_character(	buffer,
					asset_name,
					'\'' ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=prepaid_asset_accrual	"
		 "			where=\"%s\"			"
		 "			order=accrual_date		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	accrual_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		accrual =
			accrual_parse(
				application_name,
				input_buffer );

		list_append_pointer( accrual_list, accrual );
	}

	pclose( input_pipe );

	return accrual_list;

} /* accrual_fetch_list() */

/* Returns new accumulated_accrual */
/* ------------------------------------ */
double accrual_list_set(
		LIST *accrual_list,
		char *purchase_date,
		double extension,
		double accrual_period_years )
{
	double accumulated_accrual;
	ACCRUAL *accrual;
	char *prior_accrual_date_string = {0};

	if ( !list_rewind( accrual_list ) ) return 0.0;

	accumulated_accrual = 0.0;
	prior_accrual_date_string = purchase_date;

	do {
		accrual = list_get( accrual_list );

		accrual->accrual_amount =
			accrual_get_amount(
				extension,
				accrual_period_years,
				prior_accrual_date_string,
				accrual->accrual_date,
				accumulated_accrual );

		accumulated_accrual +=
			accrual->accrual_amount;

		prior_accrual_date_string =
			accrual->accrual_date;

	} while( list_next( accrual_list ) );

	return accumulated_accrual;

} /* accrual_list_set() */

void accrual_list_update(
		LIST *accrual_list,
		char *application_name,
		char *fund_name )
{
	ACCRUAL *accrual;
	char *propagate_transaction_date_time = {0};

	if ( !list_rewind( accrual_list ) ) return;

	do {
		accrual = list_get( accrual_list );

		if ( !accrual->transaction )
		{
			accrual->transaction_date_time =
				ledger_get_transaction_date_time(
					accrual->accrual_date );
	
			accrual->transaction =
				ledger_transaction_new(
					accrual->full_name,
					accrual->street_address,
					accrual->transaction_date_time,
					ACCRUAL_MEMO );
	
			ledger_transaction_insert(
				application_name,
				accrual->transaction->full_name,
				accrual->transaction->street_address,
				accrual->
					transaction->
					transaction_date_time,
				accrual->accrual_amount,
				accrual->transaction->memo,
				0 /* check_number */,
				1 /* lock_transaction */ );
		}

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
				accrual->transaction_date_time;
		}

		accrual_update(
			application_name,
			accrual->full_name,
			accrual->street_address,
			accrual->purchase_date_time,
			accrual->asset_name,
			accrual->accrual_date,
			accrual->accrual_amount,
			accrual->database_accrual_amount,
			accrual->transaction_date_time,
			accrual->database_transaction_date_time );

/*
	char *accrual_expense_account;
	char *accumulated_accrual_account;
		accrual_journal_ledger_refresh(
			application_name,
			fund_name,
			accrual->transaction->full_name,
			accrual->transaction->street_address,
			accrual->
				transaction->
				transaction_date_time,
			accrual->accrual_amount );
*/

	} while( list_next( accrual_list ) );

/*
	ledger_get_accrual_account_names(
		&accrual_expense_account,
		&accumulated_accrual_account,
		application_name,
		fund_name );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		accrual_expense_account );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		accumulated_accrual_account );
*/

} /* accrual_list_update() */

void accrual_list_delete(
			LIST *accrual_list,
			char *application_name,
			char *fund_name )
{
	ACCRUAL *accrual;
	char *propagate_transaction_date_time = {0};

	if ( !list_rewind( accrual_list ) ) return;

	do {
		accrual = list_get( accrual_list );

		accrual_delete(
			application_name,
			accrual->full_name,
			accrual->street_address,
			accrual->purchase_date_time,
			accrual->asset_name,
			accrual->accrual_date );

		if ( !accrual->transaction )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: expecting a transaction = (%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	accrual->transaction_date_time );
			exit( 1 );
		}

		ledger_delete(
			application_name,
			TRANSACTION_FOLDER_NAME,
			accrual->transaction->full_name,
			accrual->transaction->street_address,
			accrual->transaction->transaction_date_time );

		ledger_delete(
			application_name,
			LEDGER_FOLDER_NAME,
			accrual->transaction->full_name,
			accrual->transaction->street_address,
			accrual->transaction->transaction_date_time );

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
				accrual->
					transaction->
					transaction_date_time;
		}

	} while( list_next( accrual_list ) );

/*
	char *accrual_expense_account;
	char *accumulated_accrual_account;
	ledger_get_accrual_account_names(
		&accrual_expense_account,
		&accumulated_accrual_account,
		application_name,
		fund_name );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		accrual_expense_account );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		accumulated_accrual_account );
*/

} /* accrual_list_delete() */

void accrual_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;
	char *table_name;
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	field=
"full_name,street_address,purchase_date_time,asset_name,accrual_date";

	table_name =
		get_table_name(
			application_name,
			"prepaid_asset_accrual" );

	sprintf( sys_string,
		 "delete_statement table=%s field=%s delimiter='^'	|"
		 "sql.e							 ",
		 table_name,
		 field );

	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
			"%s^%s^%s^%s^%s\n",
		 	escape_character(	buffer1,
						full_name,
						'\'' ),
			street_address,
			purchase_date_time,
		 	escape_character(	buffer2,
						asset_name,
						'\'' ),
			accrual_date );

	pclose( output_pipe );

} /* accrual_delete() */

ACCRUAL *accrual_list_seek(
			LIST *accrual_list,
			char *accrual_date )
{
	ACCRUAL *accrual;

	if ( !list_rewind( accrual_list ) ) return (ACCRUAL *)0;

	do {
		accrual = list_get( accrual_list );

		if ( strcmp(	accrual->accrual_date,
				accrual_date ) == 0 )
		{
			return accrual;
		}

	} while( list_next( accrual_list ) );

	return (ACCRUAL *)0;

} /* accrual_list_seek() */

