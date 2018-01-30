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
#include "bank_upload.h"

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

BANK_UPLOAD *bank_upload_new(
				char *bank_date,
				char *bank_description )
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

	p->bank_date = bank_date;
	p->bank_description = bank_description;

	return p;

} /* bank_upload_new() */

REOCCURRING_TRANSACTION *bank_upload_reoccurring_transaction_new(
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

	if ( !bank_upload_reoccurring_transaction_load(
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

} /* bank_upload_reoccurring_transaction_new() */

boolean bank_upload_reoccurring_transaction_load(
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

} /* bank_upload_reoccurring_transaction_load() */

/* Returns table_insert_count */
/* -------------------------- */
int bank_upload_table_insert(	FILE *input_file,
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
	char *insert_bank_download;

	if ( fund_name && *fund_name && strcmp( fund_name, "fund" ) != 0 )
		insert_bank_download = INSERT_BANK_UPLOAD_FUND;
	else
		insert_bank_download = INSERT_BANK_UPLOAD;

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
		 	insert_bank_download,
		 	FOLDER_DATA_DELIMITER,
			error_filename );

		bank_upload_insert_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
		"queue_top_bottom_lines.e 50		|"
		"html_table.e '' %s '%c'		 ",
			 INSERT_BANK_UPLOAD,
			 FOLDER_DATA_DELIMITER);

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

		if (	fund_name
		&&	*fund_name
		&&	strcmp( fund_name, "fund" ) != 0 )
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

		if ( table_output_pipe )
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
			fprintf(bank_upload_insert_pipe,
			 	"%s^%s^%d^%s^%s^%s\n",
			 	bank_date_international,
			 	bank_description,
				starting_sequence_number++,
			 	bank_amount,
				bank_running_balance,
				fund_name );
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
		printf( "<h2>ERROR: cannot open %s for read</h2>\n",
			input_filename );
		document_close();
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

