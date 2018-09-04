/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/reoccurring.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "timlib.h"
#include "date_convert.h"
#include "piece.h"
#include "appaserver_library.h"
#include "html_table.h"
#include "reoccurring.h"

REOCCURRING_STRUCTURE *reoccurring_calloc( void )
{
	REOCCURRING_STRUCTURE *p =
		(REOCCURRING_STRUCTURE *)
			calloc( 1, sizeof( REOCCURRING_STRUCTURE ) );

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

} /* reoccurring_calloc() */

REOCCURRING_STRUCTURE_STRUCTURE *reoccurring_structure_new( void )
{
	REOCCURRING_STRUCTURE_STRUCTURE *p =
		(REOCCURRING_STRUCTURE_STRUCTURE *)
			calloc( 1, sizeof( REOCCURRING_STRUCTURE_STRUCTURE ) );

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

} /* reoccurring_structure_new() */

REOCCURRING_TRANSACTION *reoccurring_reoccurring_transaction_calloc( void )
{
	REOCCURRING_TRANSACTION *p;

	if ( ! ( p = calloc( 1, sizeof( REOCCURRING_TRANSACTION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return p;
}

REOCCURRING_TRANSACTION *reoccurring_reoccurring_transaction_new(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_description,
					double transaction_amount )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;

	reoccurring_transaction = reoccurring_reoccurring_transaction_calloc();

	reoccurring_transaction->full_name = full_name;
	reoccurring_transaction->street_address = street_address;

	reoccurring_transaction->transaction_description =
		transaction_description;

	if ( !reoccurring_reoccurring_transaction_load(
			&reoccurring_transaction->debit_account,
			&reoccurring_transaction->credit_account,
			&reoccurring_transaction->transaction_amount,
			&reoccurring_transaction->accrued_daily_amount,
			&reoccurring_transaction->accrued_monthly_amount,
			application_name,
			reoccurring_transaction->full_name,
			reoccurring_transaction->street_address,
			reoccurring_transaction->transaction_description ) )
	{
		return (REOCCURRING_TRANSACTION *)0;
	}

	if ( !timlib_dollar_virtually_same( transaction_amount, 0.0 ) )
	{
		reoccurring_transaction->transaction_amount =
			transaction_amount;
	}

	return reoccurring_transaction;

} /* reoccurring_reoccurring_transaction_new() */

boolean reoccurring_reoccurring_transaction_load(
				char **debit_account,
				char **credit_account,
				double *transaction_amount,
				double *accrued_daily_amount,
				double *accrued_monthly_amount,
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_description )
{
	char sys_string[ 1024 ];
	char *select;
	char where[ 512 ];
	char *results;
	char entity_buffer[ 128 ];
	char description_buffer[ 128 ];
	REOCCURRING_TRANSACTION *reoccurring_transaction;

	select = reoccurring_reoccurring_transaction_get_select();

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "transaction_description = '%s'	",
		 escape_character(	entity_buffer,
					full_name,
					'\'' ),
		 street_address,
		 escape_character(	description_buffer,
					transaction_description,
					'\'' ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 REOCCURRING_TRANSACTION_FOLDER_NAME,
		 where );

	results = pipe2string( sys_string );

	if ( !results ) return 0;

	reoccurring_transaction = reoccurring_reoccurring_transaction_calloc();

	reoccurring_reoccurring_transaction_parse(
			&reoccurring_transaction->
				full_name,
			&reoccurring_transaction->
				street_address,
			&reoccurring_transaction->
				debit_account,
			&reoccurring_transaction->
				credit_account,
			&reoccurring_transaction->
				transaction_amount,
			&reoccurring_transaction->
				bank_upload_search_phrase,
			&reoccurring_transaction->
				accrued_daily_amount,
			&reoccurring_transaction->
				accrued_monthly_amount,
			results );

	*debit_account = reoccurring_transaction->debit_account;
	*credit_account = reoccurring_transaction->credit_account;
	*transaction_amount = reoccurring_transaction->transaction_amount;
	*accrued_daily_amount = reoccurring_transaction->accrued_daily_amount;

	*accrued_monthly_amount =
		reoccurring_transaction->
			accrued_monthly_amount;

	return 1;

} /* reoccurring_reoccurring_transaction_load() */

LIST *reoccurring_spreadsheet_get_list(
				char *input_filename,
				char **minimum_bank_date,
				char *application_name,
				int date_piece_offset,
				int description_piece_offset,
				int debit_piece_offset,
				int credit_piece_offset,
				int balance_piece_offset,
				int starting_sequence_number,
				char *fund_name )
{
	char input_string[ 4096 ];
	char bank_date[ 128 ];
	char bank_date_international[ 128 ];
	char bank_description[ 1024 ];
	char bank_amount[ 128 ];
	char bank_balance[ 128 ];
	boolean found_header = 0;
	static char local_minimum_bank_date[ 16 ] = {0};
	FILE *input_file;
	REOCCURRING_STRUCTURE *reoccurring;
	LIST *reoccurring_list;
	boolean exists_fund;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			input_filename );
		exit( 1 );
	}

	exists_fund = ledger_fund_attribute_exists( application_name );

	if ( minimum_bank_date )
	{
		*minimum_bank_date = local_minimum_bank_date;
	}

	reoccurring_list = list_new();

	while( timlib_get_line( input_string, input_file, 4096 ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		timlib_remove_character( input_string, '\\' );

		/* Get bank_date */
		/* ------------- */
		if ( !piece_quote_comma(
				bank_date,
				input_string,
				date_piece_offset ) )
		{
			continue;
		}

		if ( !found_header )
		{
			if ( timlib_exists_string( bank_date, "date" ) )
			{
				found_header = 1;
			}
			continue;
		}

		if ( !reoccurring_get_bank_date_international(
				bank_date_international,
				bank_date ) )
		{
			continue;
		}

		if ( !*local_minimum_bank_date )
		{
			strcpy(	local_minimum_bank_date,
				bank_date_international );
		}
		else
		{
			if ( strcmp( 
				local_minimum_bank_date,
				bank_date_international ) < 0 )
			{
				strcpy(	local_minimum_bank_date,
					bank_date_international );
			}
		}

		/* Get bank_description */
		/* -------------------- */
		if ( !piece_quote_comma(
				bank_description,
				input_string,
				description_piece_offset ) )
		{
			continue;
		}

		if ( exists_fund )
		{
			if ( timlib_strcmp(
				bank_description,
				"interest earned" ) == 0
			||   timlib_strcmp(
				bank_description,
				"deposit" ) == 0 )
			{
				sprintf(
				bank_description + strlen( bank_description ),
			 	" %s",
			 	fund_name );
			}
		}

		/* =============== */
		/* Get bank_amount */
		/* =============== */

		/* The bank_amount is either a single column or two columns. */
		/* --------------------------------------------------------- */
		if ( !piece_quote_comma(
				bank_amount,
				input_string,
				debit_piece_offset ) )
		{
			continue;
		}

		if ( !atof( bank_amount ) )
		{
			/* See if there's a second column. */
			/* ------------------------------- */
			if ( credit_piece_offset < 0 )
			{
				continue;
			}

			if ( !piece_quote_comma(
					bank_amount,
					input_string,
					credit_piece_offset ) )
			{
				continue;
			}
		}

		if ( !atof( bank_amount ) ) continue;

		/* Get bank_balance */
		/* ---------------- */
		if ( balance_piece_offset >= 0 )
		{
			piece_quote_comma(
				bank_balance,
				input_string,
				balance_piece_offset );
		}

		reoccurring = reoccurring_calloc();

		reoccurring->bank_date = strdup( bank_date_international );
		reoccurring->bank_description = strdup( bank_description );
		reoccurring->sequence_number = starting_sequence_number;
		reoccurring->bank_amount = atof( bank_amount );;
		reoccurring->bank_running_balance = atof( bank_balance );;

		list_append_pointer( reoccurring_list, reoccurring );
		starting_sequence_number++;
	}

	fclose( input_file );
	return reoccurring_list;

} /* reoccurring_spreadsheet_get_list() */

/* Returns table_insert_count */
/* -------------------------- */
int reoccurring_insert(			char *application_name,
					LIST *reoccurring_list,
					char *fund_name )
{
	char sys_string[ 1024 ];
	FILE *reoccurring_insert_pipe = {0};
	FILE *reoccurring_archive_insert_pipe = {0};
	int table_insert_count = 0;
	char error_filename[ 128 ] = {0};
	char *insert_reoccurring;
	char *insert_reoccurring_archive;
	REOCCURRING_STRUCTURE *reoccurring;
	int error_file_lines;
	boolean exists_fund;
	char *table_name;

	if ( !list_rewind( reoccurring_list ) ) return 0;

	exists_fund = ledger_fund_attribute_exists( application_name );

	if ( exists_fund )
	{
		insert_reoccurring = INSERT_REOCCURRING_STRUCTURE_FUND;
		insert_reoccurring_archive = INSERT_REOCCURRING_STRUCTURE_ARCHIVE_FUND;
	}
	else
	{
		insert_reoccurring = INSERT_REOCCURRING_STRUCTURE;
		insert_reoccurring_archive = INSERT_REOCCURRING_STRUCTURE_ARCHIVE;
	}

	/* Open reoccurring_insert_pipe */
	/* ---------------------------- */
	table_name =
		get_table_name(	application_name,
				"reoccurring" );

	sprintf(	error_filename,
			"/tmp/%s_%d.err",
			table_name,
			getpid() );

	sprintf( sys_string,
	 	 "insert_statement table=%s field=%s del='%c' 		  |"
	 	 "sql.e 2>&1						  |"
	 	 "cat > %s						   ",
	 	table_name,
	 	insert_reoccurring,
	 	FOLDER_DATA_DELIMITER,
		error_filename );

	reoccurring_insert_pipe = popen( sys_string, "w" );

	/* Open reoccurring_archive_insert_pipe */
	/* ------------------------------------ */
	table_name =
		get_table_name(	application_name,
				"reoccurring_archive" );

	sprintf( sys_string,
		 "insert_statement table=%s field=%s del='%c' 		  |"
		 "sql.e							  |"
		 "cat							   ",
		 	table_name,
		 	insert_reoccurring_archive,
		 	FOLDER_DATA_DELIMITER );

	reoccurring_archive_insert_pipe = popen( sys_string, "w" );

	do {
		reoccurring = list_get_pointer( reoccurring_list );

		/* Output insert into REOCCURRING_STRUCTURE */
		/* ------------------------------ */
		if ( exists_fund )
		{
			fprintf(reoccurring_insert_pipe,
		 		"%s^%s^%d^%.2lf^%s\n",
		 		reoccurring->bank_date,
		 		reoccurring->bank_description,
				reoccurring->sequence_number,
		 		reoccurring->bank_amount,
				fund_name );
		}
		else
		{
			fprintf(reoccurring_insert_pipe,
		 		"%s^%s^%d^%.2lf\n",
		 		reoccurring->bank_date,
		 		reoccurring->bank_description,
				reoccurring->sequence_number,
		 		reoccurring->bank_amount );
		}

		/* Output insert into REOCCURRING_STRUCTURE_ARCHIVE */
		/* -------------------------------------- */
		if ( exists_fund )
		{
			fprintf(reoccurring_archive_insert_pipe,
		 		"%s^%s^%d^%.2lf^%.2lf^%s\n",
		 		reoccurring->bank_date,
		 		reoccurring->bank_description,
				reoccurring->sequence_number,
		 		reoccurring->bank_amount,
		 		reoccurring->bank_running_balance,
				fund_name );
		}
		else
		{
			fprintf(reoccurring_archive_insert_pipe,
		 		"%s^%s^%d^%.2lf^%.2lf\n",
		 		reoccurring->bank_date,
		 		reoccurring->bank_description,
				reoccurring->sequence_number,
		 		reoccurring->bank_amount,
				reoccurring->bank_running_balance );
		}

		table_insert_count++;

	} while( list_next( reoccurring_list ) );

	pclose( reoccurring_insert_pipe );
	pclose( reoccurring_archive_insert_pipe );

	sprintf( sys_string,
		 "wc -l %s",
		 error_filename );

	error_file_lines = atoi( pipe2string( sys_string ) );
	table_insert_count -= error_file_lines;

	if ( error_file_lines )
	{
		printf( "<h3>Error records.</h3>\n" );
		fflush( stdout );

		sprintf( sys_string,
			 "cat %s | html_paragraph_wrapper.e",
			 error_filename );

		system( sys_string );
	}

	sprintf( sys_string,
		 "rm -f %s",
		 error_filename );

	system( sys_string );

	return table_insert_count;

} /* reoccurring_insert() */

int reoccurring_get_starting_sequence_number(
			char *application_name,
			char *input_filename,
			int date_piece_offset )
{
	int line_count;
	char sys_string[ 1024 ];

	line_count =
		reoccurring_get_line_count(
			input_filename,
			date_piece_offset );

	sprintf( sys_string,
		 "reference_number.sh %s %d",
		 application_name,
		 line_count );

	return atoi( pipe2string( sys_string ) );

} /* reoccurring_get_starting_sequence_number() */

int reoccurring_get_line_count(	char *input_filename,
				int date_piece_offset )
{
	char input_string[ 4096 ];
	char bank_date[ 128 ];
	FILE *input_file;
	int line_count = 0;
	boolean found_header = 0;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filename );
		exit( 1 );
	}

	while( timlib_get_line( input_string, input_file, 4096 ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_quote_comma(
				bank_date,
				input_string,
				date_piece_offset ) )
		{
			continue;
		}

		if ( !found_header )
		{
			if ( strcmp( bank_date, "Date" ) == 0 )
			{
				found_header = 1;
			}
			continue;
		}

		line_count++;
	}

	fclose( input_file );
	return line_count;

} /* reoccurring_get_line_count() */

boolean reoccurring_get_bank_date_international(
				char *bank_date_international,
				char *bank_date )
{
	date_convert_source_american(
		bank_date_international,
		international,
		bank_date );

	return date_convert_is_valid_international(
		bank_date_international );

} /* reoccurring_get_bank_date_international() */

char *reoccurring_get_select( void )
{
	char *select;

	select =	"bank_date,		"
			"bank_description,	"
			"sequence_number,	"
			"bank_amount,		"
			"bank_running_balance	";

	return select;
}

REOCCURRING_STRUCTURE *reoccurring_fetch(		char *application_name,
					char *bank_date,
					char *bank_description )
{
	REOCCURRING_STRUCTURE *reoccurring;
	char *select;
	char where[ 512 ];
	char description_buffer[ 512 ];
	char sys_string[ 1024 ];
	char *results;

	select = reoccurring_get_select();

	sprintf( where,
		 "bank_date = '%s' and			"
		 "bank_description = '%s'		",
		 bank_date,
		 escape_character(	description_buffer,
					bank_description,
					'\'' ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=reoccurring	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	if ( !( results = pipe2string( sys_string ) ) )
		return (REOCCURRING_STRUCTURE *)0;

	reoccurring = reoccurring_calloc();

	reoccurring_fetch_parse(
			&reoccurring->bank_date,
			&reoccurring->bank_description,
			&reoccurring->sequence_number,
			&reoccurring->bank_amount,
			&reoccurring->bank_running_balance,
			results );

	return reoccurring;

} /* reoccurring_fetch() */

LIST *reoccurring_fetch_list(		char *application_name,
					int starting_sequence_number )
{
	LIST *reoccurring_list;
	REOCCURRING_STRUCTURE *reoccurring;
	char *select;
	char where[ 128 ];
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;

	reoccurring_list = list_new();
	select = reoccurring_get_select();

	sprintf( where, "sequence_number >= %d", starting_sequence_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=reoccurring	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		reoccurring = reoccurring_calloc();

		reoccurring_fetch_parse(
				&reoccurring->bank_date,
				&reoccurring->bank_description,
				&reoccurring->sequence_number,
				&reoccurring->bank_amount,
				&reoccurring->bank_running_balance,
				input_buffer );

		list_append_pointer( reoccurring_list, reoccurring );
	}

	pclose( input_pipe );

	return reoccurring_list;

} /* reoccurring_fetch_list() */

void reoccurring_fetch_parse(	char **bank_date,
				char **bank_description,
				int *sequence_number,
				double *bank_amount,
				double *bank_running_balance,
				char *input_buffer )
{
	char buffer[ 128 ];

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	if ( *buffer )
		*bank_date = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	if ( *buffer )
		*bank_description = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *buffer )
		*sequence_number = atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *buffer )
		*bank_amount = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *buffer )
		*bank_running_balance = atof( buffer );

} /* reoccurring_fetch_parse() */

void reoccurring_reoccurring_transaction_parse(
					char **full_name,
					char **street_address,
					char **debit_account,
					char **credit_account,
					double *transaction_amount,
					char **bank_upload_search_phrase,
					double *accrued_daily_amount,
					double *accrued_monthly_amount,
					char *input_buffer )
{
	char buffer[ 256 ];

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	if ( *buffer )
		*full_name = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	if ( *buffer )
		*street_address = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *buffer )
		*debit_account = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *buffer )
		*credit_account = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	if ( *buffer )
		*transaction_amount = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *buffer )
		*bank_upload_search_phrase = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *buffer )
		*accrued_daily_amount = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	if ( *buffer )
		*accrued_monthly_amount = atof( buffer );

} /* reoccurring_reoccurring_transaction_parse() */

LIST *reoccurring_fetch_existing_cash_journal_ledger_list(
					char *application_name,
					char *minimum_bank_date,
					char *fund_name )
{
	LIST *existing_cash_journal_ledger_list = {0};
	char *cash_account_name;

	cash_account_name =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			LEDGER_CASH_KEY,
			0 /* not warning_only */ );

	existing_cash_journal_ledger_list =
		ledger_get_journal_ledger_list(
				application_name,
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				minimum_bank_date
					/* minimum_transaction_date_time */,
				cash_account_name );

	return existing_cash_journal_ledger_list;

} /* reoccurring_fetch_existing_cash_journal_ledger_list() */

char *reoccurring_reoccurring_transaction_get_select( void )
{
	char *select;

	select = "full_name,			"
		 "street_address,		"
		 "debit_account,		"
		 "credit_account,		"
		 "transaction_amount,		"
		 "bank_upload_search_phrase,	"
		 "accrued_daily_amount,		"
		 "accrued_monthly_amount	";

	return select;
}

LIST *reoccurring_fetch_reoccurring_transaction_list(
					char *application_name )
{
	LIST *reoccurring_transaction_list;
	REOCCURRING_TRANSACTION *reoccurring_transaction;
	char *select;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char *where;

	reoccurring_transaction_list = list_new();

	select = reoccurring_reoccurring_transaction_get_select();

	where = "bank_upload_search_phrase is not null";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=reoccurring_transaction	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		reoccurring_transaction =
			reoccurring_reoccurring_transaction_calloc();

		reoccurring_reoccurring_transaction_parse(
				&reoccurring_transaction->
					full_name,
				&reoccurring_transaction->
					street_address,
				&reoccurring_transaction->
					debit_account,
				&reoccurring_transaction->
					credit_account,
				&reoccurring_transaction->
					transaction_amount,
				&reoccurring_transaction->
					bank_upload_search_phrase,
				&reoccurring_transaction->
					accrued_daily_amount,
				&reoccurring_transaction->
					accrued_monthly_amount,
				input_buffer );

		list_append_pointer(
			reoccurring_transaction_list,
			reoccurring_transaction );
	}

	pclose( input_pipe );

	return reoccurring_transaction_list;

} /* reoccurring_fetch_reoccurring_transaction_list() */

REOCCURRING_TRANSACTION *reoccurring_seek_bank_upload_search_phrase(
				LIST *reoccurring_transaction_list,
				char *bank_description )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;

	if ( !list_rewind( reoccurring_transaction_list ) )
		return (REOCCURRING_TRANSACTION *)0;

	do {
		reoccurring_transaction =
			list_get(
				reoccurring_transaction_list );

		if ( timlib_exists_string(
			bank_description /* string */,
			reoccurring_transaction->
				bank_upload_search_phrase
					/* substring */ ) )
		{
			return reoccurring_transaction;
		}

	} while( list_next( reoccurring_transaction_list ) );

	return (REOCCURRING_TRANSACTION *)0;

} /* reoccurring_seek_bank_upload_search_phrase() */

void reoccurring_set_transaction(
				LIST *reoccurring_list,
				LIST *reoccurring_transaction_list,
				LIST *existing_cash_journal_ledger_list )
{
	REOCCURRING_STRUCTURE *reoccurring;
	REOCCURRING_TRANSACTION *reoccurring_transaction;
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( reoccurring_list ) ) return;

	do {
		reoccurring = list_get( reoccurring_list );

		if ( ! ( reoccurring_transaction =
				reoccurring_seek_bank_upload_search_phrase(
					reoccurring_transaction_list,
					reoccurring->bank_description ) ) )
		{
			continue;
		}

		if ( ledger_exists_journal_ledger(
				existing_cash_journal_ledger_list,
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				reoccurring->bank_date
					/* transaction_date */,
				reoccurring->bank_amount
					/* transaction_amount */ ) )
		{
			continue;
		}

		transaction =
			ledger_transaction_new(
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				ledger_get_transaction_date_time(
					reoccurring->bank_date
						/* transaction_date */ ),
				(char *)0 /* memo */ );

		transaction->transaction_amount =
			float_abs( reoccurring->bank_amount );

		reoccurring->transaction = transaction;
		transaction->journal_ledger_list = list_new();

		/* Set the debit account */
		/* --------------------- */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				reoccurring_transaction->debit_account );

		journal_ledger->debit_amount = transaction->transaction_amount;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		/* Set the credit account */
		/* ---------------------- */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				reoccurring_transaction->credit_account );

		journal_ledger->credit_amount = transaction->transaction_amount;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

	} while( list_next( reoccurring_list ) );

} /* reoccurring_set_transaction() */

void reoccurring_insert_transaction(
					char *application_name,
					LIST *reoccurring_list )
{
	REOCCURRING_STRUCTURE *reoccurring;
	TRANSACTION *transaction;

	if ( !list_rewind( reoccurring_list ) ) return;

	do {
		reoccurring = list_get( reoccurring_list );

		if ( !reoccurring->transaction ) continue;

		transaction = reoccurring->transaction;

		transaction->transaction_date_time =
			ledger_transaction_journal_ledger_insert(
				application_name,
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->transaction_amount,
				transaction->memo,
				transaction->check_number,
				transaction->lock_transaction,
				transaction->journal_ledger_list );

	} while( list_next( reoccurring_list ) );

} /* reoccurring_insert_transaction() */

void reoccurring_transaction_display( LIST *reoccurring_list )
{
	REOCCURRING_STRUCTURE *reoccurring;
	char transaction_memo[ 256 ];

	if ( !list_rewind( reoccurring_list ) ) return;

	do {
		reoccurring = list_get( reoccurring_list );

		if ( reoccurring->transaction )
		{
			sprintf(transaction_memo,
				"%s/%s",
				reoccurring->transaction->full_name,
				reoccurring->transaction->street_address );

			format_initial_capital(
				transaction_memo,
				transaction_memo );

			ledger_list_html_display(
				transaction_memo,
				reoccurring->
					transaction->
					journal_ledger_list );
		}

	} while( list_next( reoccurring_list ) );

} /* reoccurring_transaction_display() */

REOCCURRING_STRUCTURE *reoccurring_dictionary_extract(
				char *application_name,
				DICTIONARY *dictionary )
{
	char *key;
	REOCCURRING_STRUCTURE *reoccurring;
	char *bank_date;
	char *bank_description;

	key = "bank_date";

	if ( ! ( bank_date = dictionary_fetch( dictionary, key ) ) )
	{
		return (REOCCURRING_STRUCTURE *)0;
	}

	key = "bank_description";

	if ( ! ( bank_description =
			dictionary_fetch( dictionary, key ) ) )
	{
		return (REOCCURRING_STRUCTURE *)0;
	}

	reoccurring =
		reoccurring_fetch(
			application_name,
			bank_date,
			bank_description );

	return reoccurring;

} /* reoccurring_dictionary_extract() */

