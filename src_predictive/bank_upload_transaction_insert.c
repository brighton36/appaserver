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
#include <unistd.h>
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

/* Global variables */
/* ---------------- */
enum bank_upload_exception bank_upload_exception = {0};

/* Prototypes */
/* ---------- */
void seek_withdrawal(		char *application_name );

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
/*
		bank_upload_transaction_insert_deposit( application_name );
*/
	}

	if ( strcmp( operation, "withdrawal" ) == 0
	||   strcmp( operation, "both" ) == 0 )
	{
		seek_withdrawal( application_name );
	}
	else
	{
		delimiter_count =
			timlib_count_delimiters(
				'^', operation );

		/* Doesn't do the propagate */
		/* ------------------------ */
		if ( delimiter_count == 1 )
		{
			double bank_amount;
			BANK_UPLOAD *bank_upload;

			piece( bank_date, '^', operation, 0 );

			piece( bank_description, '^', operation, 1 );
			bank_upload_description_crop( bank_description );

			bank_amount =
				bank_upload_fetch_bank_amount(
					application_name,
					bank_date,
					bank_description );

			bank_upload =
				bank_upload_new(
					strdup( bank_date ),
					strdup( bank_description ) );

			bank_upload->bank_amount = bank_amount;

			bank_upload->reconciled_transaction_list =
				bank_upload_get_reconciled_transaction_list(
				   application_name,
				   bank_upload->bank_date,
				   bank_upload->bank_description,
				   bank_upload->bank_amount,
				   (LIST *)0
				       /* uncleared_check_transaction_list */ );

			if ( list_length( bank_upload->
						reconciled_transaction_list ) )
			{
				bank_upload_transaction_insert(
					bank_upload->bank_date,
					bank_upload->bank_description,
					bank_upload->
						reconciled_transaction_list );
			}
		}
		else
		/* ------------------------ */
		/* Doesn't do the propagate */
		/* ------------------------ */
		if ( delimiter_count == 4 )
		{
			piece( bank_date, '^', operation, 0 );

			piece( bank_description, '^', operation, 1 );
			bank_upload_description_crop( bank_description );

			piece( full_name, '^', operation, 2 );
			piece( street_address, '^', operation, 3 );
			piece( transaction_date_time, '^', operation, 4 );

			bank_upload_transaction_direct_insert(
				application_name,
				bank_date,
				bank_description,
				full_name,
				street_address,
				transaction_date_time );
		}
	}

	return 0;

} /* main() */

void seek_withdrawal( char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 512 ];
	char *order;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char bank_date[ 128 ];
	char bank_description[ 256 ];
	char bank_amount[ 32 ];
	BANK_UPLOAD *bank_upload;

	select = "bank_date, bank_description, bank_amount";
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
		 "			order=%s		 ",
		 application_name,
		 select,
		 folder,
		 where,
		 order );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( bank_date, FOLDER_DATA_DELIMITER, input_buffer, 0 );

		piece(	bank_description,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		bank_upload =
			bank_upload_new(
				strdup( bank_date ),
				strdup( bank_description ) );

		piece(	bank_amount,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		bank_upload->bank_amount = atof( bank_amount );

		bank_upload->reconciled_transaction_list =
			bank_upload_get_reconciled_transaction_list(
				application_name,
				bank_upload->bank_date,
				bank_upload->bank_description,
				bank_upload->bank_amount,
				( LIST *)0
				       /* uncleared_check_transaction_list */ );

		if ( list_length( bank_upload->reconciled_transaction_list ) )
		{
			bank_upload_transaction_insert(
				bank_upload->bank_date,
				bank_upload->bank_description,
				bank_upload->reconciled_transaction_list );

			/* Just do one */
			/* ----------- */
			break;
		}
	}

	pclose( input_pipe );

} /* seek_withdrawal() */

