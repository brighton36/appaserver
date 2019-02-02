/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/bank_upload_transaction_insert.c	*/
/* ----------------------------------------------------------------	*/
/* Input: BANK_UPLOAD and JOURNAL_LEDGER where account is cash.		*/
/* Output: Insert into BANK_UPLOAD_TRANSACTION statements.		*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "date.h"
#include "environ.h"
#include "ledger.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "bank_upload.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void bank_upload_transaction_direct_insert(
				char *bank_date,
				char *bank_description,
				char *full_name,
				char *street_address,
				char *transaction_date_time );

char *bank_upload_bank_date_todo_subquery( void );

char *bank_upload_full_name_todo_subquery( void );

boolean bank_upload_transaction_insert_debit(
			char *application_name,
			char *key,
			char *value,
			char *date,
			double exact_value );

boolean bank_upload_transaction_insert_credit(
			char *application_name,
			char *key,
			char *value,
			char *date,
			double exact_value );

void bank_upload_transaction_insert_input_buffer(
	/* --------------------------------- */
	/* key is bank_date^bank_description */
	/* --------------------------------- */
	char *key,
	/* -------------------------------------------------------------- */
	/* input_buffer is full_name^street_address^transaction_date_time */
	/* input_buffer optionally appends '[|another]'			  */
	/* -------------------------------------------------------------- */
	char *input_buffer );

void bank_upload_transaction_insert_deposit(
			char *application_name );

void bank_upload_transaction_insert_withdrawal(
			char *application_name );

void bank_upload_transaction_insert_bank_upload_withdrawal(
			char *application_name,
			char *bank_date,
			char *bank_description );

void bank_upload_transaction_insert_bank_upload_deposit(
			char *application_name,
			char *bank_date,
			char *bank_description );

int main( int argc, char **argv )
{
	char *application_name;
	char *operation;
	char bank_date[ 128 ];
	char bank_description[ 1024 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 64 ];
	int delimiter_count;

	if ( argc != 2 )
	{
		fprintf( stderr,
"Usage: %s deposit|withdrawal|both|bank_date^bank_description|bank_date^bank_description^full_name^street_address^transaction_date_time\n",
			 argv[ 0 ] );

		exit ( 1 );
	}

	operation = argv[ 1 ];

	application_name = environ_get_application_name( argv[ 0 ] );

	if ( strcmp( operation, "deposit" ) == 0
	||   strcmp( operation, "both" ) == 0 )
	{
		bank_upload_transaction_insert_deposit( application_name );
	}

	if ( strcmp( operation, "withdrawal" ) == 0
	||   strcmp( operation, "both" ) == 0 )
	{
		bank_upload_transaction_insert_withdrawal( application_name );
	}
	else
	{
		delimiter_count =
			timlib_count_delimiters(
				'^', operation );

		if ( delimiter_count == 1 )
		{
			piece( bank_date, '^', operation, 0 );
			piece( bank_description, '^', operation, 1 );

			bank_upload_transaction_insert_bank_upload_withdrawal(
				application_name,
				bank_date,
				bank_description );

			bank_upload_transaction_insert_bank_upload_deposit(
				application_name,
				bank_date,
				bank_description );
		}
		else
		if ( delimiter_count == 4 )
		{
			piece( bank_date, '^', operation, 0 );
			piece( bank_description, '^', operation, 1 );
			piece( full_name, '^', operation, 2 );
			piece( street_address, '^', operation, 3 );
			piece( transaction_date_time, '^', operation, 4 );

			bank_upload_transaction_direct_insert(
				bank_date,
				bank_description,
				full_name,
				street_address,
				transaction_date_time );
		}
	}

	return 0;

} /* main() */

char *bank_upload_bank_date_todo_subquery( void )
{
	char *subquery;

	subquery =
		"not exists						"
		"(select 1 from bank_upload_transaction			"
		"	where bank_upload_transaction.bank_date =	"
		"	      bank_upload.bank_date and			"
		"	      bank_upload_transaction.bank_description ="
		"	      bank_upload.bank_description )		";

	return subquery;

} /* bank_upload_bank_date_todo_subquery() */

char *bank_upload_full_name_todo_subquery( void )
{
	char *subquery;

	subquery =
		"not exists						"
		"(select 1 from bank_upload_transaction			"
		"	where bank_upload_transaction.full_name =	"
		"	      journal_ledger.full_name and		"
		"	      bank_upload_transaction.street_address =	"
		"	      journal_ledger.street_address and		"
		"	      bank_upload_transaction.			"
		"		transaction_date_time =			"
		" 	      journal_ledger.transaction_date_time )	";

	return subquery;

} /* bank_upload_full_name_todo_subquery() */

void bank_upload_transaction_insert_bank_upload_deposit(
				char *application_name,
				char *bank_date,
				char *bank_description )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 512 ];
	char buffer[ 256 ];
	char *order;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char key[ 256 ];
	char date[ 128 ];
	char value[ 32 ];

	select = "bank_date, bank_description, bank_amount";
	folder = "bank_upload";
	order = "sequence_number,bank_date";

	sprintf( where,
		 "bank_date = '%s' and bank_description = '%s' and %s",
		 bank_date,
		 escape_character(	buffer,
					bank_description,
					'\'' ),
		 bank_upload_bank_date_todo_subquery() );

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=\"%s\"		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=%s		|"
		 "tr '%c' '|'					|"
		 "sed 's/|/^/1'					|"
		 "cat						 ",
		 application_name,
		 select,
		 folder,
		 where,
		 order,
		 FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while ( get_line( input_buffer, input_pipe ) )
	{
		piece( key, '|', input_buffer, 0 );
		piece( value, '|', input_buffer, 1 );
		piece( date, '^', key, 0 );

		if ( bank_upload_transaction_insert_debit(
			application_name,
			key,
			value,
			date,
			atof( value ) /* exact_value */ ) )
		{
			/* Just do one */
			/* ----------- */
			break;
		}

		if ( bank_upload_transaction_insert_debit(
				application_name,
				key,
				value,
				date,
				0.0 /* exact_value */ ) )
		{
			/* Just do one */
			/* ----------- */
			break;
		}

	}

	pclose( input_pipe );

} /* bank_upload_transaction_insert_bank_upload_deposit() */

void bank_upload_transaction_insert_bank_upload_withdrawal(
				char *application_name,
				char *bank_date,
				char *bank_description )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 512 ];
	char buffer[ 256 ];
	char *order;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char key[ 256 ];
	char date[ 128 ];
	char value[ 32 ];

	select = "bank_date, bank_description, 0 - bank_amount";
	folder = "bank_upload";
	order = "sequence_number,bank_date";

	sprintf( where,
		 "bank_date = '%s' and bank_description = '%s' and %s",
		 bank_date,
		 escape_character(	buffer,
					bank_description,
					'\'' ),
		 bank_upload_bank_date_todo_subquery() );

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=\"%s\"		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=%s		|"
		 "tr '%c' '|'					|"
		 "sed 's/|/^/1'					|"
		 "cat						 ",
		 application_name,
		 select,
		 folder,
		 where,
		 order,
		 FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while ( get_line( input_buffer, input_pipe ) )
	{
		piece( key, '|', input_buffer, 0 );
		piece( value, '|', input_buffer, 1 );
		piece( date, '^', key, 0 );

		if ( bank_upload_transaction_insert_credit(
			application_name,
			key,
			value,
			date,
			atof( value ) /* exact_value */ ) )
		{
			/* Just do one */
			/* ----------- */
			break;
		}

		if ( bank_upload_transaction_insert_credit(
				application_name,
				key,
				value,
				date,
				0.0 /* exact_value */ ) )
		{
			/* Just do one */
			/* ----------- */
			break;
		}
	}

	pclose( input_pipe );

} /* bank_upload_transaction_insert_bank_upload_withdrawal() */

void bank_upload_transaction_insert_withdrawal( char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 512 ];
	char *order;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char key[ 256 ];
	char date[ 128 ];
	char value[ 32 ];

	select = "bank_date, bank_description, 0 - bank_amount";
	folder = "bank_upload";
	order = "sequence_number,bank_date";

	sprintf( where,
		 "bank_amount < 0 and %s",
		 bank_upload_bank_date_todo_subquery() );

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=\"%s\"		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=%s		|"
		 "tr '%c' '|'					|"
		 "sed 's/|/^/1'					|"
		 "cat						 ",
		 application_name,
		 select,
		 folder,
		 where,
		 order,
		 FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
fprintf( stderr, "%s/%s()/%d: input_buffer = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
input_buffer );

		piece( key, '|', input_buffer, 0 );
		piece( value, '|', input_buffer, 1 );
		piece( date, '^', key, 0 );

		if ( bank_upload_transaction_insert_credit(
			application_name,
			key,
			value,
			date,
			atof( value ) /* exact_value */ ) )
		{
			/* Just do one */
			/* ----------- */
			break;
		}

		if ( bank_upload_transaction_insert_credit(
			application_name,
			key,
			value,
			date,
			0.0 /* exact_value */ ) )
		{
			/* Just do one */
			/* ----------- */
			break;
		}
	}

	pclose( input_pipe );

} /* bank_upload_transaction_insert_withdrawal() */

void bank_upload_transaction_insert_deposit( char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 512 ];
	char *order;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char key[ 256 ];
	char date[ 128 ];
	char value[ 32 ];

	select = "bank_date, bank_description, bank_amount";
	folder = "bank_upload";

	sprintf( where,
		 "bank_amount > 0 and %s",
		 bank_upload_bank_date_todo_subquery() );

	order = "sequence_number,bank_date";

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=\"%s\"		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=%s		|"
		 "tr '%c' '|'					|"
		 "sed 's/|/^/1'					|"
		 "cat						 ",
		 application_name,
		 select,
		 folder,
		 where,
		 order,
		 FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while ( get_line( input_buffer, input_pipe ) )
	{
		piece( key, '|', input_buffer, 0 );
		piece( value, '|', input_buffer, 1 );
		piece( date, '^', key, 0 );

		if ( bank_upload_transaction_insert_debit(
			application_name,
			key,
			value,
			date,
			atof( value ) /* exact_value */ ) )
		{
			/* Just do one */
			/* ----------- */
			break;
		}

		if ( bank_upload_transaction_insert_debit(
			application_name,
			key,
			value,
			date,
			0.0 /* exact_value */ ) )
		{
			/* Just do one */
			/* ----------- */
			break;
		}
	}

	pclose( input_pipe );

} /* bank_upload_transaction_insert_deposit() */

boolean bank_upload_transaction_insert_credit(
			char *application_name,
			char *key,
			char *value,
			char *date,
			double exact_value )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 512 ];
	char *order;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	DATE *d;
	char *cash_account;
	char exact_where[ 128 ];
	boolean return_value = 0;

	cash_account =
		ledger_get_hard_coded_account_name(
			application_name,
			(char *)0 /* fund_name */,
			LEDGER_CASH_KEY,
			0 /* not warning_only */ );

	if ( exact_value )
	{
		sprintf( exact_where,
			 "ifnull( credit_amount, 0 ) = %.2lf",
			 exact_value );
	}
	else
	{
		strcpy( exact_where, "1 = 1" );
	}

	d = date_yyyy_mm_dd_new( date );
	date_increment_days( d, CASH_LEDGER_DAYS_AGO );

	select =
	"full_name, street_address, transaction_date_time, credit_amount";

	folder = "journal_ledger";

	sprintf( where,
"account = '%s' and ifnull( credit_amount, 0 ) <> 0 and transaction_date_time >= '%s' and %s and %s",
		 cash_account,
		 date_display( d ),
		 bank_upload_full_name_todo_subquery(),
		 exact_where );

	order = "transaction_date_time";

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=\"%s\"		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=%s		|"
		 "tr '%c' '|'					|"
		 "head -%d					|"
		 "sed 's/|/^/1'					|"
		 "sed 's/|/^/1'					|"
		 "keys_match_sum.e %s				|"
		 "cat						 ",
		 application_name,
		 select,
		 folder,
		 where,
		 order,
		 FOLDER_DATA_DELIMITER,
		 TRANSACTIONS_CHECK_COUNT,
		 value );

	input_pipe = popen( sys_string, "r" );

	if ( get_line( input_buffer, input_pipe ) )
	{
		bank_upload_transaction_insert_input_buffer(
			key, input_buffer );

		return_value = 1;
	}

	pclose( input_pipe );

	return return_value;

} /* bank_upload_transaction_insert_credit() */

boolean bank_upload_transaction_insert_debit(
			char *application_name,
			char *key,
			char *value,
			char *date,
			double exact_value )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 512 ];
	char *order;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	DATE *d;
	char *cash_account;
	char exact_where[ 128 ];
	boolean return_value = 0;

	cash_account =
		ledger_get_hard_coded_account_name(
			application_name,
			(char *)0 /* fund_name */,
			LEDGER_CASH_KEY,
			0 /* not warning_only */ );

	if ( exact_value )
	{
		sprintf( exact_where,
			 "ifnull( credit_amount, 0 ) = %.2lf",
			 exact_value );
	}
	else
	{
		strcpy( exact_where, "1 = 1" );
	}


	d = date_yyyy_mm_dd_new( date );
	date_increment_days( d, CASH_LEDGER_DAYS_AGO );

	select =
	"full_name, street_address, transaction_date_time, debit_amount";

	folder = "journal_ledger";

	sprintf( where,
"account = '%s' and ifnull( debit_amount, 0 ) <> 0 and transaction_date_time >= '%s' and %s and %s",
		 cash_account,
		 date_display( d ),
		 bank_upload_full_name_todo_subquery(),
		 exact_where );

	order = "transaction_date_time";

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=\"%s\"		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=%s		|"
		 "tr '%c' '|'					|"
		 "head -%d					|"
		 "sed 's/|/^/1'					|"
		 "sed 's/|/^/1'					|"
		 "keys_match_sum.e %s				|"
		 "cat						 ",
		 application_name,
		 select,
		 folder,
		 where,
		 order,
		 FOLDER_DATA_DELIMITER,
		 TRANSACTIONS_CHECK_COUNT,
		 value );

	input_pipe = popen( sys_string, "r" );

	if ( get_line( input_buffer, input_pipe ) )
	{
		bank_upload_transaction_insert_input_buffer(
			key, input_buffer );

		return_value = 1;
	}

	pclose( input_pipe );

	return return_value;

} /* bank_upload_transaction_insert_debit() */

void bank_upload_transaction_insert_input_buffer(
	/* --------------------------------- */
	/* key is bank_date^bank_description */
	/* --------------------------------- */
	char *key,
	/* -------------------------------------------------------------- */
	/* input_buffer is full_name^street_address^transaction_date_time */
	/* input_buffer optionally appends '[|another]'			  */
	/* -------------------------------------------------------------- */
	char *input_buffer )
{
	char sys_string[ 1024 ];
	char piece_buffer[ 256 ];
	int p;
	FILE *output_pipe;
	char *table;
	char *field;

	table = "bank_upload_transaction";

	field =
"bank_date,bank_description,full_name,street_address,transaction_date_time";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s del='^'",
		 table,
		 field );

	output_pipe = popen( sys_string, "w" );

	for (	p = 0;
		piece( piece_buffer, '|', input_buffer, p );
		p++ )
	{
		fprintf( output_pipe,
			 "%s^%s\n",
			 key,
			 piece_buffer );
	}

	pclose( output_pipe );

} /* bank_upload_transaction_insert_input_buffer() */

void bank_upload_transaction_direct_insert(
				char *bank_date,
				char *bank_description,
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	char key[ 1024 ];
	char input_buffer[ 1024 ];

	sprintf( key, "%s^%s", bank_date, bank_description );

	sprintf(	input_buffer,
			"%s^%s^%s",
			full_name,
			street_address,
			transaction_date_time );

	bank_upload_transaction_insert_input_buffer( key, input_buffer );

} /* bank_upload_transaction_direct_insert() */

