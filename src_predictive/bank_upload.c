/* -------------------------------------------------------------------- */
/* src_predictive/bank_upload.c						*/
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
#include "bank_upload.h"

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
			char *fund_name,
			char *input_filename )
{
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
	p->input_filename = input_filename;

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

REOCCURRING_TRANSACTION *bank_upload_reoccurring_transaction_calloc( void )
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

REOCCURRING_TRANSACTION *bank_upload_reoccurring_transaction_new(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_description,
					double transaction_amount )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;

	reoccurring_transaction = bank_upload_reoccurring_transaction_calloc();

	reoccurring_transaction->full_name = full_name;
	reoccurring_transaction->street_address = street_address;

	reoccurring_transaction->transaction_description =
		transaction_description;

	if ( !bank_upload_reoccurring_transaction_load(
			&reoccurring_transaction->debit_account,
			&reoccurring_transaction->credit_account,
			&reoccurring_transaction->transaction_amount,
			&reoccurring_transaction->accrued_daily_amount,
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

} /* bank_upload_reoccurring_transaction_new() */

boolean bank_upload_reoccurring_transaction_load(
				char **debit_account,
				char **credit_account,
				double *transaction_amount,
				double *accrued_daily_amount,
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

	select = bank_upload_reoccurring_transaction_get_select();

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

	reoccurring_transaction = bank_upload_reoccurring_transaction_calloc();

	bank_upload_reoccurring_transaction_parse(
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
			results );

	*debit_account = reoccurring_transaction->debit_account;
	*credit_account = reoccurring_transaction->credit_account;
	*transaction_amount = reoccurring_transaction->transaction_amount;
	*accrued_daily_amount = reoccurring_transaction->accrued_daily_amount;

	return 1;

} /* bank_upload_reoccurring_transaction_load() */

/* Returns table_insert_count */
/* -------------------------- */
int bank_upload_table_insert(	FILE *input_file,
				char **minimum_bank_date,
				char *application_name,
				char *fund_name,
				boolean execute,
				int starting_sequence_number )
{
	char sys_string[ 1024 ];
	char input_string[ 4096 ];
	char *table_name;
	char bank_date[ 128 ];
	char bank_date_international[ 128 ];
	char bank_description[ 1024 ];
	char bank_amount[ 128 ];
	char bank_running_balance[ 128 ];
	FILE *table_output_pipe = {0};
	FILE *bank_upload_insert_pipe = {0};
	int table_insert_count = 0;
	boolean found_header = 0;
	char error_filename[ 128 ] = {0};
	char *insert_bank_upload;
	static char local_minimum_bank_date[ 16 ] = {0};
	boolean exists_fund;

	exists_fund = ledger_fund_attribute_exists( application_name );

	if ( minimum_bank_date )
	{
		*minimum_bank_date = local_minimum_bank_date;
	}

	if ( exists_fund )
		insert_bank_upload = INSERT_BANK_UPLOAD_FUND;
	else
		insert_bank_upload = INSERT_BANK_UPLOAD;

	if ( execute )
	{
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
		 	insert_bank_upload,
		 	FOLDER_DATA_DELIMITER,
			error_filename );

		bank_upload_insert_pipe = popen( sys_string, "w" );
	}
	else
	{
		char *justify;

		justify = "left,left,right";

		sprintf( sys_string,
		"queue_top_bottom_lines.e 50		|"
		"html_table.e '' %s '%c' %s		 ",
			 insert_bank_upload,
			 FOLDER_DATA_DELIMITER,
			 justify );

		table_output_pipe = popen( sys_string, "w" );
	}

	while( timlib_get_line( input_string, input_file, 4096 ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_quote_comma(
				bank_date,
				input_string,
				0 ) )
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

		if ( !piece_quote_comma(
				bank_description,
				input_string,
				1 ) )
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

		if ( !piece_quote_comma(
				bank_amount,
				input_string,
				2 ) )
		{
			continue;
		}

		/* Skip beginning balance rows. */
		/* ---------------------------- */
		if ( !atof( bank_amount ) ) continue;

		/* Bank running balance is optional */
		/* -------------------------------- */
		if ( !piece_quote_comma(
				bank_running_balance,
				input_string,
				3 ) )
		{
			strcpy( bank_running_balance, "null" );
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

		if ( table_output_pipe )
		{
			if ( exists_fund )
			{
				fprintf(table_output_pipe,
			 		"%s^%s^%d^%s^%s^%s\n",
			 		bank_date_international,
			 		bank_description,
					starting_sequence_number++,
			 		bank_amount,
					bank_running_balance,
					fund_name );
			}
			else
			{
				fprintf(table_output_pipe,
			 		"%s^%s^%d^%s^%s\n",
			 		bank_date_international,
			 		bank_description,
					starting_sequence_number++,
			 		bank_amount,
					bank_running_balance );
			}
		}
		else
		{
			if ( exists_fund )
			{
				fprintf(bank_upload_insert_pipe,
			 		"%s^%s^%d^%s^%s^%s\n",
			 		bank_date_international,
			 		bank_description,
					starting_sequence_number++,
			 		bank_amount,
					bank_running_balance,
					fund_name );
			}
			else
			{
				fprintf(bank_upload_insert_pipe,
			 		"%s^%s^%d^%s^%s\n",
			 		bank_date_international,
			 		bank_description,
					starting_sequence_number++,
			 		bank_amount,
					bank_running_balance );
			}
		}

		table_insert_count++;
	}

	if ( execute )
	{
		pclose( bank_upload_insert_pipe );
		int error_file_lines;

		sprintf( sys_string,
			 "wc -l %s",
			 error_filename );

		error_file_lines = atoi( pipe2string( sys_string ) );
		table_insert_count -= error_file_lines;

		sprintf( sys_string,
			 "rm -f %s",
			 error_filename );

		system( sys_string );
	}
	else
	{
		pclose( table_output_pipe );
	}

	return table_insert_count;

} /* bank_upload_table_insert() */

int bank_upload_get_starting_sequence_number(
			char *application_name,
			char *input_filename )
{
	int line_count;
	char sys_string[ 1024 ];

	line_count =
		bank_upload_get_line_count(
			input_filename );

	sprintf( sys_string,
		 "reference_number.sh %s %d",
		 application_name,
		 line_count );

	return atoi( pipe2string( sys_string ) );

} /* bank_upload_get_starting_sequence_number() */

int bank_upload_get_line_count( char *input_filename )
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
				0 ) )
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

		bank_upload_parse(
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

void bank_upload_parse(		char **bank_date,
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

} /* bank_upload_parse() */

void bank_upload_reoccurring_transaction_parse(
					char **full_name,
					char **street_address,
					char **debit_account,
					char **credit_account,
					double *transaction_amount,
					char **bank_upload_search_phrase,
					double *accrued_daily_amount,
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

} /* bank_upload_reoccurring_transaction_parse() */

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
					/* transaction_date_time */,
				cash_account_name );

	return existing_cash_journal_ledger_list;

} /* bank_upload_fetch_existing_cash_journal_ledger_list() */

char *bank_upload_reoccurring_transaction_get_select( void )
{
	char *select;

	select = "full_name,			"
		 "street_address,		"
		 "debit_account,		"
		 "credit_account,		"
		 "transaction_amount,		"
		 "bank_upload_search_phrase,	"
		 "accrued_daily_amount		";

	return select;
}

LIST *bank_upload_fetch_reoccurring_transaction_list(
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

	select = bank_upload_reoccurring_transaction_get_select();

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
			bank_upload_reoccurring_transaction_calloc();

		bank_upload_reoccurring_transaction_parse(
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
				input_buffer );

		list_append_pointer(
			reoccurring_transaction_list,
			reoccurring_transaction );
	}

	pclose( input_pipe );

	return reoccurring_transaction_list;

} /* bank_upload_fetch_reoccurring_transaction_list() */

REOCCURRING_TRANSACTION *bank_upload_seek_bank_upload_search_phrase(
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

} /* bank_upload_seek_bank_upload_search_phrase() */

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
				bank_upload_seek_bank_upload_search_phrase(
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

void bank_upload_html_display( LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;

	if ( !list_length( bank_upload_list ) ) return;

#ifdef NOT_DEFINED
	char buffer[ 128 ];
	HTML_TABLE *html_table;
	LIST *heading_list;
	html_table =
		html_table_new(
			(char *)0 /* title */,
			(char *)0 /* sub_title */,
			(char *)0 /* sub_sub_title */ );

	heading_list = new_list();
	list_append_string( heading_list, "Date" );
	list_append_string( heading_list, "Description" );
	list_append_string( heading_list, "BankAmount" );
	list_append_string( heading_list, "RunningBalance" );
	
	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	if ( !list_rewind( bank_upload_list ) )
	{
		html_table_close();
		return;
	}

	do {
		bank_upload = list_get( bank_upload_list );

		html_table_set_data(
			html_table->data_list,
			bank_upload->bank_date );

		html_table_set_data(
			html_table->data_list,
			bank_upload->bank_description );

		sprintf( buffer, "%.2lf", bank_upload->bank_amount );

		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		sprintf( buffer, "%.2lf", bank_upload->bank_running_balance );

		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		html_table->data_list = list_new();

	} while( list_next( bank_upload_list ) );

	html_table_close();
#endif

	/* Display the generated transactions */
	/* ---------------------------------- */
	list_rewind( bank_upload_list );

	do {
		bank_upload = list_get( bank_upload_list );

		if ( bank_upload->transaction )
		{
			ledger_list_html_display(
				bank_upload->
					transaction->
					journal_ledger_list );
		}

	} while( list_next( bank_upload_list ) );

} /* bank_upload_html_display() */

