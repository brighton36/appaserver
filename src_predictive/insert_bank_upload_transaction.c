/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/insert_bank_upload_transaction.c	*/
/* ----------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void insert_bank_upload_transaction_debit(
			char *application_name,
			char *key,
			char *value,
			char *date );

void insert_bank_upload_transaction_input_buffer(
			char *key,
			char *input_buffer );

void insert_bank_upload_transaction_deposit(
			char *application_name );

int main( int argc, char **argv )
{
	char *application_name;

	if ( argc ){};

	application_name = environ_get_application_name( argv[ 0 ] );

	insert_bank_upload_transaction_deposit( application_name );

	return 0;

} /* main() */

void insert_bank_upload_transaction_deposit( char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char *where;
	char *order;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char key[ 256 ];
	char date[ 128 ];
	char value[ 32 ];

	select = "bank_date, bank_description, bank_amount";
	folder = "bank_upload";
	where = "bank_amount > 0";
	order = "bank_date";
	
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

	select =
	"full_name, street_address, transaction_date_time, debit_amount";

	folder = "journal_ledger";

	sprintf( where,
"account = 'bank_of_america_checking' and debit_amount is not null and transaction_date_time >= '%s'",
		 date );

	order = "transaction_date_time";

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=\"%s\"		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=%s		|"
		 "tr '%c' '|'					|"
		 "head -20					|"
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
	FILE *output_pipe;
	int c;
	int i;
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

	/* -------------------------------------------- */
	/* Sample input_buffer =			*/
	/* bank_of_america^null^2017-12-29 14:09:22	*/
	/* -------------------------------------------- */
	c = timlib_character_count( '^', input_buffer );

	i = c / 2;

	if ( i == 1 )
	{
		fprintf( output_pipe,
			 "%s^%s\n",
			 key,
			 input_buffer );
	}
	else
	{
		fprintf( stderr,
			 "Warning: need to output multiple rows: %s.\n",
			 key );
	}

	pclose( output_pipe );

} /* insert_bank_upload_transaction_input_buffer() */


