/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/insert_bank_upload_transaction.c	*/
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
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"

/* Constants */
/* --------- */
#define CASH_LEDGER_DAYS_AGO		-40.0
#define TRANSACTIONS_CHECK_COUNT	20

/* Prototypes */
/* ---------- */
char *bank_upload_bank_date_todo_subquery( void );

char *bank_upload_full_name_todo_subquery( void );

void insert_bank_upload_transaction_debit(
			char *application_name,
			char *key,
			char *value,
			char *date );

void insert_bank_upload_transaction_credit(
			char *application_name,
			char *key,
			char *value,
			char *date );

void insert_bank_upload_transaction_input_buffer(
			char *key,
			char *input_buffer );

void insert_bank_upload_transaction_deposit(
			char *application_name );

void insert_bank_upload_transaction_withdrawal(
			char *application_name );

int main( int argc, char **argv )
{
	char *application_name;

	if ( argc ){};

	application_name = environ_get_application_name( argv[ 0 ] );

	insert_bank_upload_transaction_deposit( application_name );
	insert_bank_upload_transaction_withdrawal( application_name );

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

void insert_bank_upload_transaction_withdrawal( char *application_name )
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
		piece( key, '|', input_buffer, 0 );
		piece( value, '|', input_buffer, 1 );
		piece( date, '^', key, 0 );

		insert_bank_upload_transaction_credit(
			application_name,
			key,
			value,
			date );
	}

	pclose( input_pipe );

} /* insert_bank_upload_transaction_withdrawal() */

void insert_bank_upload_transaction_deposit( char *application_name )
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

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( key, '|', input_buffer, 0 );
		piece( value, '|', input_buffer, 1 );
		piece( date, '^', key, 0 );

		insert_bank_upload_transaction_debit(
			application_name,
			key,
			value,
			date );
	}

	pclose( input_pipe );

} /* insert_bank_upload_transaction_deposit() */

void insert_bank_upload_transaction_credit(
			char *application_name,
			char *key,
			char *value,
			char *date )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 512 ];
	char *order;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	DATE *d;

	d = date_yyyy_mm_dd_new( date );
	date_increment_days( d, CASH_LEDGER_DAYS_AGO );

	select =
	"full_name, street_address, transaction_date_time, credit_amount";

	folder = "journal_ledger";

	sprintf( where,
"account = 'bank_of_america_checking' and ifnull( credit_amount, 0 ) <> 0 and transaction_date_time >= '%s' and %s",
		 date_display( d ),
		 bank_upload_full_name_todo_subquery() );

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
		insert_bank_upload_transaction_input_buffer(
			key, input_buffer );
	}

	pclose( input_pipe );

} /* insert_bank_upload_transaction_credit() */

void insert_bank_upload_transaction_debit(
			char *application_name,
			char *key,
			char *value,
			char *date )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 512 ];
	char *order;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	DATE *d;

	d = date_yyyy_mm_dd_new( date );
	date_increment_days( d, CASH_LEDGER_DAYS_AGO );

	select =
	"full_name, street_address, transaction_date_time, debit_amount";

	folder = "journal_ledger";

	sprintf( where,
"account = 'bank_of_america_checking' and ifnull( debit_amount, 0 ) <> 0 and transaction_date_time >= '%s' and %s",
		 date_display( d ),
		 bank_upload_full_name_todo_subquery() );

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
		insert_bank_upload_transaction_input_buffer(
			key, input_buffer );
	}

	pclose( input_pipe );

} /* insert_bank_upload_transaction_debit() */

void insert_bank_upload_transaction_input_buffer(
			char *key,
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

	/* --------------------------------------------------	*/
	/* Sample input_buffer =				*/
	/* bank_of_america^null^2017-12-29 14:09:22[|another]	*/
	/* --------------------------------------------------	*/
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

} /* insert_bank_upload_transaction_input_buffer() */


