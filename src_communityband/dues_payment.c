/* -------------------------------------------------------------------- */
/* src_communityband/dues_payment.c					*/
/* -------------------------------------------------------------------- */
/* This is the appaserver dues_payment ADT.				*/
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
#include "dues_payment.h"

DUES_PAYMENT *dues_payment_new( void )
{
	DUES_PAYMENT *p =
		(DUES_PAYMENT *)
			calloc( 1, sizeof( DUES_PAYMENT ) );

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
} /* dues_payment_new() */

char *dues_payment_get_where(	char *full_name,
				char *street_address,
				char *payment_date )
{
	static char where[ 512 ];
	char buffer[ 256 ];

	sprintf( where,
		"full_name = '%s' and			"
	  	"street_address = '%s' and		"
	  	"payment_date = '%s'	 		",
		escape_character(	buffer,
					full_name,
					'\'' ),
		street_address,
		payment_date );

	return where;
} /* dues_payment_get_where() */

DUES_PAYMENT *dues_payment_fetch(
			char *application_name,
			char *full_name,
			char *street_address,
			char *payment_date )
{
	char *select;
	char sys_string[ 1024 ];
	char *where;
	char *results;
	DUES_PAYMENT *dues_payment;

	select = dues_payment_get_select();

	where = dues_payment_get_where(
			full_name,
			street_address,
			payment_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=dues_payment		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		return (DUES_PAYMENT *)0;
	}

	dues_payment = dues_payment_parse( results );

	if ( dues_payment->transaction_date_time )
	{
		dues_payment->transaction =
			ledger_transaction_fetch(
				application_name,
				dues_payment->full_name,
				dues_payment->street_address,
				dues_payment->transaction_date_time );
	}

	return dues_payment;

} /* dues_payment_fetch() */

void dues_payment_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *payment_date,
			double payment_amount,
			double database_payment_amount,
			char *transaction_date_time,
			char *database_transaction_date_time )
{
	char *sys_string;
	FILE *output_pipe;

	sys_string = dues_payment_get_update_sys_string( application_name );
	output_pipe = popen( sys_string, "w" );

	if ( !timlib_dollar_virtually_same(
				payment_amount,
				database_payment_amount ) )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^payment_amount^%.2lf\n",
			full_name,
			street_address,
			payment_date,
			payment_amount );
	}

	if ( timlib_strcmp(	transaction_date_time,
				database_transaction_date_time ) != 0 )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^transaction_date_time^%s\n",
			full_name,
			street_address,
			payment_date,
			(transaction_date_time)
				? transaction_date_time
				: "" );
	}

	pclose( output_pipe );

} /* dues_payment_update() */

DUES_PAYMENT *dues_payment_parse(
			char *input_buffer )
{
	DUES_PAYMENT *dues_payment;
	char piece_buffer[ 256 ];

	dues_payment = dues_payment_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	dues_payment->full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	dues_payment->street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	dues_payment->payment_date = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	dues_payment->payment_amount =
	dues_payment->database_payment_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	dues_payment->check_number = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *piece_buffer )
		dues_payment->transaction_date_time =
		dues_payment->database_transaction_date_time =
			strdup( piece_buffer );

	return dues_payment;

} /* dues_payment_parse() */

char *dues_payment_get_select( void )
{
	char *select =
"full_name,street_address,payment_date,payment_amount,check_number,transaction_date_time";
	return select;
}

char *dues_payment_get_update_sys_string(
			char *application_name )
{
	static char sys_string[ 256 ];
	char *table_name;
	char *key;

	table_name = get_table_name( application_name, "dues_payment" );
	key = "full_name,street_address,payment_date";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	return sys_string;

} /* dues_payment_get_update_sys_string() */

void dues_payment_journal_ledger_refresh(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double payment_amount )
{
	ACCOUNT *debit_account;
	ACCOUNT *credit_account;
	LIST *account_list;

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	account_list = ledger_get_account_list(	application_name );

	if ( ! ( debit_account =
			ledger_seek_hard_coded_account_key_account(
				account_list,
				LEDGER_GENERAL_FUND,
				LEDGER_CASH_KEY ) ) )
	{
		fprintf( stderr,
	"Error in %s/%s()/%d: cannot seek hard_coded_account_key = (%s/%s).\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
			LEDGER_GENERAL_FUND,
			LEDGER_CASH_KEY );
		exit(1 );
	}

	if ( ! ( credit_account =
			ledger_seek_hard_coded_account_key_account(
				account_list,
				LEDGER_GENERAL_FUND,
				DUES_PAYMENT_KEY ) ) )
	{
		fprintf( stderr,
	"Error in %s/%s()/%d: cannot seek hard_coded_account_key = (%s/%s).\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
			LEDGER_GENERAL_FUND,
			DUES_PAYMENT_KEY );
		exit(1 );
	}

	ledger_journal_ledger_insert(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		debit_account->account_name,
		payment_amount,
		1 /* is_debit */ );

	ledger_journal_ledger_insert(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		credit_account->account_name,
		payment_amount,
		0 /* not is_debit */ );

} /* dues_payment_journal_ledger_refresh() */

void dues_payment_journal_ledger_propagate(
			char *application_name,
			char *transaction_date_time )
{
	ACCOUNT *debit_account;
	ACCOUNT *credit_account;
	LIST *account_list;

	account_list = ledger_get_account_list(	application_name );

	if ( ! ( debit_account =
			ledger_seek_hard_coded_account_key_account(
				account_list,
				LEDGER_GENERAL_FUND,
				LEDGER_CASH_KEY ) ) )
	{
		fprintf( stderr,
	"Error in %s/%s()/%d: cannot seek hard_coded_account_key = (%s/%s).\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
			LEDGER_GENERAL_FUND,
			LEDGER_CASH_KEY );
		exit(1 );
	}

	if ( ! ( credit_account =
			ledger_seek_hard_coded_account_key_account(
				account_list,
				LEDGER_GENERAL_FUND,
				DUES_PAYMENT_KEY ) ) )
	{
		fprintf( stderr,
	"Error in %s/%s()/%d: cannot seek hard_coded_account_key = (%s/%s).\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
			LEDGER_GENERAL_FUND,
			DUES_PAYMENT_KEY );
		exit(1 );
	}

	ledger_propagate(	application_name,
				transaction_date_time,
				debit_account->account_name );

	ledger_propagate(	application_name,
				transaction_date_time,
				credit_account->account_name );

} /* dues_payment_journal_ledger_propagate() */

