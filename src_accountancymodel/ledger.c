/* -------------------------------------------------------------------- */
/* src_accountancymodel/ledger.c					*/
/* -------------------------------------------------------------------- */
/* This is the AccountancyModel ledger ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "application_constants.h"
#include "list.h"
#include "column.h"
#include "date_convert.h"
#include "document.h"
#include "application.h"
#include "ledger.h"
#include "inventory.h"

ELEMENT *ledger_element_new( char *element_name )
{
	return ledger_new_element( element_name );
}

ELEMENT *ledger_new_element( char *element_name )
{
	ELEMENT *element;

	if ( ! ( element = calloc( 1, sizeof( ELEMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	element->element_name = element_name;
	return element;

} /* ledger_new_element() */

ELEMENT *ledger_account_fetch_element(	char *application_name,
					char *account_name )
{
	ELEMENT *element;
	char element_name[ 128 ];
	char accumulate_debit_yn[ 8 ];
	char where[ 512 ];
	char select[ 128 ];
	char sys_string[ 1024 ];
	char buffer[ 128 ];
	char *account_table;
	char *subclassification_table;
	char *element_table;
	char *results;

	account_table =
		get_table_name(
			application_name, ACCOUNT_FOLDER_NAME );

	subclassification_table =
		get_table_name(
			application_name, SUBCLASSIFICATION_FOLDER_NAME );

	element_table =
		get_table_name(
			application_name, ELEMENT_FOLDER_NAME );

	sprintf(	select,
			"%s.element,accumulate_debit_yn",
			element_table );

	sprintf( where,
		 "%s.subclassification = %s.subclassification and	"
		 "%s.element = %s.element and				"
		 "%s.account = '%s'					",
		 account_table,
		 subclassification_table,
		 subclassification_table,
		 element_table,
		 account_table,
		 escape_character(	buffer,
					account_name,
					'\'' ) );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s,%s where %s;\" | sql.e",
		 select,
		 account_table,
		 subclassification_table,
		 element_table,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (ELEMENT *)0;

	piece(	element_name,
		FOLDER_DATA_DELIMITER,
		results,
		0 );

	element = ledger_element_new( strdup( element_name ) );

	piece(	accumulate_debit_yn,
		FOLDER_DATA_DELIMITER,
		results,
		1 );

	element->accumulate_debit = ( *accumulate_debit_yn == 'y' );

	return element;

} /* ledger_account_fetch_element() */

ACCOUNT *ledger_account_calloc( void )
{
	ACCOUNT *account;

	if ( ! ( account = calloc( 1, sizeof( ACCOUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return account;

} /* ledger_account_calloc() */

ACCOUNT *ledger_account_new( char *account_name )
{
	ACCOUNT *account;

	account = ledger_account_calloc();
	account->account_name = account_name;
	return account;

} /* ledger_account_new() */

ACCOUNT *ledger_account_fetch(	char *application_name,
				char *account_name )
{
	ACCOUNT *account;

	account = ledger_account_new( account_name );

	ledger_account_load(	&account->fund_name,
				&account->subclassification_name,
				&account->display_order,
				&account->hard_coded_account_key,
				&account->tax_form_category_name,
				&account->accumulate_debit,
				application_name,
				account->account_name );

	return account;

} /* ledger_account_fetch() */

char *ledger_account_get_select( void )
{
	return
"account,fund,subclassification,display_order,hard_coded_account_key,tax_form_category";
}

ACCOUNT *ledger_subclassification_fund_seek_account(
				LIST *account_list,
				char *subclassification_name,
				char *fund_name )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return (ACCOUNT *)0;

	do {
		account = list_get( account_list );

		if ( timlib_strcmp(	account->subclassification_name,
					subclassification_name ) == 0
		&&   timlib_strcmp(	account->fund_name,
					fund_name ) == 0 )
		{
			return account;
		}

	} while( list_next( account_list ) );

	return (ACCOUNT *)0;

} /* ledger_subclassification_fund_seek_account() */

ACCOUNT *ledger_seek_hard_coded_account_key_account(
				LIST *account_list,
				char *fund_name,
				char *hard_coded_account_key )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return (ACCOUNT *)0;

	do {
		account = list_get( account_list );

		if ( timlib_strcmp(
			account->fund_name,
			fund_name ) == 0
		&&   timlib_strcmp(
			account->hard_coded_account_key,
			hard_coded_account_key ) == 0 )
		{
			return account;
		}

	} while( list_next( account_list ) );

	return (ACCOUNT *)0;

} /* ledger_seek_hard_coded_account_key_account() */

ACCOUNT *ledger_seek_account(	LIST *account_list,
				char *account_name )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return (ACCOUNT *)0;

	do {
		account = list_get( account_list );

		if ( strcmp( account->account_name, account_name ) == 0 )
			return account;

	} while( list_next( account_list ) );

	return (ACCOUNT *)0;

} /* ledger_seek_account() */

LIST *ledger_get_account_list(	char *application_name )
{
	ACCOUNT *account;
	char *select;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	static LIST *account_list = {0};
	FILE *input_pipe;

	if ( account_list ) return account_list;

	account_list = list_new();
	select = ledger_account_get_select();

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=account		",
		 application_name,
		 select );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		account = ledger_account_calloc();

		ledger_account_parse(
				&account->account_name,
				&account->fund_name,
				&account->subclassification_name,
				&account->display_order,
				&account->hard_coded_account_key,
				&account->tax_form_category_name,
				input_buffer );

		account->accumulate_debit =
			ledger_account_get_accumulate_debit(
				application_name, account->account_name );

		list_append_pointer( account_list, account );
	}

	pclose( input_pipe );
	return account_list;

} /* ledger_get_account_list() */

void ledger_account_parse(	char **account_name,
				char **fund_name,
				char **subclassification_name,
				int *display_order,
				char **hard_coded_account_key,
				char **tax_form_category_name,
				char *input_buffer )
{
	char buffer[ 128 ];

	if ( account_name )
	{
		piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		if ( *buffer )
			*account_name = strdup( buffer );
	}

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	if ( *buffer )
		*fund_name = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *buffer )
		*subclassification_name = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *buffer )
		*display_order = atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	if ( *buffer )
		*hard_coded_account_key = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *buffer )
		*tax_form_category_name = strdup( buffer );

} /* ledger_account_parse() */

void ledger_account_load(	char **fund_name,
				char **subclassification_name,
				int *display_order,
				char **hard_coded_account_key,
				char **tax_form_category_name,
				boolean *accumulate_debit,
				char *application_name,
				char *account_name )
{
	char *select;
	char buffer[ 128 ];
	char where[ 256 ];
	char sys_string[ 1024 ];
	char *results;

	select = ledger_account_get_select();

	sprintf(	where,
			"account = '%s'",
		 	timlib_escape_single_quotes(
				buffer,
				account_name ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=account		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot load account = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 account_name );
		exit( 1 );
	}

	ledger_account_parse(	(char **)0,
				fund_name,
				subclassification_name,
				display_order,
				hard_coded_account_key,
				tax_form_category_name,
				results );

	*accumulate_debit =
		ledger_account_get_accumulate_debit(
			application_name, account_name );

} /* ledger_account_load() */

SUBCLASSIFICATION *ledger_new_subclassification( char *subclassification_name )
{
	SUBCLASSIFICATION *subclassification;

	if ( ! ( subclassification =
			calloc( 1, sizeof( SUBCLASSIFICATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	subclassification->subclassification_name = subclassification_name;
	return subclassification;

} /* ledger_new_subclassification() */

JOURNAL_LEDGER *ledger_journal_ledger_calloc( void )
{
	JOURNAL_LEDGER *ledger;

	if ( ! ( ledger = calloc( 1, sizeof( JOURNAL_LEDGER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return ledger;

} /* ledger_journal_ledger_calloc() */

JOURNAL_LEDGER *journal_ledger_new(	char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *account_name )
{
	JOURNAL_LEDGER *ledger;

	ledger = ledger_journal_ledger_calloc();
	ledger->full_name = full_name;
	ledger->street_address = street_address;
	ledger->transaction_date_time = transaction_date_time;
	ledger->account_name = account_name;
	return ledger;

} /* journal_ledger_new() */

JOURNAL_LEDGER *ledger_get_latest_ledger(
					char *application_name,
					char *account_name,
					char *as_of_date )
{
	JOURNAL_LEDGER *ledger;
	char where[ 512 ];
	char select[ 128 ];
	char buffer[ 128 ];
	char sys_string[ 1024 ];
	char *ledger_table;
	char *transaction_table;
	char *results;
	char *latest_transaction_time;

	latest_transaction_time = LEDGER_CLOSING_TRANSACTION_TIME;

	ledger_table =
		get_table_name(
			application_name, LEDGER_FOLDER_NAME );

	transaction_table =
		get_table_name(
			application_name, TRANSACTION_FOLDER_NAME );

	if ( !as_of_date
	||   !*as_of_date
	||   strcmp(	as_of_date,
			"as_of_date" ) == 0 )
	{
		sprintf( where,
		"%s.transaction_date_time = %s.transaction_date_time and "
		"%s.account = '%s'					 ",
			ledger_table,
			transaction_table,
			ledger_table,
		 	timlib_escape_single_quotes( buffer, account_name ) );
	}
	else
	{
		sprintf( where,
		"%s.transaction_date_time = %s.transaction_date_time and "
		"%s.account = '%s' and					 "
		"%s.transaction_date_time <= '%s %s'			 ",
			ledger_table,
			transaction_table,
			ledger_table,
		 	timlib_escape_single_quotes( buffer, account_name ),
			transaction_table,
		 	as_of_date,
			latest_transaction_time );
	}

	sprintf(	select,
			"max( %s.transaction_date_time )",
			transaction_table );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s;\" | sql.e",
		 select,
		 ledger_table,
		 transaction_table,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (JOURNAL_LEDGER *)0;

	ledger = journal_ledger_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				results /* transaction_date_time */,
				account_name );

	ledger_load(		&ledger->full_name,
				&ledger->street_address,
				&ledger->transaction_count,
				&ledger->previous_balance,
				&ledger->debit_amount,
				&ledger->credit_amount,
				&ledger->balance,
				&ledger->memo,
				ledger->transaction_date_time,
				ledger->account_name,
				application_name );

	return ledger;

} /* ledger_get_latest_ledger() */

JOURNAL_LEDGER *ledger_get_prior_ledger(
					char *application_name,
					char *transaction_date_time,
					char *account_name )
{
	JOURNAL_LEDGER *ledger;
	char buffer[ 128 ];
	char where[ 512 ];
	char select[ 128 ];
	char sys_string[ 1024 ];
	char *table;
	char *results;

	if ( !transaction_date_time
	||   !*transaction_date_time )
	{
		return (JOURNAL_LEDGER *)0;
	}

	table = get_table_name( application_name, LEDGER_FOLDER_NAME );

	sprintf( where,
	"account = '%s' and transaction_date_time < '%s'",
		 timlib_escape_single_quotes( buffer, account_name ),
		 transaction_date_time );

	strcpy( select, "max( transaction_date_time )" );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 select,
		 table,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (JOURNAL_LEDGER *)0;

	ledger = journal_ledger_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				results /* transaction_date_time */,
				account_name );

	ledger_load(		&ledger->full_name,
				&ledger->street_address,
				&ledger->transaction_count,
				&ledger->previous_balance,
				&ledger->debit_amount,
				&ledger->credit_amount,
				&ledger->balance,
				&ledger->memo,
				ledger->transaction_date_time,
				ledger->account_name,
				application_name );

	return ledger;

} /* ledger_get_prior_ledger() */

void ledger_load(		char **full_name,
				char **street_address,
				int *transaction_count,
				double *previous_balance,
				double *debit_amount,
				double *credit_amount,
				double *balance,
				char **memo,
				char *transaction_date_time,
				char *account_name,
				char *application_name )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char select[ 256 ];
	char folder[ 128 ];
	char buffer[ 256 ];
	char piece_buffer[ 256 ];
	char *results;

	sprintf( select,
"%s.full_name,%s.street_address,transaction_count,previous_balance,debit_amount,credit_amount,balance,memo",
		 LEDGER_FOLDER_NAME,
		 LEDGER_FOLDER_NAME );

	sprintf(	folder,
			"%s,%s",
		 	LEDGER_FOLDER_NAME,
			TRANSACTION_FOLDER_NAME );

	sprintf( where,
		 "account = '%s' and					"
		 "%s.transaction_date_time = '%s' and			"
		 "%s.full_name = %s.full_name and			"
		 "%s.street_address = %s.street_address and		"
		 "%s.transaction_date_time = %s.transaction_date_time	",
		 timlib_escape_single_quotes( buffer, account_name ),
		 LEDGER_FOLDER_NAME,
		 transaction_date_time,
		 LEDGER_FOLDER_NAME,
		 TRANSACTION_FOLDER_NAME,
		 LEDGER_FOLDER_NAME,
		 TRANSACTION_FOLDER_NAME,
		 LEDGER_FOLDER_NAME,
		 TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( !results )
	{
		fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot fetch from %s: %s/%s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_FOLDER_NAME,
			 transaction_date_time,
			 account_name );
		exit( 1 );
	}

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
	*transaction_count = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 3 );
	*previous_balance = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 4 );
	*debit_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 5 );
	*credit_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 6 );
	*balance = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 7 );
	*memo = strdup( piece_buffer );

	free( results );

} /* ledger_load() */

void ledger_list_set_balances(	LIST *ledger_list,
				boolean accumulate_debit )
{
	JOURNAL_LEDGER *ledger;
	JOURNAL_LEDGER *prior_ledger = {0};
	JOURNAL_LEDGER *first_ledger = {0};
	int transaction_count;

	if ( !list_rewind( ledger_list ) ) return;

	/* Need a separate transaction_count to keep from double counting. */
	/* --------------------------------------------------------------- */
	first_ledger = list_get_first_pointer( ledger_list );
	transaction_count = first_ledger->transaction_count;

	do {
		ledger = list_get_pointer( ledger_list );

		if ( prior_ledger )
		{
			ledger->previous_balance = prior_ledger->balance;
		}

		if ( accumulate_debit )
		{
			if ( ledger->debit_amount )
			{
				ledger->balance =
					ledger->previous_balance +
					ledger->debit_amount;
			}
			else
			if ( ledger->credit_amount )
			{
				ledger->balance =
					ledger->previous_balance -
					ledger->credit_amount;

			}
		}
		else
		{
			if ( ledger->credit_amount )
			{
				ledger->balance =
					ledger->previous_balance +
					ledger->credit_amount;

			}
			else
			if ( ledger->debit_amount )
			{
				ledger->balance =
					ledger->previous_balance -
					ledger->debit_amount;
			}
		}

		if ( timlib_dollar_virtually_same(
			ledger->balance,
			0.0 ) )
		{
			ledger->transaction_count = 0;
			transaction_count = 1;
		}
		else
		{
			ledger->transaction_count = transaction_count;
			transaction_count++;
		}

		prior_ledger = ledger;

	} while( list_next( ledger_list ) );

} /* ledger_list_set_balances() */

void ledger_propagate_accounts(		LIST *propagate_account_list,
					char *application_name )
{
	ledger_account_list_balance_update(
				propagate_account_list,
				application_name );

} /* ledger_propagate_accounts() */

void ledger_account_list_balance_update(
				LIST *propagate_account_list,
				char *application_name )
{
	ACCOUNT *account;

	if ( !list_rewind( propagate_account_list ) ) return;

	do {
		account = list_get_pointer( propagate_account_list );

		ledger_journal_ledger_list_balance_update(
				account->journal_ledger_list,
				application_name );

	} while( list_next( propagate_account_list ) );

} /* ledger_account_list_balance_update() */

void ledger_journal_ledger_list_balance_update(
				LIST *journal_ledger_list,
				char *application_name )
{
	JOURNAL_LEDGER *ledger;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_name;
	char *key_column_list_string;

	if ( !list_rewind( journal_ledger_list ) ) return;

	table_name = get_table_name( application_name, LEDGER_FOLDER_NAME );
	key_column_list_string =
		"full_name,street_address,transaction_date_time,account";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	output_pipe = popen( sys_string, "w" );

	do {
		ledger = list_get_pointer( journal_ledger_list );

		if ( !ledger->transaction_count
		||	ledger->transaction_count !=
			ledger->database_transaction_count )
		{
			fprintf( output_pipe,
			 	"%s^%s^%s^%s^transaction_count^%d\n",
			 	ledger->full_name,
			 	ledger->street_address,
			 	ledger->transaction_date_time,
			 	ledger->account_name,
			 	ledger->transaction_count );

			ledger->database_transaction_count =
				ledger->transaction_count;
		}

		if ( timlib_dollar_virtually_same(
			ledger->previous_balance,
			0.0 )
		||   !timlib_dollar_virtually_same(
			ledger->previous_balance,
			ledger->database_previous_balance ) )
		{
			fprintf( output_pipe,
			 	"%s^%s^%s^%s^previous_balance^%.2lf\n",
			 	ledger->full_name,
			 	ledger->street_address,
			 	ledger->transaction_date_time,
			 	ledger->account_name,
			 	ledger->previous_balance );

			ledger->database_previous_balance =
				ledger->previous_balance;
		}

		if ( timlib_dollar_virtually_same(
			ledger->balance,
			0.0 )
		||   !timlib_dollar_virtually_same(
			ledger->balance,
			ledger->database_balance ) )
		{
			fprintf( output_pipe,
			 	"%s^%s^%s^%s^balance^%.2lf\n",
			 	ledger->full_name,
			 	ledger->street_address,
			 	ledger->transaction_date_time,
			 	ledger->account_name,
			 	ledger->balance );

		}

	} while( list_next( journal_ledger_list ) );

	pclose( output_pipe );

} /* ledger_journal_ledger_list_balance_update() */

LIST *ledger_subclassification_quickly_get_account_list(
					char *application_name,
					char *subclassification_name )
{
	ACCOUNT *account;
	char where[ 128 ];
	char *select;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	LIST *account_list;
	FILE *input_pipe;

	account_list = list_new();
	select = ledger_account_get_select();

	sprintf(where,
		"subclassification = '%s'",
		subclassification_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=account		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		account = ledger_account_calloc();

		ledger_account_parse(
				&account->account_name,
				&account->fund_name,
				&account->subclassification_name,
				&account->display_order,
				&account->hard_coded_account_key,
				&account->tax_form_category_name,
				input_buffer );

		list_append_pointer( account_list, account );
	}

	pclose( input_pipe );
	return account_list;

} /* ledger_subclassification_quickly_get_account_list() */

LIST *ledger_subclassification_get_account_list(
					char *application_name,
					char *subclassification_name,
					char *fund_name,
					char *as_of_date )
{
	LIST *account_list;
	ACCOUNT *account;
	char sys_string[ 1024 ];
	char where[ 256 ];
	char account_name[ 128 ];
	FILE *input_pipe;

	if ( fund_name
	&&   *fund_name
	&&   strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf(where,
			"fund = '%s' and subclassification = '%s'",
			fund_name,
			subclassification_name );
	}
	else
	{
		sprintf(where,
			"subclassification = '%s'",
			subclassification_name );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=account		"
		 "			folder=account		"
		 "			where=\"%s\"		"
		 "			order=display_order	",
		 application_name,
		 where );

	account_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( account_name, input_pipe ) )
	{
		account =
			ledger_account_fetch(
				application_name,
				strdup( account_name ) );

		account->latest_ledger =
			ledger_get_latest_ledger(
				application_name,
				account->account_name,
				as_of_date );

		if ( !account->latest_ledger
		||   timlib_double_virtually_same(
			account->latest_ledger->balance,
			0.0 ) )
		{
			continue;
		}

		list_add_pointer_in_order(
			account_list,
			account,
			ledger_balance_match_function );
	}

	pclose( input_pipe );
	return account_list;

} /* ledger_subclassification_get_account_list() */

LIST *ledger_element_get_subclassification_list(
					char *application_name,
					char *element_name,
					char *fund_name,
					char *as_of_date )
{
	LIST *subclassification_list;
	SUBCLASSIFICATION *subclassification;
	char sys_string[ 1024 ];
	char where[ 256 ];
	char subclassification_name[ 128 ];
	FILE *input_pipe;

	sprintf( where, "element = '%s'", element_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=subclassification	"
		 "			folder=subclassification	"
		 "			where=\"%s\"			"
		 "			order=display_order		",
		 application_name,
		 where );

	subclassification_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( subclassification_name, input_pipe ) )
	{
		subclassification =
			ledger_new_subclassification(
				strdup( subclassification_name ) );

		subclassification->account_list =
			ledger_subclassification_get_account_list(
				application_name,
				subclassification->subclassification_name,
				fund_name,
				as_of_date );

		list_append_pointer(	subclassification_list,
					subclassification );
	}

	pclose( input_pipe );
	return subclassification_list;

} /* ledger_element_get_subclassification_list() */

ELEMENT *ledger_element_list_seek(
				LIST *element_list,
				char *element_name )
{
	return ledger_element_seek( element_list, element_name );
}

ELEMENT *ledger_element_seek(	LIST *element_list,
				char *element_name )
{
	ELEMENT *element;

	if ( !list_rewind( element_list ) ) return (ELEMENT *)0;

	do {
		element = list_get_pointer( element_list );

		if ( strcmp( element->element_name, element_name ) == 0 )
		{
			return element;
		}

	} while( list_next( element_list ) );

	return (ELEMENT *)0;

} /* ledger_element_seek() */

LIST *ledger_get_element_list(	char *application_name,
				LIST *filter_element_name_list,
				char *fund_name,
				char *as_of_date )
{
	LIST *element_list;
	ELEMENT *element;
	char sys_string[ 1024 ];
	char input_buffer[ 256 ];
	char element_name[ 128 ];
	char accumulate_debit_yn[ 2 ];
	FILE *input_pipe;

	sprintf( sys_string,
		 "get_folder_data	application=%s			   "
		 "			select=element,accumulate_debit_yn "
		 "			folder=element			   "
		 "			order=display_order		   ",
		 application_name );

	element_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( element_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );

		if ( list_length( filter_element_name_list )
		&&   !list_exists_string(
			filter_element_name_list,
			element_name ) )
		{
			continue;
		}

		piece(	accumulate_debit_yn,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		element =
			ledger_new_element(
				strdup( element_name ) );

		element->accumulate_debit = ( *accumulate_debit_yn == 'y' );

		element->subclassification_list =
			ledger_element_get_subclassification_list(
				application_name,
				element->element_name,
				fund_name,
				as_of_date );

		list_append_pointer(	element_list,
					element );
	}

	pclose( input_pipe );

	return element_list;

} /* ledger_get_element_list() */

LATEX_ROW *ledger_get_latex_liabilities_plus_equity_row(
				double liabilities_plus_equity,
				boolean aggregate_subclassification )
{
	LATEX_ROW *latex_row;

	latex_row = latex_new_latex_row();

	list_append_pointer(
		latex_row->column_data_list,
		"\\Large \\bf Liabilities Plus Equity" );

	list_append_pointer( latex_row->column_data_list, "" );

	if ( !aggregate_subclassification )
		list_append_pointer( latex_row->column_data_list, "" );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( place_commas_in_money(
			   liabilities_plus_equity ) ) );

	return latex_row;

} /* ledger_get_latex_liabilities_plus_equity_row() */

LATEX_ROW *ledger_get_subclassification_latex_net_income_row(
				double net_income,
				boolean is_statement_of_activities )
{
	LATEX_ROW *latex_row;

	latex_row = latex_new_latex_row();

	if ( is_statement_of_activities )
	{
		list_append_pointer(
			latex_row->column_data_list,
			"\\bf Change in Net Assets" );
	}
	else
	{
		list_append_pointer(
			latex_row->column_data_list,
			"\\bf Net Income" );
	}

	list_append_pointer( latex_row->column_data_list, "" );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( place_commas_in_money(
			   net_income ) ) );

	return latex_row;

} /* ledger_get_subclassification_latex_net_income_row() */

LATEX_ROW *ledger_get_latex_net_income_row(
				double net_income,
				boolean is_statement_of_activities )
{
	LATEX_ROW *latex_row;

	latex_row = latex_new_latex_row();

	if ( is_statement_of_activities )
	{
		list_append_pointer(
			latex_row->column_data_list,
			"\\Large \\bf Change in Net Assets" );
	}
	else
	{
		list_append_pointer(
			latex_row->column_data_list,
			"\\Large \\bf Net Income" );
	}

	list_append_pointer( latex_row->column_data_list, "" );
	list_append_pointer( latex_row->column_data_list, "" );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( place_commas_in_money(
			   net_income ) ) );

	return latex_row;

} /* ledger_get_latex_net_income_row() */

LIST *ledger_get_subclassification_latex_row_list(
					double *total_element,
					LIST *subclassification_list,
					char *element_name,
					boolean element_accumulate_debit )
{
	LIST *row_list;
	double subclassification_amount;
	ACCOUNT *account;
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	double latest_ledger_balance;
	int first_time = 1;
	LATEX_ROW *latex_row;

	*total_element = 0.0;

	if ( !list_length( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	/* For equity, always display the element title */
	/* -------------------------------------------- */
	if ( strcmp( element_name, LEDGER_EQUITY_ELEMENT ) ==  0 )
	{
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		sprintf( format_buffer,
			 "\\large \\bf %s",
			 element_name );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					format_buffer ) ) );

		first_time = 0;

		/* Maybe financial_position, so get beginning balance. */
		/* --------------------------------------------------- */
		list_append_list(
			row_list,
			ledger_get_subclassification_beginning_latex_row_list(
				total_element,
				subclassification_list,
				element_accumulate_debit ) );
	}

	list_rewind( subclassification_list );

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Don't do net assets. */
		/* -------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( first_time )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			sprintf( format_buffer,
				 "\\large \\bf %s",
				 element_name );

			list_append_pointer(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ) );

			first_time = 0;
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			*total_element += latest_ledger_balance;

			subclassification_amount += latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

		if ( !timlib_double_virtually_same(
			subclassification_amount,
			0.0 )
		&&   list_length( subclassification_list ) > 1 )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			sprintf( format_buffer,
			 	 "%s",
			 	 subclassification->
					subclassification_name );

			list_append_pointer(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ) );

			list_append_pointer(
				latex_row->column_data_list,
				strdup( place_commas_in_money(
					   subclassification_amount ) ) );
		}

	} while( list_next( subclassification_list ) );

	if ( *total_element )
	{
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
		{
			sprintf(format_buffer,
				"\\bf %s",
				"Equity Ending Balance" );
		}
		else
		{
			sprintf(format_buffer,
			 	"\\bf Total %s",
			 	element_name );
		}

		 format_initial_capital( format_buffer, format_buffer );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( format_buffer ) );

		list_append_pointer(
			latex_row->column_data_list,
			"" );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
				   *total_element ) ) );

		/* Blank line */
		/* ---------- */
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );
		list_append_pointer(
			latex_row->column_data_list,
			"" );
	}

	return row_list;

} /* ledger_get_subclassification_latex_row_list() */

LIST *ledger_get_subclassification_beginning_latex_row_list(
				double *total_element,
				LIST *subclassification_list,
				boolean element_accumulate_debit )
{
	LIST *row_list;
	double subclassification_amount;
	ACCOUNT *account;
	SUBCLASSIFICATION *subclassification;
	double latest_ledger_balance;
	LATEX_ROW *latex_row;

	if ( !list_rewind( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Only do net assets. */
		/* ------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) != 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			/* -------------------------------------------- */
			/* Display all the equity element accounts,	*/
			/* except for change in net assets,		*/
			/* which gets displayed below.			*/
			/* -------------------------------------------- */
/*
			if ( strcmp(	element_name,
					LEDGER_EQUITY_ELEMENT ) ==  0
			&&   strcmp(	account->account_name,
					LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS )
					!= 0 )
			{
				latex_row = latex_new_latex_row();
				list_append_pointer( row_list, latex_row );

				list_append_pointer(
					latex_row->column_data_list,
					strdup( format_initial_capital(
							format_buffer,
							account->
							    account_name ) ) );

				list_append_pointer(
					latex_row->column_data_list,
					strdup( place_commas_in_money(
					   	latest_ledger_balance ) ) );
			}
*/

			*total_element += latest_ledger_balance;

			subclassification_amount += latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

/*
		sprintf( format_buffer,
			 "\\bf %s",
			 "Equity Beginning Balance" );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					format_buffer ) ) );
*/

		list_append_pointer(
			latex_row->column_data_list,
			strdup( LEDGER_BEGINNING_BALANCE_LABEL ) );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
				   subclassification_amount ) ) );

		return row_list;

	} while( list_next( subclassification_list ) );

	return (LIST *)0;

} /* ledger_get_subclassification_beginning_latex_row_list() */

LIST *ledger_get_latex_row_list(	double *total_element,
					LIST *subclassification_list,
					char *element_name,
					boolean element_accumulate_debit )
{
	LIST *row_list;
	double subclassification_amount;
	ACCOUNT *account;
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	double latest_ledger_balance;
	int first_time = 1;
	LATEX_ROW *latex_row;

	*total_element = 0.0;

	if ( !list_length( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	/* For equity, always display the element title */
	/* -------------------------------------------- */
	if ( strcmp( element_name, LEDGER_EQUITY_ELEMENT ) ==  0 )
	{
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		sprintf( format_buffer,
			 "\\large \\bf %s",
			 element_name );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					format_buffer ) ) );

		first_time = 0;

		/* Maybe financial_position, so get beginning balance. */
		/* --------------------------------------------------- */
		list_append_list(
			row_list,
			ledger_get_beginning_balance_latex_row_list(
				total_element,
				subclassification_list,
				element_accumulate_debit ) );
	}

	list_rewind( subclassification_list );

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Don't do net assets. */
		/* -------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( first_time )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			sprintf( format_buffer,
				 "\\large \\bf %s",
				 element_name );

			list_append_pointer(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ) );

			first_time = 0;
		}

		if ( list_length( subclassification->account_list ) > 1 )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			sprintf( format_buffer,
				 "\\bf %s",
				 subclassification->
					subclassification_name );

			list_append_pointer(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ) );
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			/* ---------------------------------------------- */
			/* Don't display single Change in net assets row. */
			/* Display the total row below.			  */
			/* ---------------------------------------------- */
			if ( strcmp(	account->account_name,
					LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS )
					!= 0 )
			{
				latex_row = latex_new_latex_row();
				list_append_pointer( row_list, latex_row );

				list_append_pointer(
					latex_row->column_data_list,
					strdup( format_initial_capital(
							format_buffer,
							account->
							    account_name ) ) );

				list_append_pointer(
					latex_row->column_data_list,
					strdup( place_commas_in_money(
					   	latest_ledger_balance ) ) );
			}
	
			*total_element += latest_ledger_balance;

			subclassification_amount +=
					latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

		if ( subclassification_amount
		&&   list_length( subclassification_list ) > 1 )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			if ( strcmp(
				subclassification->
					subclassification_name,
				LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0 )
			{
				sprintf(format_buffer,
					"\\bf %s",
					LEDGER_BEGINNING_BALANCE_LABEL );
			}
			else
			if ( strcmp(
				subclassification->
					subclassification_name,
				LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
			{
				sprintf( format_buffer,
					 "\\bf %s",
					 "Change in Net Assets" );
			}
			else
			{
				sprintf( format_buffer,
				 	"\\bf Total %s",
				 	subclassification->
						subclassification_name );
			}

			list_append_pointer(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ) );

			list_append_pointer(
				latex_row->column_data_list,
				"" );

			list_append_pointer(
				latex_row->column_data_list,
				strdup( place_commas_in_money(
					   subclassification_amount ) ) );
		}

	} while( list_next( subclassification_list ) );

	if ( *total_element )
	{
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
		{
			sprintf(format_buffer,
				"\\large \\bf %s",
				"Equity Ending Balance" );
		}
		else
		{
			sprintf(format_buffer,
			 	"\\large \\bf Total %s",
			 	element_name );
		}

		 format_initial_capital( format_buffer, format_buffer );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( format_buffer ) );

		list_append_pointer(
			latex_row->column_data_list,
			"" );

		list_append_pointer(
			latex_row->column_data_list,
			"" );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
				   *total_element ) ) );

		/* Blank line */
		/* ---------- */
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );
		list_append_pointer(
			latex_row->column_data_list,
			"" );
	}

	return row_list;

} /* ledger_get_latex_row_list() */

LIST *ledger_get_beginning_balance_latex_row_list(
				double *total_element,
				LIST *subclassification_list,
				boolean element_accumulate_debit )
{
	LIST *row_list;
	double subclassification_amount;
	ACCOUNT *account;
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	double latest_ledger_balance;
	LATEX_ROW *latex_row;

	if ( !list_rewind( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Only do net assets. */
		/* ------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) != 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( list_length( subclassification->account_list ) > 1 )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			sprintf( format_buffer,
				 "\\bf %s",
				 subclassification->
					subclassification_name );

			list_append_pointer(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ) );
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			list_append_pointer(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						account->
						    account_name ) ) );

			list_append_pointer(
				latex_row->column_data_list,
				strdup( place_commas_in_money(
				   	latest_ledger_balance ) ) );
	
			*total_element += latest_ledger_balance;

			subclassification_amount +=
					latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		sprintf( format_buffer,
			 "\\bf %s",
			 LEDGER_BEGINNING_BALANCE_LABEL );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					format_buffer ) ) );

		list_append_pointer(
			latex_row->column_data_list,
			"" );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
				   subclassification_amount ) ) );

		return row_list;

	} while( list_next( subclassification_list ) );

	return (LIST *)0;

} /* ledger_get_beginning_balance_latex_row_list() */

double ledger_get_element_value(	LIST *subclassification_list,
					boolean element_accumulate_debit )
{
	double total_element = 0.0;
	ACCOUNT *account;
	SUBCLASSIFICATION *subclassification;
	double latest_ledger_balance;

	if ( !list_rewind( subclassification_list ) ) return 0.0;
	
	do {
		subclassification = list_get_pointer( subclassification_list );

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			total_element += latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	return total_element;

} /* ledger_get_element_value() */

double ledger_output_subclassification_html_element(
					HTML_TABLE *html_table,
					LIST *subclassification_list,
					char *element_name,
					boolean element_accumulate_debit )
{
	double total_element = 0.0;
	double subclassification_amount;
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	char element_title[ 128 ];
	double latest_ledger_balance;
	boolean first_time = 1;

	/* For equity, always display the element title */
	/* -------------------------------------------- */
	if ( strcmp(	element_name,
			LEDGER_EQUITY_ELEMENT ) ==  0 )
	{
		sprintf(	element_title,
				"<h2>%s</h2>",
				format_initial_capital(
					format_buffer,
					element_name ) );

		html_table_set_data(
				html_table->data_list,
				element_title );

		html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
		html_table->data_list = list_new();

		/* Maybe financial_position, so display beginning balance. */
		/* ------------------------------------------------------- */
		total_element =
		ledger_output_subclassification_beginning_balance_html_element(
				html_table,
				subclassification_list,
				element_accumulate_debit );

		first_time = 0;
	}

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Don't display net assets. */
		/* ------------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( first_time )
		{
			sprintf(	element_title,
					"<h2>%s</h2>",
					format_initial_capital(
						format_buffer,
						element_name ) );

			html_table_set_data(
					html_table->data_list,
					element_title );

			html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );
			html_table->data_list = list_new();

			first_time = 0;
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			total_element +=
					latest_ledger_balance;

			subclassification_amount +=
					latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

		if ( !timlib_dollar_virtually_same(
			subclassification_amount,
			0.0 ) )
		{
			sprintf(buffer,
			 	"%s",
			 	format_initial_capital(
					format_buffer,
			 		subclassification->
					    subclassification_name ) );

			html_table_set_data(	html_table->data_list,
						strdup( buffer ) );

			html_table_set_data(
				html_table->data_list,
				strdup( place_commas_in_money(
					subclassification_amount ) ) );
	
			html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );

			html_table->data_list = list_new();
		}

	} while( list_next( subclassification_list ) );

	if ( !timlib_double_virtually_same( total_element, 0.0 ) )
	{
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
		{
			sprintf(element_title,
				"<h2>%s</h2>",
				"Equity Ending Balance" );
		}
		else
		{
			sprintf(element_title,
				"<h2>Total %s</h2>",
				element_name );
		}

		html_table_set_data(	html_table->data_list,
					element_title );
	
		html_table_set_data( html_table->data_list, "" );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				total_element ) ) );

		html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );
		html_table->data_list = list_new();
	}

	return total_element;

} /* ledger_output_subclassification_html_element() */

double ledger_output_subclassification_beginning_balance_html_element(
					HTML_TABLE *html_table,
					LIST *subclassification_list,
					boolean element_accumulate_debit )
{
	double subclassification_amount;
	ACCOUNT *account;
	char buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	double latest_ledger_balance;

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Only display net assets. */
		/* ------------------------ */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) != 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			subclassification_amount +=
					latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

		strcpy( buffer, LEDGER_BEGINNING_BALANCE_LABEL );

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				subclassification_amount ) ) );
	
		html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		html_table->data_list = list_new();

		return subclassification_amount;

	} while( list_next( subclassification_list ) );

	return 0.0;

} /* ledger_output_subclassification_beginning_balance_html_element() */

double ledger_output_html_element(	HTML_TABLE *html_table,
					LIST *subclassification_list,
					char *element_name,
					boolean element_accumulate_debit )
{
	double total_element = 0.0;
	double subclassification_amount;
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	char element_title[ 128 ];
	double latest_ledger_balance;
	boolean first_time = 1;

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* For equity, always display the element title */
	/* -------------------------------------------- */
	if ( strcmp(	element_name,
			LEDGER_EQUITY_ELEMENT ) ==  0 )
	{
		sprintf(	element_title,
				"<h2>%s</h2>",
				format_initial_capital(
					format_buffer,
					element_name ) );

		html_table_set_data(
				html_table->data_list,
				element_title );

		html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
		html_table->data_list = list_new();

		/* Maybe financial_position, so display beginning balance. */
		/* ------------------------------------------------------- */
		total_element =
			ledger_output_equity_beginning_balance_html_element(
				html_table,
				subclassification_list,
				element_accumulate_debit );

		first_time = 0;
	}

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Skip net assets. */
		/* ---------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( first_time )
		{
			sprintf(	element_title,
					"<h2>%s</h2>",
					format_initial_capital(
						format_buffer,
						element_name ) );

			html_table_set_data(
					html_table->data_list,
					element_title );

			html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );
			html_table->data_list = list_new();

			first_time = 0;
		}

		if ( list_length( subclassification->account_list ) > 1 )
		{
			html_table->data_list = list_new();

			sprintf(element_title,
				"<h3>%s</h3>",
				format_initial_capital(
					format_buffer,
					subclassification->
						subclassification_name ) );

			html_table_set_data(	html_table->data_list,
						element_title );
	
			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
			html_table->data_list = list_new();
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			/* ---------------------------------------------- */
			/* Don't display single Change in net assets row. */
			/* Display the total row below.			  */
			/* ---------------------------------------------- */
			if ( strcmp(	account->account_name,
					LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS )
					!= 0 )
			{
				html_table_set_data(
					html_table->data_list,
					strdup( format_initial_capital(
						buffer,
						account->account_name ) ) );
	
				html_table_set_data(
					html_table->data_list,
					strdup( place_commas_in_money(
						   latest_ledger_balance ) ) );
	
				html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );
	
				list_free_string_list( html_table->data_list );
				html_table->data_list = list_new();
			}

			total_element +=
					latest_ledger_balance;

			subclassification_amount +=
					latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
		{
			sprintf( buffer,
				 "<h3>%s</h3>",
				 "Change in Net Assets" );
		}
		else
		{
			sprintf( buffer,
			 	"<h3>Total %s</h3>",
			 	format_initial_capital(
					format_buffer,
			 		subclassification->
					    subclassification_name ) );
		}

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );
		html_table_set_data( html_table->data_list, "" );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				subclassification_amount ) ) );
	
		html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		html_table->data_list = list_new();

	} while( list_next( subclassification_list ) );

	if ( !timlib_double_virtually_same( total_element, 0.0 ) )
	{
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
		{
			sprintf(element_title,
				"<h2>%s</h2>",
				"Equity Ending Balance" );
		}
		else
		{
			sprintf(element_title,
				"<h2>Total %s</h2>",
				element_name );
		}

		html_table_set_data(	html_table->data_list,
					element_title );
	
		html_table_set_data( html_table->data_list, "" );
		html_table_set_data( html_table->data_list, "" );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				total_element ) ) );

		html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );
		html_table->data_list = list_new();
	}
	return total_element;

} /* ledger_output_html_element() */

double ledger_output_equity_beginning_balance_html_element(
				HTML_TABLE *html_table,
				LIST *subclassification_list,
				boolean element_accumulate_debit )
{
	double subclassification_amount;
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	char element_title[ 128 ];
	double latest_ledger_balance;

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Only display net assets. */
		/* ------------------------ */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) != 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( list_length( subclassification->account_list ) > 1 )
		{
			html_table->data_list = list_new();

			sprintf(element_title,
				"<h3>%s</h3>",
				format_initial_capital(
					format_buffer,
					subclassification->
						subclassification_name ) );

			html_table_set_data(	html_table->data_list,
						element_title );
	
			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
			html_table->data_list = list_new();
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			html_table_set_data(
				html_table->data_list,
				strdup( format_initial_capital(
					buffer,
					account->account_name ) ) );
	
			html_table_set_data(
				html_table->data_list,
				strdup( place_commas_in_money(
					   latest_ledger_balance ) ) );
	
			html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
	
			list_free_string_list( html_table->data_list );
			html_table->data_list = list_new();

			subclassification_amount +=
				latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

		sprintf( buffer,
			 "<h3>%s</h3>",
			 LEDGER_BEGINNING_BALANCE_LABEL );

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );
		html_table_set_data( html_table->data_list, "" );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				subclassification_amount ) ) );
	
		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		html_table->data_list = list_new();

		return subclassification_amount;

	} while( list_next( subclassification_list ) );

	return 0.0;

} /* ledger_output_equity_beginning_balance_html_element() */

double ledger_get_net_income(	double total_revenues,
				double total_expenses,
				double total_gains,
				double total_losses )
{
	return (	total_revenues -
			total_expenses +
			total_gains -
			total_losses );

} /* ledger_get_net_income() */

void ledger_output_subclassification_net_income(
					HTML_TABLE *html_table,
					double net_income,
					boolean is_statement_of_activities )
{
	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	html_table->data_list = list_new();

	if ( is_statement_of_activities )
	{
		html_table_set_data(
			html_table->data_list,
			"<h2>Change in Net Assets</h2>" );
	}
	else
	{
		html_table_set_data(
			html_table->data_list,
			"<h2>Net Income</h2>" );
	}
	html_table_set_data( html_table->data_list, "" );

	html_table_set_data(	html_table->data_list,
				strdup( place_commas_in_money( net_income ) ) );

	html_table_output_data( html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
	html_table->data_list = list_new();

} /* ledger_output_subclassification_net_income() */

void ledger_output_net_income(		HTML_TABLE *html_table,
					double net_income,
					boolean is_statement_of_activities )
{
	html_table->data_list = list_new();

	if ( is_statement_of_activities )
	{
		html_table_set_data(
			html_table->data_list,
			"<h2>Change in Net Assets</h2>" );
	}
	else
	{
		html_table_set_data(
			html_table->data_list,
			"<h2>Net Income</h2>" );
	}
	html_table_set_data( html_table->data_list, "" );
	html_table_set_data( html_table->data_list, "" );

	html_table_set_data(	html_table->data_list,
				strdup( place_commas_in_money( net_income ) ) );

	html_table_output_data( html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
	html_table->data_list = list_new();

} /* ledger_output_net_income() */

char *ledger_propagate_account_list_display(
					LIST *propagate_account_list )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;
	ACCOUNT *account;

	if ( !list_rewind( propagate_account_list ) ) return strdup( "" );

	do {
		account = list_get( propagate_account_list );

		buf_ptr += sprintf( buf_ptr,
				    "account = %s:\n",
				    account->account_name );

		buf_ptr +=
			   sprintf(
				buf_ptr,
				"%s\n",
				ledger_list_display(
					account->journal_ledger_list ) );

	} while( list_next( propagate_account_list ) );

	return strdup( buffer );

} /* ledger_propagate_account_list_display() */

char *ledger_transaction_display( TRANSACTION *transaction )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;

	buf_ptr += sprintf(	buf_ptr,
				"full_name = %s, "
				"street_address = %s, "
				"transaction_date_time = %s\n",
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time );

	buf_ptr += sprintf(	buf_ptr,
				"%s\n",
				ledger_list_display(
					transaction->journal_ledger_list ) );

	return strdup( buffer );

} /* ledger_transaction_display() */

char *ledger_list_display( LIST *ledger_list )
{
	char buffer[ 65536 ];
	JOURNAL_LEDGER *ledger;
	char *buf_ptr = buffer;

	*buf_ptr = '\0';

	if ( list_rewind( ledger_list ) )
	{
		do {
			ledger = list_get_pointer( ledger_list );

			buf_ptr +=
			   sprintf(	buf_ptr,
					"full_name = %s, "
					"street_address = %s, "
					"account=%s, "
					"transaction_date_time=%s, "
					"transaction_count=%d, "
					"database_transaction_count=%d, "
					"previous_balance=%.2lf, "
					"database_previous_balance=%.2lf, "
					"debit_amount=%.2lf, "
					"credit_amount=%.2lf, "
					"balance=%.2lf, "
					"database_balance=%.2lf\n",
					ledger->full_name,
					ledger->street_address,
					ledger->account_name,
					ledger->transaction_date_time,
					ledger->transaction_count,
					ledger->database_transaction_count,
					ledger->previous_balance,
					ledger->database_previous_balance,
					ledger->debit_amount,
					ledger->credit_amount,
					ledger->balance,
					ledger->database_balance );
		} while( list_next( ledger_list ) );
	}

	return strdup( buffer );

} /* ledger_list_display() */

TRANSACTION *ledger_transaction_calloc(	void )
{
	TRANSACTION *transaction;

	if ( ! ( transaction = calloc( 1, sizeof( TRANSACTION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return transaction;

} /* ledger_transaction_calloc() */

TRANSACTION *ledger_transaction_fetch(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	return ledger_transaction_with_load_new(
					application_name,
					full_name,
					street_address,
					transaction_date_time );
}

TRANSACTION *ledger_transaction_with_load_new(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	TRANSACTION *transaction;

	transaction = ledger_transaction_calloc();

	transaction->full_name = full_name;
	transaction->street_address = street_address;
	transaction->transaction_date_time = transaction_date_time;

	if ( !ledger_transaction_load(	&transaction->transaction_amount,
					&transaction->
						database_transaction_amount,
					&transaction->memo,
					application_name,
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: cannot load transaction = (%s/%s/%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 transaction->full_name,
			 transaction->street_address,
			 transaction->transaction_date_time );
		return transaction;
	}

	transaction->journal_ledger_list =
		ledger_get_journal_ledger_list(
			application_name,
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			(char *)0 /* account_name */ );

	return transaction;

} /* ledger_transaction_with_load_new() */

TRANSACTION *ledger_transaction_new(	char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *memo )
{
	TRANSACTION *transaction;

	transaction = ledger_transaction_calloc();

	transaction->full_name = full_name;
	transaction->street_address = street_address;
	transaction->transaction_date_time = transaction_date_time;
	transaction->memo = memo;

	return transaction;

} /* ledger_transaction_new() */

LIST *ledger_get_journal_ledger_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *account_name )
{
	char sys_string[ 1024 ];
	LIST *journal_ledger_list;
	char where[ 512 ];
	char *select;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char local_full_name[ 128 ];
	char local_street_address[ 128 ];
	char local_transaction_date_time[ 128 ];
	char local_account_name[ 128 ];
	char piece_buffer[ 128 ];
	char buffer[ 128 ];
	JOURNAL_LEDGER *ledger;

	select = ledger_journal_ledger_get_select();

/*
"full_name,street_address,transaction_date_time,account,transaction_count,previous_balance,debit_amount,credit_amount,balance";
*/

	if ( !full_name
	&&   !street_address
	&&   !transaction_date_time
	&&   account_name )
	{
		sprintf(where,
			"account = '%s'",
			timlib_escape_single_quotes(
				buffer, account_name ) );
	}
	else
	if ( !full_name
	&&   !street_address
	&&   transaction_date_time
	&&   account_name )
	{
		sprintf(where,
		 	"transaction_date_time >= '%s' and account = '%s'",
		 	transaction_date_time,
		 	timlib_escape_single_quotes(
				buffer,
				account_name ) );
	}
	else
	if ( full_name
	&&   street_address
	&&   transaction_date_time
	&&   !account_name )
	{
		strcpy(	where,
			ledger_get_transaction_where(
				full_name,
				street_address,
				transaction_date_time,
				(char *)0 /* folder_name */,
				"transaction_date_time" ) );
	}
	else
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: invalid parameter configuration.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		"
		 "			order=\"%s\"		",
		 application_name,
		 select,
		 LEDGER_FOLDER_NAME,
		 where,
		 "transaction_date_time" );

	input_pipe = popen( sys_string, "r" );
	journal_ledger_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	local_full_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	local_street_address,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	local_transaction_date_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	local_account_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		if ( !full_name
		&&   !street_address
		&&   !transaction_date_time
		&&   account_name )
		{
			ledger = journal_ledger_new(
					strdup( local_full_name ),
					strdup( local_street_address ),
					strdup( local_transaction_date_time ),
					account_name );
		}
		else
		if ( !full_name
		&&   !street_address
		&&   transaction_date_time
		&&   account_name )
		{
			ledger = journal_ledger_new(
					strdup( local_full_name ),
					strdup( local_street_address ),
					strdup( local_transaction_date_time ),
					account_name );
		}
		else
		{
			ledger = journal_ledger_new(
					full_name,
					street_address,
					transaction_date_time,
					strdup( local_account_name ) );
		}

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );
		ledger->transaction_count =
		ledger->database_transaction_count = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		ledger->previous_balance =
		ledger->database_previous_balance =
			atof( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			6 );
		ledger->debit_amount = atof( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			7 );
		ledger->credit_amount = atof( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			8 );
		ledger->balance =
		ledger->database_balance =
			atof( piece_buffer );

		list_append_pointer( journal_ledger_list, ledger );
	}

	pclose( input_pipe );

	return journal_ledger_list;

} /* ledger_get_journal_ledger_list() */

boolean ledger_transaction_load(	double *transaction_amount,
					double *database_transaction_amount,
					char **memo,
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char *results;
	char piece_buffer[ 2048 ];

	select = "transaction_amount,memo";

	where = ledger_get_transaction_where(
			full_name,
			street_address,
			transaction_date_time,
			(char *)0 /* folder_name */,
			"transaction_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=transaction		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	if ( *piece_buffer )
		*transaction_amount =
		*database_transaction_amount = atof( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	if ( *piece_buffer )
		*memo = strdup( piece_buffer );

	free( results );

	return 1;

} /* ledger_transaction_load() */

void ledger_transaction_insert(		char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double transaction_amount,
					char *memo,
					int check_number )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *transaction_table;
	char *field;
	char entity_buffer[ 128 ];
	char memo_buffer[ 1024 ];

	field =
"full_name,street_address,transaction_date_time,transaction_amount,memo,check_number";

	transaction_table =
		get_table_name(
			application_name, TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s del='^'	|"
		 "sql.e 2>&1					|"
		 "grep -vi duplicate				 ",
		 transaction_table,
		 field );

	output_pipe = popen( sys_string, "w" );

	if ( memo && *memo )
	{
		fprintf(	output_pipe,
				"%s^%s^%s^%.2lf^%s",
		 		escape_character(	entity_buffer,
							full_name,
							'\'' ),
				street_address,
				transaction_date_time,
				transaction_amount,
				escape_character(	memo_buffer,
							memo,
							'\'' ) );
	}
	else
	{
		fprintf(	output_pipe,
				"%s^%s^%s^%.2lf^",
		 		escape_character(	entity_buffer,
							full_name,
							'\'' ),
				street_address,
				transaction_date_time,
				transaction_amount );
	}

	if ( check_number )
		fprintf( output_pipe, "^%d\n", check_number );
	else
		fprintf( output_pipe, "^\n" );

	pclose( output_pipe );

} /* ledger_transaction_insert() */

void ledger_entity_update(		char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *preupdate_full_name,
					char *preupdate_street_address )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *table_name;
	char *key;
	char entity_buffer1[ 128 ];
	char entity_buffer2[ 128 ];
	char *where_full_name;
	char *where_street_address;

	if ( !transaction_date_time || !*transaction_date_time ) return;

	if ( strcmp( preupdate_full_name, "preupdate_full_name" ) != 0 )
		where_full_name = preupdate_full_name;
	else
		where_full_name = full_name;

	if ( strcmp(	preupdate_street_address,
			"preupdate_street_address" ) != 0 )
	{
		where_street_address = preupdate_street_address;
	}
	else
	{
		where_street_address = street_address;
	}

	key = "full_name,street_address,transaction_date_time";

	/* update TRANSACTION */
	/* ------------------ */
	table_name =
		get_table_name(
			application_name,
			TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s	|"
		 "sql.e					 ",
		 table_name,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s|full_name=%s|street_address=%s\n",
	 	escape_character(	entity_buffer1,
					where_full_name,
					'\'' ),
	 	where_street_address,
	 	transaction_date_time,
	 	escape_character(	entity_buffer2,
					full_name,
					'\'' ),
		street_address );

	pclose( output_pipe );

	/* update JOURNAL_LEDGER */
	/* --------------------- */
	table_name =
		get_table_name(
			application_name,
			LEDGER_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s	|"
		 "sql.e					 ",
		 table_name,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s|full_name=%s|street_address=%s\n",
	 	escape_character(	entity_buffer1,
					where_full_name,
					'\'' ),
	 	where_street_address,
	 	transaction_date_time,
	 	escape_character(	entity_buffer2,
					full_name,
					'\'' ),
		street_address );

	pclose( output_pipe );

} /* ledger_entity_update() */

void ledger_transaction_amount_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double transaction_amount,
					double database_transaction_amount )
{
	char transaction_amount_string[ 16 ];

	if ( timlib_dollar_virtually_same(
		transaction_amount,
		database_transaction_amount ) )
	{
		return;
	}

	sprintf( transaction_amount_string,
		 "%.2lf",
		 transaction_amount );

	ledger_transaction_generic_update(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		"transaction_amount" /* attribute */,
		transaction_amount_string /* data */ );

} /* ledger_transaction_amount_update() */

void ledger_transaction_generic_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *attribute_name,
					char *data )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *transaction_table;
	char *key;
	char entity_buffer[ 128 ];
	char data_buffer[ 65536 ];

	key = "full_name,street_address,transaction_date_time";

	transaction_table =
		get_table_name(
			application_name, TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 transaction_table,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^%s^%s\n",
	 	escape_character(	entity_buffer,
					full_name,
					'\'' ),
	 	street_address,
	 	transaction_date_time,
		attribute_name,
	 	escape_character(	data_buffer,
					data,
					'\'' ) );

	pclose( output_pipe );

} /* ledger_transaction_generic_update() */

void ledger_journal_generic_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *attribute_name,
					char *data )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *transaction_table;
	char *key;
	char entity_buffer[ 128 ];
	char data_buffer[ 65536 ];

	key = "full_name,street_address,transaction_date_time";

	transaction_table =
		get_table_name(
			application_name, LEDGER_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 transaction_table,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^%s^%s\n",
	 	escape_character(	entity_buffer,
					full_name,
					'\'' ),
	 	street_address,
	 	transaction_date_time,
		attribute_name,
	 	escape_character(	data_buffer,
					data,
					'\'' ) );

	pclose( output_pipe );

} /* ledger_journal_generic_update() */

void ledger_transaction_memo_update(	char *application_name,
					TRANSACTION *transaction )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *transaction_table;
	char *key;
	char entity_buffer[ 128 ];
	char memo_buffer[ 65536 ];

	if ( !transaction ) return;

	key = "full_name,street_address,transaction_date_time";

	transaction_table =
		get_table_name(
			application_name, TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 transaction_table,
		 key );

	output_pipe = popen( sys_string, "w" );

	if ( transaction->memo )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^memo^%s\n",
		 	escape_character(	entity_buffer,
						transaction->full_name,
						'\'' ),
		 	transaction->street_address,
		 	transaction->transaction_date_time,
		 	escape_character(	memo_buffer,
						transaction->memo,
						'\'' ) );
	}

	pclose( output_pipe );

} /* ledger_transaction_memo_update() */

DICTIONARY *ledger_get_hard_coded_dictionary(
				char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 128 ];
	DICTIONARY *non_fund_dictionary;
	static DICTIONARY *return_dictionary = {0};

	if ( return_dictionary ) return return_dictionary;

	folder = "account";

	/* Get fund dictionary */
	/* ------------------- */
	select = "concat( fund, '|', hard_coded_account_key ), account";

	strcpy( where,
	 	"hard_coded_account_key is not null and fund is not null" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
		 where );

	return_dictionary =
		pipe2dictionary(
			sys_string,
			FOLDER_DATA_DELIMITER );

	/* Get non fund dictionary */
	/* ------------------- */
	select = "hard_coded_account_key, account";

	strcpy( where,
	 	"hard_coded_account_key is not null and fund is null" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
		 where );

	non_fund_dictionary =
		pipe2dictionary(
			sys_string,
			FOLDER_DATA_DELIMITER );

	dictionary_append_dictionary( return_dictionary, non_fund_dictionary );

	return return_dictionary;

} /* ledger_get_hard_coded_dictionary() */

char *ledger_get_hard_coded_account_name(
				char *application_name,
				char *fund_name,
				char *hard_coded_account_key,
				boolean warning_only )
{
	DICTIONARY *hard_coded_dictionary;
	char *key;
	char *account_name;

	hard_coded_dictionary =
		ledger_get_hard_coded_dictionary(
			application_name );

	key = ledger_get_hard_coded_dictionary_key(
					fund_name,
					hard_coded_account_key );

	if ( ! ( account_name =
			dictionary_fetch(
				hard_coded_dictionary,
				key ) ) )
	{
		if ( warning_only )
			fprintf( stderr, "Warning " );
		else
			fprintf( stderr, "ERROR " );

		fprintf( stderr,
"in %s/%s()/%d: cannot fetch %s from hard_coded_account_key.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 key );

		if ( !warning_only ) exit( 1 );
	}

	return account_name;

} /* ledger_get_hard_coded_account_name() */

char *ledger_get_hard_coded_dictionary_key(
					char *fund_name,
					char *hard_coded_account_key )
{
	static char key[ 128 ];

	if ( fund_name )
	{
		sprintf(	key,
				"%s|%s",
				fund_name,
				hard_coded_account_key );
	}
	else
	{
		strcpy(	key, hard_coded_account_key );
	}

	return key;

} /* ledger_get_hard_coded_dictionary_key() */

void ledger_get_depreciation_account_names(
				char **depreciation_expense_account,
				char **accumulated_depreciation_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	key = "depreciation_expense_key";
	*depreciation_expense_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */ );

	key = "accumulated_depreciation_key";
	*accumulated_depreciation_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */ );

} /* ledger_get_depreciation_account_names() */

void ledger_get_customer_payment_account_names(
				char **checking_account,
				char **account_receivable_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	key = LEDGER_CASH_KEY;
	*checking_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "account_receivable_key";
	*account_receivable_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

} /* ledger_get_customer_payment_account_names() */

void ledger_get_vendor_payment_account_names(
				char **checking_account,
				char **uncleared_checks_account,
				char **account_payable_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	key = LEDGER_CASH_KEY;
	*checking_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "uncleared_checks_key";
	*uncleared_checks_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "account_payable_key";
	*account_payable_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

} /* ledger_get_vendor_payment_account_names() */

void ledger_get_customer_sale_account_names(
				char **sales_revenue_account,
				char **service_revenue_account,
				char **sales_tax_payable_account,
				char **shipping_revenue_account,
				char **inventory_account,
				char **cost_of_goods_sold_account,
				char **account_receivable_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	key = "sales_revenue_key";
	*sales_revenue_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "service_revenue_key";
	*service_revenue_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "sales_tax_payable_key";
	*sales_tax_payable_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "shipping_revenue_key";
	*shipping_revenue_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "inventory_key";
	*inventory_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "cost_of_goods_sold_key";
	*cost_of_goods_sold_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "account_receivable_key";
	*account_receivable_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

} /* ledger_get_customer_sale_account_names() */

void ledger_get_purchase_order_account_names(
				char **inventory_account,
				char **sales_tax_expense_account,
				char **freight_in_expense_account,
				char **account_payable_account,
				char **cash_account,
				char **uncleared_checks_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	key = "inventory_key";
	*inventory_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "sales_tax_expense_key";
	*sales_tax_expense_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "freight_in_expense_key";
	*freight_in_expense_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "account_payable_key";
	*account_payable_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	key = "cash_key";
	*cash_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */ );

	key = "uncleared_checks_key";
	*uncleared_checks_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */ );

} /* ledger_get_purchase_order_account_names() */

void ledger_journal_delete(		char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *account_name )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;
	char *table_name;

	field= "full_name,street_address,transaction_date_time,account";

	table_name = get_table_name( application_name, LEDGER_FOLDER_NAME );

	sprintf( sys_string,
		 "delete_statement table=%s field=%s delimiter='^'	|"
		 "sql.e							 ",
		 table_name,
		 field );

	output_pipe = popen( sys_string, "w" );

/*
		 	escape_character(	buffer,
						full_name,
						'\'' ),
*/
	fprintf(	output_pipe,
			"%s^%s^%s^%s\n",
			full_name,
			street_address,
			transaction_date_time,
			account_name );

	pclose( output_pipe );

} /* ledger_journal_delete() */

void ledger_delete(			char *application_name,
					char *folder_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;
	char *table_name;

	field= "full_name,street_address,transaction_date_time";

	table_name = get_table_name( application_name, folder_name );

	sprintf( sys_string,
		 "delete_statement table=%s field=%s delimiter='^'	|"
		 "sql.e							 ",
		 table_name,
		 field );

	output_pipe = popen( sys_string, "w" );

/*
	char buffer[ 256 ];
		 	escape_character(	buffer,
						full_name,
						'\'' ),
*/
	fprintf(	output_pipe,
			"%s^%s^%s\n",
			full_name,
			street_address,
			transaction_date_time );

	pclose( output_pipe );

} /* ledger_delete() */

void ledger_journal_ledger_insert(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *account_name,
					double amount,
					boolean is_debit )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;
	char *table_name;

	if ( is_debit )
	{
		field=
"full_name,street_address,transaction_date_time,account,debit_amount";
	}
	else
	{
		field=
"full_name,street_address,transaction_date_time,account,credit_amount";
	}

	table_name = get_table_name( application_name, "journal_ledger" );

	sprintf( sys_string,
		 "insert_statement table=%s field=%s delimiter='^' replace=n |"
		 "sql.e							      ",
		 table_name,
		 field );

	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
			"%s^%s^%s^%s^%.2lf\n",
			full_name,
			street_address,
			transaction_date_time,
			account_name,
			amount );

	pclose( output_pipe );

} /* ledger_journal_ledger_insert() */

void ledger_propagate(			char *application_name,
					char *transaction_date_time,
					char *account_name )
{
	JOURNAL_LEDGER *prior_ledger;
	LIST *propagate_journal_ledger_list;

	if ( !account_name )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: empty account_name for transaction_date_time = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 transaction_date_time );
		return;
	}

	prior_ledger =
		ledger_get_prior_ledger(
			application_name,
			transaction_date_time,
			account_name );

	propagate_journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			account_name );

	ledger_journal_ledger_list_balance_update(
			propagate_journal_ledger_list,
			application_name );

} /* ledger_propagate() */

char *ledger_get_transaction_where(	char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *folder_name,
					char *date_time_column )
{
	static char where[ 512 ];
	char buffer[ 256 ];

	if ( folder_name )
	{
		if ( full_name )
		{
			sprintf( where,
		 		"%s.full_name = '%s' and		"
	  	 		"%s.street_address = '%s' and		"
	  	 		"%s.%s = '%s'		 		",
				folder_name,
		 		escape_character(	buffer,
							full_name,
							'\'' ),
				folder_name,
		 		street_address,
				folder_name,
		 		date_time_column,
		 		transaction_date_time );
		}
		else
		{
			sprintf( where,
	  	 		"%s.%s = '%s'",
				folder_name,
		 		date_time_column,
		 		transaction_date_time );
		}
	}
	else
	{
		if ( full_name )
		{
			sprintf( where,
		 		"full_name = '%s' and			"
	  	 		"street_address = '%s' and		"
	  	 		"%s = '%s'		 		",
		 		escape_character(	buffer,
							full_name,
							'\'' ),
		 		street_address,
		 		date_time_column,
		 		transaction_date_time );
		}
		else
		{
			sprintf( where,
	  	 		"%s = '%s'",
		 		date_time_column,
		 		transaction_date_time );
		}
	}

	return where;

} /* ledger_get_transaction_where() */

LIST *ledger_get_propagate_journal_ledger_list(
					char *application_name,
					JOURNAL_LEDGER *prior_ledger,
					char *account_name )
{
	LIST *ledger_list = {0};
	boolean accumulate_debit;
	JOURNAL_LEDGER *first_journal_ledger;

	if ( !account_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: account_name is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	accumulate_debit =
		ledger_account_get_accumulate_debit(
			application_name, account_name );

	if ( prior_ledger )
	{
		ledger_list =
			ledger_get_journal_ledger_list(
				application_name,
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				prior_ledger->transaction_date_time,
				account_name );
	}
	else
	/* ------------------------------------------------- */
	/* Otherwise doing period of record for the account. */
	/* ------------------------------------------------- */
	{
		ledger_list =
			ledger_get_journal_ledger_list(
				application_name,
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				account_name );

		if ( !list_length( ledger_list ) ) return (LIST *)0;

		first_journal_ledger = list_get_first_pointer( ledger_list );
		first_journal_ledger->previous_balance = 0.0;
		first_journal_ledger->transaction_count = 1;
	}

	/* If deleted the latest one, then no propagate. */
	/* --------------------------------------------- */
	if ( prior_ledger && list_length( ledger_list ) == 1 )
	{
		return (LIST *)0;
	}

	ledger_list_set_balances(	ledger_list,
					accumulate_debit );

	return ledger_list;

} /* ledger_get_propagate_journal_ledger_list() */

boolean ledger_transaction_exists(	char *application_name,
					char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char *results;
	boolean return_value;

	select = "count";

	where = ledger_get_transaction_where(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				transaction_date_time,
				"transaction" /* folder_name */,
				"transaction_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 TRANSACTION_FOLDER_NAME,
		 where );

	results = pipe2string( sys_string );
	return_value = (*results == '1');
	free( results );

	return return_value;

} /* ledger_transaction_exists() */

LIST *ledger_get_transaction_list( char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char input_buffer[ 512 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 128 ];
	char memo[ 1024 ];
	LIST *transaction_list = list_new();
	FILE *input_pipe;
	TRANSACTION *transaction;

	select = "full_name,street_address,transaction_date_time,memo";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=transaction		"
		 "			order=transaction_date_time    |"
		 "cat							",
		 application_name,
		 select );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	full_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );
		piece(	street_address,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );
		piece(	transaction_date_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );
		piece(	memo,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		transaction = ledger_transaction_new(
					strdup( full_name ),
					strdup( street_address ),
					strdup( transaction_date_time ),
					strdup( memo ) );

		transaction->journal_ledger_list =
			ledger_get_journal_ledger_list(
				application_name,
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				(char *)0 /* account_name */ );

		list_append_pointer( transaction_list, transaction );
	}

	pclose( input_pipe );
	return transaction_list;

} /* ledger_get_transaction_list() */

char *ledger_get_transaction_date_time( char *transaction_date )
{
	char transaction_date_time[ 32 ];

	if ( !transaction_date
	||   !*transaction_date
	||   strcmp( transaction_date, "transaction_date" ) == 0 )
	{
		sprintf( transaction_date_time,
			 "%s %s",
			 date_get_now_yyyy_mm_dd(),
			 date_get_now_hh_colon_mm_colon_ss() );
	}
	else
	{
		strcpy( transaction_date_time,
			date_append_hhmmss( transaction_date ) );
	}

	return strdup( transaction_date_time );

} /* ledger_get_transaction_date_time() */

REOCCURRING_TRANSACTION *ledger_reoccurring_transaction_new(
					char *application_name,
					char *full_name,
					char *street_address,
					double transaction_amount )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;

	if ( ! ( reoccurring_transaction =
			calloc( 1, sizeof( REOCCURRING_TRANSACTION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	reoccurring_transaction->full_name = full_name;
	reoccurring_transaction->street_address = street_address;

	if ( !ledger_reoccurring_transaction_load(
			&reoccurring_transaction->debit_account,
			&reoccurring_transaction->credit_account,
			&transaction_amount,
			application_name,
			reoccurring_transaction->full_name,
			reoccurring_transaction->street_address ) )
	{
		return (REOCCURRING_TRANSACTION *)0;
	}

	reoccurring_transaction->transaction_amount = transaction_amount;

	return reoccurring_transaction;

} /* ledger_reoccurring_transaction_new() */

boolean ledger_reoccurring_transaction_load(
				char **debit_account,
				char **credit_account,
				double *transaction_amount,
				char *application_name,
				char *full_name,
				char *street_address )
{
	char sys_string[ 1024 ];
	char *select;
	char where[ 256 ];
	char *results;
	char entity_buffer[ 128 ];
	char piece_buffer[ 256 ];

	select = "debit_account,credit_account,transaction_amount";

	sprintf( where,
		 "full_name = '%s' and street_address = '%s'",
		 escape_character(	entity_buffer,
					full_name,
					'\'' ),
		 street_address );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 REOCCURRING_TRANSACTION_FOLDER_NAME,
		 where );

	results = pipe2string( sys_string );

	if ( !results ) return 0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*debit_account = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*credit_account = strdup( piece_buffer );

	if ( !*transaction_amount )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
		*transaction_amount = atof( piece_buffer );
	}

	return 1;

} /* ledger_reoccurring_transaction_load() */

LIST *ledger_transaction_date_time_account_name_list(
				char *application_name,
				char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	sprintf(	where,
			"transaction_date_time = '%s'",
			transaction_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=account		"
		 "			folder=journal_ledger	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	return pipe2list( sys_string );

} /* ledger_transaction_date_time_account_name_list() */

double ledger_get_account_debit_amount(	LIST *journal_ledger_list,
					char *account_name )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( ! ( journal_ledger =
			ledger_journal_ledger_seek(
				journal_ledger_list,
				account_name ) ) )
	{
		return 0.0;
	}
	else
	{
		return journal_ledger->debit_amount;
	}

} /* ledger_get_account_debit_amount() */

double ledger_get_account_credit_amount(LIST *journal_ledger_list,
					char *account_name )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( ! ( journal_ledger =
			ledger_journal_ledger_seek(
				journal_ledger_list,
				account_name ) ) )
	{
		return 0.0;
	}
	else
	{
		return journal_ledger->credit_amount;
	}

} /* ledger_get_account_credit_amount() */

JOURNAL_LEDGER *ledger_journal_ledger_seek(
					LIST *journal_ledger_list,
					char *account_name )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( journal_ledger_list ) ) return (JOURNAL_LEDGER *)0;

	do {
		journal_ledger = list_get_pointer( journal_ledger_list );

		if ( timlib_strcmp(	journal_ledger->account_name,
					account_name ) == 0 )
		{
			return journal_ledger;
		}

	} while( list_next( journal_ledger_list ) );

	return (JOURNAL_LEDGER *)0;

} /* ledger_journal_ledger_seek() */

void ledger_journal_ledger_update(	FILE *update_pipe,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *account_name,
					double debit_amount,
					double credit_amount )
{
	if ( !full_name || !street_address || !transaction_date_time )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty primary key.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		pclose( update_pipe );
		exit( 1 ); 
	}

	if ( !timlib_dollar_virtually_same(
		debit_amount,
		0.0 ) )
	{
		fprintf(update_pipe,
		 	"%s^%s^%s^%s^debit_amount^%.2lf\n",
		 	full_name,
		 	street_address,
		 	transaction_date_time,
		 	account_name,
		 	debit_amount );
	}
	else
	if ( !timlib_dollar_virtually_same(
		credit_amount,
		0.0 ) )
	{
		fprintf(update_pipe,
		 	"%s^%s^%s^%s^credit_amount^%.2lf\n",
		 	full_name,
		 	street_address,
		 	transaction_date_time,
		 	account_name,
		 	credit_amount );
	}
	else
	{
		pclose( update_pipe );

		fprintf( stderr,
	"ERROR in %s/%s()/%d: both debit_amount and credit_amount are zero.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fflush( update_pipe );

} /* ledger_journal_ledger_update() */

FILE *ledger_open_update_pipe( char *application_name )
{
	char *table_name;
	char *key_column_list_string;
	char sys_string[ 1024 ];

	table_name = get_table_name( application_name, LEDGER_FOLDER_NAME );

	key_column_list_string =
		"full_name,street_address,transaction_date_time,account";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	return popen( sys_string, "w" );

} /* ledger_open_update_pipe() */

void ledger_update_transaction_date_time(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *new_transaction_date_time )
{
	char *table_name;
	char *key_column_list_string;
	char sys_string[ 1024 ];
	FILE *update_pipe;

	key_column_list_string =
		"full_name,street_address,transaction_date_time";

	table_name =
		get_table_name(
			application_name,
			TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	update_pipe = popen( sys_string, "w" );

	fprintf(update_pipe,
	 	"%s^%s^%s^transaction_date_time^%s\n",
	 	full_name,
	 	street_address,
	 	transaction_date_time,
	 	new_transaction_date_time );

	pclose( update_pipe );

	table_name =
		get_table_name(
			application_name,
			LEDGER_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	update_pipe = popen( sys_string, "w" );

	fprintf(update_pipe,
	 	"%s^%s^%s^transaction_date_time^%s\n",
	 	full_name,
	 	street_address,
	 	transaction_date_time,
	 	new_transaction_date_time );

	pclose( update_pipe );

} /* ledger_update_transaction_date_time() */

char *ledger_transaction_get_select( void )
{
	char *select =
"transaction.full_name,transaction.street_address,transaction.transaction_date_time,memo";

	return select;

} /* ledger_transaction_get_select() */

void ledger_transaction_parse(
				char **full_name,
				char **street_address,
				char **transaction_date_time,
				char **memo,
				char *input_buffer )
{
	char piece_buffer[ 1024 ];

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	*full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	*street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	*transaction_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	*memo = strdup( piece_buffer );

} /* ledger_transaction_parse() */

HASH_TABLE *ledger_get_transaction_hash_table(
				char *application_name,
				char *inventory_name )
{
	char sys_string[ 2048 ];
	char *select;
	char *folder_list_string;
	char *inventory_subquery;
	char where[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	HASH_TABLE *transaction_hash_table;

	transaction_hash_table = hash_table_new( hash_table_huge );

	select = ledger_transaction_get_select();

	folder_list_string = "transaction,purchase_order";

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"purchase_order"     /* one2m_folder_name */,
			"inventory_purchase" /* mto1_folder_name */,
			"purchase_date_time" /* foreign_attribute_name */ );

	sprintf( where,
		 "%s and %s and "
		 "purchase_order.transaction_date_time is not null",
		 inventory_subquery,
		 ledger_get_transaction_purchase_order_join_where() );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_list_string,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		ledger_set_transaction_hash_table(
			transaction_hash_table,
			input_buffer );
	}

	pclose( input_pipe );

	folder_list_string = "transaction,customer_sale";

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"customer_sale"      /* one2m_folder_name */,
			"inventory_sale"     /* mto1_folder_name */,
			"sale_date_time"     /* foreign_attribute_name */ );

	sprintf( where,
		 "%s and %s and "
		 "customer_sale.transaction_date_time is not null",
		 inventory_subquery,
		 ledger_get_transaction_customer_sale_join_where() );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_list_string,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		ledger_set_transaction_hash_table(
			transaction_hash_table,
			input_buffer );
	}

	pclose( input_pipe );

	return transaction_hash_table;

} /* ledger_get_transaction_hash_table() */

void ledger_set_transaction_hash_table(
				HASH_TABLE *transaction_hash_table,
				char *input_buffer )
{
	TRANSACTION *transaction;
	char *key;

	transaction = ledger_transaction_calloc();

	ledger_transaction_parse(
			&transaction->full_name,
			&transaction->street_address,
			&transaction->transaction_date_time,
			&transaction->memo,
			input_buffer );

	key = ledger_get_transaction_hash_table_key(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time );

	hash_table_set_pointer(
			transaction_hash_table,
			strdup( key ),
			transaction );

} /* ledger_set_transaction_hash_table() */

char *ledger_get_transaction_hash_table_key(
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	static char key[ 256 ];

	sprintf(	key,
			"%s^%s^%s",
			full_name,
			street_address,
			transaction_date_time );

	return key;

} /* ledger_get_transaction_hash_table_key() */

char *ledger_journal_ledger_get_select( void )
{
	char *select =
"journal_ledger.full_name,journal_ledger.street_address,journal_ledger.transaction_date_time,account,transaction_count,previous_balance,debit_amount,credit_amount,balance";

	return select;

} /* ledger_journal_ledger_get_select() */

char *ledger_get_transaction_purchase_order_join_where( void )
{
	char *join_where;

	join_where =	"transaction.full_name =			"
			"	purchase_order.full_name and		"
			"transaction.street_address =			"
			"	purchase_order.street_address and	"
			"transaction.transaction_date_time =		"
			"	purchase_order.transaction_date_time	";

	return join_where;

} /* ledger_get_transaction_purchase_order_join_where() */

char *ledger_get_transaction_customer_sale_join_where( void )
{
	char *join_where;

	join_where =	"transaction.full_name =			"
			"	customer_sale.full_name and		"
			"transaction.street_address =			"
			"	customer_sale.street_address and	"
			"transaction.transaction_date_time =		"
			"	customer_sale.transaction_date_time	";

	return join_where;

} /* ledger_get_transaction_purchase_order_join_where() */

char *ledger_get_journal_ledger_purchase_order_join_where( void )
{
	char *join_where;

	join_where =	"journal_ledger.full_name =			"
			"	purchase_order.full_name and		"
			"journal_ledger.street_address =		"
			"	purchase_order.street_address and	"
			"journal_ledger.transaction_date_time =		"
			"	purchase_order.transaction_date_time	";

	return join_where;

} /* ledger_get_journal_ledger_purchase_order_join_where() */

char *ledger_get_journal_ledger_customer_sale_join_where( void )
{
	char *join_where;

	join_where =	"journal_ledger.full_name =			"
			"	customer_sale.full_name and		"
			"journal_ledger.street_address =		"
			"	customer_sale.street_address and	"
			"journal_ledger.transaction_date_time =		"
			"	customer_sale.transaction_date_time	";

	return join_where;

} /* ledger_get_journal_ledger_purchase_order_join_where() */

void ledger_journal_ledger_parse(
				char **full_name,
				char **street_address,
				char **transaction_date_time,
				char **account_name,
				int *transaction_count,
				int *database_transaction_count,
				double *previous_balance,
				double *database_previous_balance,
				double *debit_amount,
				double *credit_amount,
				double *balance,
				double *database_balance,
				char *input_buffer )
{
	char piece_buffer[ 1024 ];

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	*full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	*street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	*transaction_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	*account_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	*transaction_count =
	*database_transaction_count = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	*previous_balance =
	*database_previous_balance = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	*debit_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	*credit_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	*balance =
	*database_balance = atof( piece_buffer );

} /* ledger_journal_ledger_parse() */

HASH_TABLE *ledger_get_journal_ledger_hash_table(
				char *application_name,
				char *inventory_name )
{
	char sys_string[ 2048 ];
	char *select;
	char *folder_list_string;
	char *inventory_subquery;
	char where[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	HASH_TABLE *journal_ledger_hash_table;

	journal_ledger_hash_table = hash_table_new( hash_table_huge );

	select = ledger_journal_ledger_get_select();

	folder_list_string = "journal_ledger,purchase_order";

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"purchase_order"     /* one2m_folder_name */,
			"inventory_purchase" /* mto1_folder_name */,
			"purchase_date_time" /* foreign_attribute_name */ );

	sprintf( where,
		 "%s and %s and "
		 "purchase_order.transaction_date_time is not null",
		 inventory_subquery,
		 ledger_get_journal_ledger_purchase_order_join_where() );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_list_string,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		ledger_set_journal_ledger_hash_table(
			journal_ledger_hash_table,
			input_buffer );
	}

	pclose( input_pipe );

	folder_list_string = "journal_ledger,customer_sale";

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"customer_sale"  /* one2m_folder_name */,
			"inventory_sale" /* mto1_folder_name */,
			"sale_date_time" /* foreign_attribute_name */ );

	sprintf( where,
		 "%s and %s and "
		 "customer_sale.transaction_date_time is not null",
		 inventory_subquery,
		 ledger_get_journal_ledger_customer_sale_join_where() );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_list_string,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		ledger_set_journal_ledger_hash_table(
			journal_ledger_hash_table,
			input_buffer );
	}

	pclose( input_pipe );

	return journal_ledger_hash_table;

} /* ledger_get_journal_ledger_hash_table() */

void ledger_set_journal_ledger_hash_table(
				HASH_TABLE *journal_ledger_hash_table,
				char *input_buffer )
{
	JOURNAL_LEDGER *journal_ledger;
	char *key;

	journal_ledger = ledger_journal_ledger_calloc();

	ledger_journal_ledger_parse(
			&journal_ledger->full_name,
			&journal_ledger->street_address,
			&journal_ledger->transaction_date_time,
			&journal_ledger->account_name,
			&journal_ledger->transaction_count,
			&journal_ledger->database_transaction_count,
			&journal_ledger->previous_balance,
			&journal_ledger->database_previous_balance,
			&journal_ledger->debit_amount,
			&journal_ledger->credit_amount,
			&journal_ledger->balance,
			&journal_ledger->database_balance,
			input_buffer );

	key = ledger_get_journal_ledger_hash_table_key(
			journal_ledger->full_name,
			journal_ledger->street_address,
			journal_ledger->transaction_date_time,
			journal_ledger->account_name );

	hash_table_set_pointer(
		journal_ledger_hash_table,
		strdup( key ),
		journal_ledger );

} /* ledger_get_journal_ledger_hash_table() */

char *ledger_get_journal_ledger_hash_table_key(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *account_name )
{
	static char key[ 256 ];

	sprintf(	key,
			"%s^%s^%s^%s",
			full_name,
			street_address,
			transaction_date_time,
			account_name );

	return key;

} /* ledger_get_journal_ledger_hash_table_key() */

TRANSACTION *ledger_purchase_transaction_fetch(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				HASH_TABLE *transaction_hash_table,
				HASH_TABLE *journal_ledger_hash_table )
{
	char *inventory_account = {0};
	char *sales_tax_expense_account;
	char *freight_in_expense_account;
	char *account_payable_account;
	char *cash_account;
	char *uncleared_checks_account;
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char *key;

	key = ledger_get_transaction_hash_table_key(
			full_name,
			street_address,
			transaction_date_time );

	if ( ! ( transaction =
			hash_table_fetch( 
				transaction_hash_table,
				key ) ) )
	{
		return (TRANSACTION *)0;
	}

	ledger_get_purchase_order_account_names(
		&inventory_account,
		&sales_tax_expense_account,
		&freight_in_expense_account,
		&account_payable_account,
		&cash_account,
		&uncleared_checks_account,
		application_name,
		fund_name );

	if ( !inventory_account ) return (TRANSACTION *)0;

	/* ========================= */
	/* Build journal_ledger_list */
	/* ========================= */
	transaction->journal_ledger_list = list_new();

	/* Inventory asset */
	/* --------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			inventory_account );


	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

#ifdef NOT_DEFINED
	/* Supply expense */
	/* -------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			supply_expense_account );


	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Repairs and maintenance expense */
	/* ------------------------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			repairs_maintenance_expense_account );


	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Fixed asset */
	/* ----------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			fixed_asset_account );


	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}
#endif

	/* Sales tax expense */
	/* ----------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			sales_tax_expense_account );


	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Freight in expense */
	/* ------------------ */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			freight_in_expense_account );


	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Account payable */
	/* --------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			account_payable_account );


	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	return transaction;

} /* ledger_purchase_transaction_fetch() */

TRANSACTION *ledger_sale_transaction_fetch(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				HASH_TABLE *transaction_hash_table,
				HASH_TABLE *journal_ledger_hash_table )
{
	char *sales_revenue_account = {0};
	char *service_revenue_account;
	char *sales_tax_payable_account;
	char *shipping_revenue_account;
	char *inventory_account;
	char *cost_of_goods_sold_account;
	char *receivable_account;
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char *key;

	key = ledger_get_transaction_hash_table_key(
			full_name,
			street_address,
			transaction_date_time );

	if ( ! ( transaction =
			hash_table_fetch( 
				transaction_hash_table,
				key ) ) )
	{
		return (TRANSACTION *)0;
	}

	ledger_get_customer_sale_account_names(
		&sales_revenue_account,
		&service_revenue_account,
		&sales_tax_payable_account,
		&shipping_revenue_account,
		&inventory_account,
		&cost_of_goods_sold_account,
		&receivable_account,
		application_name,
		fund_name );

	if ( !sales_revenue_account ) return (TRANSACTION *)0;

	/* ========================= */
	/* Build journal_ledger_list */
	/* ========================= */
	transaction->journal_ledger_list = list_new();

	/* Sales revenue */
	/* ------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			sales_revenue_account );


	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Service revenue */
	/* --------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			service_revenue_account );


	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Sales tax payable */
	/* ----------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			sales_tax_payable_account );

	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Shipping revenue */
	/* ---------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			shipping_revenue_account );


	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Inventory asset */
	/* --------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			inventory_account );


	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Cost of goods sold */
	/* ------------------ */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			cost_of_goods_sold_account );

	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Accounts receivable */
	/* ------------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			receivable_account );

	if ( ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	return transaction;

} /* ledger_sale_transaction_fetch() */

char *ledger_get_shipped_date_transaction_date_time(
				char *shipped_date )
{
	char transaction_date_time[ 32 ];

	sprintf( transaction_date_time,
		 "%s %s",
		 shipped_date,
		 date_get_now_hh_colon_mm_colon_ss() );

	return strdup( transaction_date_time );

} /* ledger_get_shipped_date_transaction_date_time() */

char *ledger_get_account_subclassification_name(
				char *application_name,
				char *account_name )
{
	char where[ 256 ];
	char sys_string[ 1024 ];
	char buffer[ 128 ];

	sprintf( where, "account = '%s'",
		 timlib_escape_single_quotes(
			buffer,
			account_name ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=subclassification	"
		 "			folder=account			"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* ledger_get_account_subclassification_name() */

char *ledger_get_supply_expense_account(
				char *application_name,
				char *supply_name )
{
	char where[ 256 ];
	char sys_string[ 1024 ];

	sprintf( where, "supply_name = '%s'", supply_name );
	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=account			"
		 "			folder=supply			"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* ledger_get_supply_expense_account() */

char *ledger_get_supply_expense_key_account(
				char *application_name,
				char *fund_name )
{
	char *key;
	char *supply_expense_account;

	key = "supply_expense_key";
	supply_expense_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */ );

	return supply_expense_account;

} /* ledger_get_supply_expense_key_account() */

char *ledger_get_inventory_account_name(
				char *application_name,
				char *fund_name )
{
	char *key;
	char *inventory_account;

	key = "inventory_key";
	inventory_account =
		ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				1 /* warning_only */ );

	return inventory_account;

} /* ledger_get_inventory_account_name() */

LEDGER *ledger_new( void )
{
	LEDGER *ledger;

	if ( ! ( ledger = calloc( 1, sizeof( LEDGER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	ledger->element_list = list_new();

	return ledger;

} /* ledger_new() */

CONTRA_ACCOUNT *ledger_contra_account_new(	char *account_name,
						char *contra_to_account_name )
{
	CONTRA_ACCOUNT *contra_account;

	if ( ! ( contra_account = calloc( 1, sizeof( CONTRA_ACCOUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	contra_account->account_name = account_name;
	contra_account->contra_to_account_name = contra_to_account_name;

	return contra_account;

} /* ledger_contra_account_new() */

LIST *ledger_get_contra_account_list( char *application_name )
{
	LIST *contra_account_list;
	CONTRA_ACCOUNT *contra_account;
	char sys_string[ 1024 ];
	char input_buffer[ 512 ];
	char account_name[ 128 ];
	char contra_to_account_name[ 128 ];
	FILE *input_pipe;
	char *select;

	select = "account,contra_to_account";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=contra_account	",
		 application_name,
		 select );

	contra_account_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	account_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	contra_to_account_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		contra_account =
			ledger_contra_account_new(
				strdup( account_name ),
				strdup( contra_to_account_name ) );

		list_append_pointer( contra_account_list, contra_account );
	}

	pclose( input_pipe );
	return contra_account_list;

} /* ledger_get_contra_account_list() */

CONTRA_ACCOUNT *ledger_seek_contra_account(
				LIST *contra_account_list,
				char *account_name )
{
	CONTRA_ACCOUNT *contra_account;

	if ( !list_rewind( contra_account_list ) ) return (CONTRA_ACCOUNT *)0;

	do {
		contra_account = list_get( contra_account_list );

		if ( strcmp(	contra_account->account_name,
				account_name ) == 0 )
		{
			return contra_account;
		}

	} while( list_next( contra_account_list ) );

	return (CONTRA_ACCOUNT *)0;

} /* ledger_seek_contra_account() */

boolean ledger_account_get_accumulate_debit(
					char *application_name,
					char *account_name )
{
	static LEDGER *ledger = {0};
	ELEMENT *element;

	if ( !ledger )
	{
		ledger = ledger_new();

/*
		ledger->contra_account_list =
			ledger_get_contra_account_list(
				application_name );
*/
	}

	if ( ! ( element =
			ledger_account_fetch_element(
				application_name,
				account_name ) ) )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: cannot fetch element for account = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 account_name );
		return 0;
	}

/*
	if ( ( contra_account =
			ledger_seek_contra_account(
				ledger->contra_account_list,
				account_name ) ) )
	{
		return 1 - element->accumulate_debit;
	}
	else
	{
		return element->accumulate_debit;
	}
*/
	return element->accumulate_debit;

} /* ledger_account_get_accumulate_debit() */

int ledger_balance_match_function(
				ACCOUNT *account_from_list,
				ACCOUNT *account_compare )
{
	if ( !account_from_list->latest_ledger
	||   !account_from_list->latest_ledger->balance )
	{
		return 1;
	}

	if ( !account_compare->latest_ledger
	||   !account_compare->latest_ledger->balance )
	{
		return 1;
	}

	if ( account_compare->display_order) return 1;

	/* Sort descending */
	/* --------------- */
	if (	account_from_list->latest_ledger->balance <=
		account_compare->latest_ledger->balance )
	{
		return 1;
	}
	else
	{
		return -1;
	}

} /* ledger_balance_match_function() */

void ledger_propagate_element_list(
				char *application_name,
				char *transaction_date_time_string,
				LIST *element_list )
{
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get_pointer( element_list );

		if ( !list_rewind( element->subclassification_list ) ) continue;

		do {
			subclassification =
				list_get_pointer(
					element->subclassification_list );

			if ( !list_rewind( subclassification->account_list ) )
				continue;

			do {
				account =
					list_get_pointer( 
						subclassification->
							account_list );

				if ( account->latest_ledger
				&&   account->latest_ledger->balance )
				{
					ledger_propagate(
						application_name,
						transaction_date_time_string,
						account->account_name );
				}


			} while( list_next( subclassification->account_list ) );

		} while( list_next( element->subclassification_list ) );

	} while( list_next( element_list ) );

} /* ledger_propagate_element_list() */

char *ledger_get_closing_transaction_date_time(
				char *application_name,
				char *as_of_date )
{
	char transaction_date_time[ 32 ];
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *results;

	sprintf(	transaction_date_time,
			"%s %s",
			as_of_date,
			LEDGER_CLOSING_TRANSACTION_TIME );

	sprintf( where,
		 "transaction_date_time = '%s'",
		 transaction_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=count		"
		 "			folder=transaction	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	results = pipe2string( sys_string );

	if ( results && atoi( results ) )
		return (char *)0;
	else
		return strdup( transaction_date_time );

} /* ledger_get_closing_transaction_date_time() */

char *ledger_nominal_accounts_beginning_transaction_date(
				char *application_name,
				char *fund_name,
				char *ending_transaction_date )
{
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *select;
	char folder[ 128 ];
	char *results;
	char transaction_date_string[ 16 ];
	char ending_transaction_date_time[ 32 ];
	DATE *transaction_date;

	if ( fund_name && !*fund_name )
		fund_name = (char *)0;
	else
	if ( fund_name && strcmp( fund_name, "fund" ) == 0 )
		fund_name = (char *)0;

	/* Get the prior closing entry then return its tomorrow. */
	/* ----------------------------------------------------- */
	strcpy( folder, TRANSACTION_FOLDER_NAME );
	select = "max( transaction_date_time )";

	sprintf( ending_transaction_date_time,
		 "%s %s",
		 ending_transaction_date,
		 LEDGER_CLOSING_TRANSACTION_TIME );

	sprintf( where,
		 "memo = '%s' and transaction_date_time <= '%s'",
		 CLOSING_ENTRY_MEMO,
		 ending_transaction_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( results && *results )
	{
		column( transaction_date_string, 0, results );

		transaction_date =
			date_yyyy_mm_dd_new(
				transaction_date_string );

		date_increment_days( transaction_date, 1 );
		return date_get_yyyy_mm_dd_string( transaction_date );
	}

	/* No closing entries */
	/* ------------------ */
	select = "min( transaction_date_time )";

	if ( fund_name )
	{
		/* Get the first entry for the fund. */
		/* --------------------------------- */
		sprintf(folder,
		 	"%s,%s",
		 	LEDGER_FOLDER_NAME,
		 	ACCOUNT_FOLDER_NAME );

		sprintf(where,
		 	"fund = '%s' and				"
			"transaction_date_time <= '%s' and		"
			"%s.account = %s.account			",
		 	fund_name,
		 	ending_transaction_date_time,
			LEDGER_FOLDER_NAME,
			ACCOUNT_FOLDER_NAME );
	}
	else
	{
		/* Get the first entry. */
		/* -------------------- */
		strcpy( folder, LEDGER_FOLDER_NAME );

		sprintf(where,
		 	"transaction_date_time <= '%s'",
		 	ending_transaction_date_time );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (char *)0;

	return strdup( column( transaction_date_string, 0, results )  );

#ifdef NOT_DEFINED
	/* Assume the opening entry was 11:59 the night before. */
	/* ---------------------------------------------------- */
	column( transaction_date_string, 0, results );

	transaction_date =
		date_yyyy_mm_dd_new(
			transaction_date_string );

	date_increment_days( transaction_date, 1 );
	return date_get_yyyy_mm_dd_string( transaction_date );
#endif

} /* ledger_nominal_accounts_beginning_transaction_date() */

LIST *ledger_get_fund_name_list( char *application_name )
{
	char sys_string[ 512 ];

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=fund	"
		 "			folder=fund	",
		 application_name );

	return pipe2list( sys_string );

} /* ledger_get_fund_name_list() */

void ledger_get_report_title_sub_title(
		char *title,
		char *sub_title,
		char *process_name,
		char *application_name,
		char *fund_name,
		char *as_of_date,
		int fund_list_length )
{
	char *beginning_date;
	char beginning_date_american[ 16 ];
	char ending_date_american[ 16 ];

	*beginning_date_american = '\0';

	sprintf(title,
		"%s",
		application_get_application_title(
			application_name ) );

	if ( ! ( beginning_date = 
		ledger_nominal_accounts_beginning_transaction_date(
			application_name,
			fund_name,
			as_of_date ) ) )
	{
		printf( "<h3>Error. No transactions.</h3.\n" );
		document_close();
		exit( 0 );
	}

	date_convert_source_international(
		beginning_date_american,
		american,
		beginning_date );

	date_convert_source_international(
		ending_date_american,
		american,
		as_of_date );

	if ( fund_name
	&&   *fund_name
	&&   strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf(sub_title,
	 		"%s, Fund: %s, Beginning: %s, Ending: %s",
	 		process_name,
			fund_name,
			beginning_date_american,
	 		ending_date_american );
	}
	else
	if ( fund_list_length > 1 )
	{
		sprintf(sub_title,
	 		"%s, Consolidated Funds, Beginning: %s, Ending: %s",
	 		process_name,
			beginning_date_american,
	 		ending_date_american );
	}
	else
	{
		if ( *beginning_date_american )
		{
			sprintf(sub_title,
	 			"%s, Beginning: %s, Ending: %s",
	 			process_name,
				beginning_date_american,
	 			ending_date_american );
		}
		else
		{
			sprintf(sub_title,
	 			"%s, as of date: %s",
	 			process_name,
	 			ending_date_american );
		}
	}

	format_initial_capital( sub_title, sub_title );

} /* ledger_get_report_title_sub_title() */

TAX_FORM_CATEGORY *ledger_tax_form_category_new(
					char *tax_form_category_name,
					char *tax_form_line,
					boolean itemize_accounts )
{
	TAX_FORM_CATEGORY *e;

	if ( ! ( e = calloc( 1, sizeof( TAX_FORM_CATEGORY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	e->tax_form_category_name = tax_form_category_name;
	e->tax_form_line = tax_form_line;
	e->itemize_accounts = itemize_accounts;
	return e;

} /* ledger_tax_form_category_new() */

LIST *ledger_tax_form_fetch_category_list(
					char *application_name,
					char *as_of_date )
{
	LIST *account_list;
	TAX_FORM_CATEGORY *tax_form_category;
	LIST *tax_form_category_list;
	char sys_string[ 1024 ];
	char *select;
	char *folder_name;
	char *order;
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char tax_form_category_name[ 128 ];
	char tax_form_line[ 128 ];
	char itemize_accounts_yn[ 128 ];

	account_list =
		ledger_get_account_list(
			application_name );

	if ( !list_length( account_list ) ) return (LIST *)0;

	select = "tax_form_category,tax_form_line,itemize_accounts_yn";
	folder_name = "tax_form_category";
	order = "tax_form_line";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			order=%s	",
		 application_name,
		 select,
		 folder_name,
		 order );

	input_pipe = popen( sys_string, "r" );

	tax_form_category_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	tax_form_category_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	tax_form_line,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	itemize_accounts_yn,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		tax_form_category =
			ledger_tax_form_category_new(
				strdup( tax_form_category_name ),
				strdup( tax_form_line ),
				(*itemize_accounts_yn == 'y' ) );

		tax_form_category->account_list =
			ledger_tax_form_get_account_list(
				&tax_form_category->balance_sum,
				application_name,
				account_list,
				tax_form_category->tax_form_category_name,
				as_of_date );

		list_append_pointer(
			tax_form_category_list,
			tax_form_category );

	} /* while( get_line() ) */

	pclose( input_pipe );
	return tax_form_category_list;

} /* ledger_tax_form_fetch_category_list() */

LIST *ledger_tax_form_get_account_list(
				double *balance_sum,
				char *application_name,
				LIST *account_list,
				char *tax_form_category_name,
				char *as_of_date )
{
	LIST *tax_form_account_list;
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return (LIST *)0;

	tax_form_account_list = list_new();

	do {
		account = list_get( account_list );

		if ( timlib_strcmp(
				account->tax_form_category_name,
				tax_form_category_name ) == 0 )
		{
			account->latest_ledger =
				ledger_get_latest_ledger(
					application_name,
					account->account_name,
					as_of_date );

			if ( !account->latest_ledger
			||   timlib_double_virtually_same(
				account->latest_ledger->balance,
				0.0 ) )
			{
				continue;
			}

			if ( account->latest_ledger )
			{
				(*balance_sum) +=
					account->
						latest_ledger->
						balance;
			}

			list_add_pointer_in_order(
				tax_form_account_list,
				account,
				ledger_balance_match_function );
		}

	} while( list_next( account_list ) );

	return tax_form_account_list;

} /* ledger_tax_form_get_account_list() */

boolean ledger_journal_ledger_list_propagate(
			char *application_name,
			LIST *journal_ledger_list,
			char *propagate_transaction_date_time )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( journal_ledger_list ) ) return 0;

	do {
		journal_ledger = list_get_pointer( journal_ledger_list );

		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			journal_ledger->account_name );

	} while( list_next( journal_ledger_list ) );

	return 1;

} /* ledger_journal_ledger_list_propagate() */

boolean ledger_journal_ledger_list_set_amount(
			char *application_name,
			LIST *journal_ledger_list,
			double amount )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( journal_ledger_list ) ) return 0;

	do {
		journal_ledger = list_get_pointer( journal_ledger_list );

		if ( !timlib_dollar_virtually_same(
			journal_ledger->debit_amount,
			0.0 ) )
		{
			journal_ledger->debit_amount = amount;
		}
		else
		if ( !timlib_dollar_virtually_same(
			journal_ledger->credit_amount,
			0.0 ) )
		{
			journal_ledger->credit_amount = amount;
		}
		else
		{
			return 0;
		}

	} while( list_next( journal_ledger_list ) );

	return 1;

} /* ledger_journal_ledger_list_set_amount() */

boolean ledger_journal_ledger_list_amount_update(
			char *application_name,
			LIST *journal_ledger_list )
{
	JOURNAL_LEDGER *journal_ledger;
	FILE *update_pipe;

	if ( !list_rewind( journal_ledger_list ) ) return 0;

	update_pipe = ledger_open_update_pipe( application_name );

	do {
		journal_ledger = list_get_pointer( journal_ledger_list );

		ledger_journal_ledger_update(
			update_pipe,
			journal_ledger->full_name,
			journal_ledger->street_address,
			journal_ledger->transaction_date_time,
			journal_ledger->account_name,
			journal_ledger->debit_amount,
			journal_ledger->credit_amount );

	} while( list_next( journal_ledger_list ) );

	pclose( update_pipe );
	return 1;

} /* ledger_journal_ledger_list_amount_update() */

