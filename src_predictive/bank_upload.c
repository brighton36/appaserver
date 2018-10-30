/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/bank_upload.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "date_convert.h"
#include "piece.h"
#include "appaserver_library.h"
#include "html_table.h"
#include "bank_upload.h"
#include "reoccurring.h"

BANK_UPLOAD *bank_upload_calloc( void )
{
	BANK_UPLOAD *p =
		(BANK_UPLOAD *)
			calloc( 1, sizeof( BANK_UPLOAD ) );

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

} /* bank_upload_calloc() */

BANK_UPLOAD_STRUCTURE *bank_upload_structure_new(
				char *application_name,
				char *fund_name,
				char *input_filename,
				int date_piece_offset,
				int description_piece_offset,
				int debit_piece_offset,
				int credit_piece_offset,
				int balance_piece_offset )
{
	extern enum bank_upload_exception bank_upload_exception;

	BANK_UPLOAD_STRUCTURE *p =
		(BANK_UPLOAD_STRUCTURE *)
			calloc( 1, sizeof( BANK_UPLOAD_STRUCTURE ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	p->fund_name = fund_name;

	p->file.input_filename = input_filename;
	p->file.date_piece_offset = date_piece_offset;
	p->file.description_piece_offset = description_piece_offset;
	p->file.debit_piece_offset = debit_piece_offset;
	p->file.credit_piece_offset = credit_piece_offset;
	p->file.balance_piece_offset = balance_piece_offset;

	if ( ! ( p->starting_sequence_number =
			bank_upload_get_starting_sequence_number(
				application_name,
				p->file.input_filename,
				p->file.date_piece_offset ) ) )
	{
		char *msg;

		msg = "<h2>ERROR: cannot get sequence number</h2>";

		fprintf( stderr,
			 "%s/%s()/%d: %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 msg );

		printf( "%s\n", msg );

		bank_upload_exception = sequence_number_not_generated;

		return (BANK_UPLOAD_STRUCTURE *)0;
	}

	p->file.bank_upload_file_list =
		bank_upload_fetch_file_list(
			&p->file.file_sha256sum,
			&p->file.minimum_bank_date,
			application_name,
			p->file.input_filename,
			p->file.date_piece_offset,
			p->file.description_piece_offset,
			p->file.debit_piece_offset,
			p->file.credit_piece_offset,
			p->file.balance_piece_offset,
			p->starting_sequence_number,
			p->fund_name );

	if ( !p->file.file_sha256sum )
	{
		char *msg;

		msg = "<h2>ERROR: cannot read file.</h2>";

		fprintf( stderr,
			 "%s/%s()/%d: %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 msg );

		printf( "%s\n", msg );

		bank_upload_exception = internal_read_permission;

		return (BANK_UPLOAD_STRUCTURE *)0;
	}

	p->file.file_row_count = list_length( p->file.bank_upload_file_list );

	if ( !p->file.file_row_count )
	{
		char *msg;

		msg = "<h2>ERROR: empty transaction rows.</h2>";

		fprintf( stderr,
			 "%s/%s()/%d: %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 msg );

		printf( "%s\n", msg );

		bank_upload_exception = empty_transaction_rows;

		return (BANK_UPLOAD_STRUCTURE *)0;
	}

	if ( bank_upload_sha256sum_exists(
			application_name,
			p->file.file_sha256sum ) )
	{
		char *msg;

		msg = "<h2>ERROR: duplicated file.</h2>";

		fprintf( stderr,
			 "%s/%s()/%d: %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 msg );

		printf( "%s\n", msg );

		bank_upload_exception = duplicated_spreadsheet_file;

		return (BANK_UPLOAD_STRUCTURE *)0;
	}

	p->existing_cash_journal_ledger_list =
		bank_upload_fetch_existing_cash_journal_ledger_list(
			application_name,
			p->file.minimum_bank_date,
			p->fund_name );

	p->reoccurring_structure = reoccurring_structure_new();

	p->reoccurring_structure->
		reoccurring_transaction_list =
			reoccurring_fetch_reoccurring_transaction_list(
				application_name );

	p->bank_upload_date_time =
		date_get_now19(
			date_get_utc_offset() );

	return p;

} /* bank_upload_structure_new() */

BANK_UPLOAD *bank_upload_new(	char *bank_date,
				char *bank_description )
{
	BANK_UPLOAD *p = bank_upload_calloc();

	p->bank_date = bank_date;
	p->bank_description = bank_description;

	return p;

} /* bank_upload_new() */

LIST *bank_upload_fetch_file_list(
				char **file_sha256sum,
				char **minimum_bank_date,
				char *application_name,
				char *input_filename,
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
	BANK_UPLOAD *bank_upload;
	LIST *bank_upload_list;
	boolean exists_fund;

	if ( file_sha256sum )
	{
		*file_sha256sum = timlib_get_sha256sum( input_filename );
	}

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

	bank_upload_list = list_new();

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

		if ( !bank_upload_get_bank_date_international(
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

		bank_upload = bank_upload_calloc();

		bank_upload->bank_date = strdup( bank_date_international );
		bank_upload->bank_description = strdup( bank_description );
		bank_upload->sequence_number = starting_sequence_number;
		bank_upload->bank_amount = atof( bank_amount );;
		bank_upload->bank_running_balance = atof( bank_balance );;

		list_append_pointer( bank_upload_list, bank_upload );
		starting_sequence_number++;
	}

	fclose( input_file );
	return bank_upload_list;

} /* bank_upload_fetch_file_list() */

void bank_upload_event_insert(		char *application_name,
					char *bank_upload_date_time,
					char *login_name,
					char *bank_upload_filename,
					char *file_sha256sum,
					char *fund_name )
{
	char sys_string[ 1024 ];
	FILE *insert_pipe;
	char *insert_upload_event;
	boolean exists_fund;
	char *table_name;

	exists_fund = ledger_fund_attribute_exists( application_name );

/*
#define INSERT_BANK_UPLOAD_EVENT		\
	"bank_upload_date_time,login_name,bank_upload_filename,file_sha256sum"

#define INSERT_BANK_UPLOAD_EVENT_FUND		\
	"bank_upload_date_time,login_name,bank_upload_filename,file_sha256sum,fund"
*/

	if ( exists_fund )
	{
		insert_upload_event = INSERT_BANK_UPLOAD_EVENT_FUND;
	}
	else
	{
		insert_upload_event = INSERT_BANK_UPLOAD_EVENT;
	}

	table_name =
		get_table_name(	application_name,
				"bank_upload_event" );

	sprintf( sys_string,
	 	 "insert_statement table=%s field=%s del='%c' 		  |"
	 	 "sql.e 2>&1						  |"
	 	 "html_paragraph_wrapper.e				   ",
	 	 table_name,
	 	 insert_upload_event,
	 	 FOLDER_DATA_DELIMITER );

	insert_pipe = popen( sys_string, "w" );

	if ( exists_fund )
	{
		fprintf(insert_pipe,
	 		"%s^%s^%s^%s^%s\n",
	 		bank_upload_date_time,
	 		login_name,
			bank_upload_filename,
			file_sha256sum,
			fund_name );
	}
	else
	{
		fprintf(insert_pipe,
	 		"%s^%s^%s^%s\n",
	 		bank_upload_date_time,
	 		login_name,
			bank_upload_filename,
			file_sha256sum );
	}

	pclose( insert_pipe );

} /* bank_upload_event_insert() */

void bank_upload_archive_insert(	char *application_name,
					LIST *bank_upload_list,
					char *bank_upload_date_time )
{
	char sys_string[ 1024 ];
	FILE *bank_upload_archive_insert_pipe = {0};
	BANK_UPLOAD *bank_upload;
	char *table_name;

	if ( !list_rewind( bank_upload_list ) );

/*
#define INSERT_BANK_UPLOAD_ARCHIVE	\
	"bank_date,bank_description,sequence_number,bank_amount,bank_running_balance,bank_upload_date_time"
*/

	table_name =
		get_table_name(	application_name,
				"bank_upload_archive" );

	sprintf( sys_string,
		 "insert_statement table=%s field=%s del='%c' 		  |"
		 "sql.e							  |"
		 "cat							   ",
		 	table_name,
		 	INSERT_BANK_UPLOAD_ARCHIVE,
		 	FOLDER_DATA_DELIMITER );

	bank_upload_archive_insert_pipe = popen( sys_string, "w" );

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		fprintf(bank_upload_archive_insert_pipe,
			"%s^%s^%d^%.2lf^%.2lf^%s\n",
		 	bank_upload->bank_date,
		 	bank_upload->bank_description,
			bank_upload->sequence_number,
		 	bank_upload->bank_amount,
			bank_upload->bank_running_balance,
			bank_upload_date_time );

	} while( list_next( bank_upload_list ) );

	pclose( bank_upload_archive_insert_pipe );

} /* bank_upload_archive_insert() */

/* Returns table_insert_count */
/* -------------------------- */
int bank_upload_insert(			char *application_name,
					LIST *bank_upload_list,
					char *bank_upload_date_time )
{
	char sys_string[ 1024 ];
	FILE *bank_upload_insert_pipe = {0};
	int table_insert_count = 0;
	char error_filename[ 128 ] = {0};
	BANK_UPLOAD *bank_upload;
	int error_file_lines;
	char *table_name;

	if ( !list_rewind( bank_upload_list ) ) return 0;

/*
#define INSERT_BANK_UPLOAD		\
	"bank_date,bank_description,sequence_number,bank_amount,bank_upload_date_time"
*/

	table_name =
		get_table_name(	application_name,
				"bank_upload" );

	sprintf(	error_filename,
			"/tmp/%s_%d.err",
			table_name,
			getpid() );

	sprintf( sys_string,
	 	 "insert_statement table=%s field=%s del='%c' 		  |"
	 	 "sql.e 2>&1						  |"
	 	 "cat > %s						   ",
	 	table_name,
	 	INSERT_BANK_UPLOAD,
	 	FOLDER_DATA_DELIMITER,
		error_filename );

	bank_upload_insert_pipe = popen( sys_string, "w" );

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		/* Output insert into BANK_UPLOAD */
		/* ------------------------------ */
		fprintf(bank_upload_insert_pipe,
			"%s^%s^%d^%.2lf^%s\n",
		 	bank_upload->bank_date,
		 	bank_upload->bank_description,
			bank_upload->sequence_number,
		 	bank_upload->bank_amount,
			bank_upload_date_time );

		table_insert_count++;

	} while( list_next( bank_upload_list ) );

	pclose( bank_upload_insert_pipe );

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

} /* bank_upload_insert() */

int bank_upload_get_starting_sequence_number(
			char *application_name,
			char *input_filename,
			int date_piece_offset )
{
	int line_count;
	char sys_string[ 1024 ];

	line_count =
		bank_upload_get_line_count(
			input_filename,
			date_piece_offset );

	sprintf( sys_string,
		 "reference_number.sh %s %d",
		 application_name,
		 line_count );

	return atoi( pipe2string( sys_string ) );

} /* bank_upload_get_starting_sequence_number() */

int bank_upload_get_line_count(	char *input_filename,
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

} /* bank_upload_get_line_count() */

boolean bank_upload_get_bank_date_international(
				char *bank_date_international,
				char *bank_date )
{
	date_convert_source_american(
		bank_date_international,
		international,
		bank_date );

	return date_convert_is_valid_international(
		bank_date_international );

} /* bank_upload_get_bank_date_international() */

char *bank_upload_get_select( void )
{
	char *select;

	select =	"bank_date,		"
			"bank_description,	"
			"sequence_number,	"
			"bank_amount,		"
			"bank_running_balance	";

	return select;
}

BANK_UPLOAD *bank_upload_fetch(		char *application_name,
					char *bank_date,
					char *bank_description )
{
	BANK_UPLOAD *bank_upload;
	char *select;
	char where[ 512 ];
	char description_buffer[ 512 ];
	char sys_string[ 1024 ];
	char *results;

	select = bank_upload_get_select();

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
		 "			folder=bank_upload	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	if ( !( results = pipe2string( sys_string ) ) )
		return (BANK_UPLOAD *)0;

	bank_upload = bank_upload_calloc();

	bank_upload_fetch_parse(
			&bank_upload->bank_date,
			&bank_upload->bank_description,
			&bank_upload->sequence_number,
			&bank_upload->bank_amount,
			&bank_upload->bank_running_balance,
			results );

	return bank_upload;

} /* bank_upload_fetch() */

LIST *bank_upload_fetch_list(		char *application_name,
					int starting_sequence_number )
{
	LIST *bank_upload_list;
	BANK_UPLOAD *bank_upload;
	char *select;
	char where[ 128 ];
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;

	bank_upload_list = list_new();
	select = bank_upload_get_select();

	sprintf( where, "sequence_number >= %d", starting_sequence_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=bank_upload	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		bank_upload = bank_upload_calloc();

		bank_upload_fetch_parse(
				&bank_upload->bank_date,
				&bank_upload->bank_description,
				&bank_upload->sequence_number,
				&bank_upload->bank_amount,
				&bank_upload->bank_running_balance,
				input_buffer );

		list_append_pointer( bank_upload_list, bank_upload );
	}

	pclose( input_pipe );

	return bank_upload_list;

} /* bank_upload_fetch_list() */

void bank_upload_fetch_parse(	char **bank_date,
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

} /* bank_upload_fetch_parse() */

LIST *bank_upload_fetch_existing_cash_journal_ledger_list(
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

} /* bank_upload_fetch_existing_cash_journal_ledger_list() */

void bank_upload_set_transaction(
				LIST *bank_upload_list,
				LIST *reoccurring_transaction_list,
				LIST *existing_cash_journal_ledger_list )
{
	BANK_UPLOAD *bank_upload;
	REOCCURRING_TRANSACTION *reoccurring_transaction;
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get( bank_upload_list );

		if ( ! ( reoccurring_transaction =
				reoccurring_seek_bank_upload_search_phrase(
					reoccurring_transaction_list,
					bank_upload->bank_description ) ) )
		{
			continue;
		}

		if ( ledger_exists_journal_ledger(
				existing_cash_journal_ledger_list,
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				bank_upload->bank_date
					/* transaction_date */,
				bank_upload->bank_amount
					/* transaction_amount */ ) )
		{
			continue;
		}

		transaction =
			ledger_transaction_new(
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				ledger_get_transaction_date_time(
					bank_upload->bank_date
						/* transaction_date */ ),
				(char *)0 /* memo */ );

		transaction->transaction_amount =
			float_abs( bank_upload->bank_amount );

		bank_upload->transaction = transaction;
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

	} while( list_next( bank_upload_list ) );

} /* bank_upload_set_transaction() */

void bank_upload_insert_transaction(
					char *application_name,
					LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	TRANSACTION *transaction;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get( bank_upload_list );

		if ( !bank_upload->transaction ) continue;

		transaction = bank_upload->transaction;

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

	} while( list_next( bank_upload_list ) );

} /* bank_upload_insert_transaction() */

void bank_upload_table_display(
				LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *heading;

	if ( !list_rewind( bank_upload_list ) ) return;

	heading = "bank_date,description,amount";

	sprintf( sys_string,
		 "html_table.e '' %s '^' left,left,right",
		 heading );

	output_pipe = popen( sys_string, "w" );

	do {
		bank_upload = list_get( bank_upload_list );

		fprintf( output_pipe,
			 "%s^%s^%.2lf\n",
			 bank_upload->bank_date,
			 bank_upload->bank_description,
			 bank_upload->bank_amount );

	} while( list_next( bank_upload_list ) );

	pclose( output_pipe );

} /* bank_upload_table_display() */

void bank_upload_transaction_display( LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	char *transaction_memo;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get( bank_upload_list );

		if ( bank_upload->transaction )
		{
			transaction_memo =
				bank_upload_get_transaction_memo(
					bank_upload->
						transaction->
						full_name,
					bank_upload->
						transaction->
						street_address,
					bank_upload->
						transaction->
						transaction_date_time );

			ledger_list_html_display(
				transaction_memo,
				bank_upload->
					transaction->
					journal_ledger_list );
		}

	} while( list_next( bank_upload_list ) );

} /* bank_upload_transaction_display() */

BANK_UPLOAD *bank_upload_dictionary_extract(
				char *application_name,
				DICTIONARY *dictionary )
{
	char *key;
	BANK_UPLOAD *bank_upload;
	char *bank_date;
	char *bank_description;

	key = "bank_date";

	if ( ! ( bank_date = dictionary_fetch( dictionary, key ) ) )
	{
		return (BANK_UPLOAD *)0;
	}

	key = "bank_description";

	if ( ! ( bank_description =
			dictionary_fetch( dictionary, key ) ) )
	{
		return (BANK_UPLOAD *)0;
	}

	bank_upload =
		bank_upload_fetch(
			application_name,
			bank_date,
			bank_description );

	return bank_upload;

} /* bank_upload_dictionary_extract() */

char *bank_upload_get_transaction_memo(
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	static char transaction_memo[ 256 ];
	char street_address_display[ 128 ];

	if ( street_address
	&&   *street_address
	&&   strcmp( street_address, "null" ) != 0 )
	{
		sprintf( street_address_display,
			 "/%s",
			 street_address );
	}
	else
	{
		*street_address_display = '\0';
	}

	sprintf(transaction_memo,
		"%s%s/%s",
		full_name,
		street_address_display,
		transaction_date_time );

	format_initial_capital(
		transaction_memo,
		transaction_memo );

	return transaction_memo;

} /* bank_upload_get_transaction_memo() */

boolean bank_upload_sha256sum_exists(
			char *application_name,
			char *file_sha256sum )
{
	char where[ 512 ];
	char sys_string[ 1024 ];

	sprintf( where,
		 "file_sha256sum = '%s'",
		 file_sha256sum );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=count			"
		 "			folder=bank_upload_event	"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return (boolean)atoi( pipe2string( sys_string ) );

} /* bank_upload_sha256sum_exists() */

int bank_upload_transaction_count(
			LIST *bank_upload_list )
{
	BANK_UPLOAD *b;
	int count = 0;

	if ( list_rewind( bank_upload_list ) )
	{
		do {
			b = list_get_pointer( bank_upload_list );

			if ( b->transaction ) count++;

		} while( list_next( bank_upload_list ) );
	}

	return count;

} /* bank_upload_transaction_count() */

