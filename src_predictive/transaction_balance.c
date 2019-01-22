/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/transaction_balance.c		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "appaserver_library.h"
#include "piece.h"
#include "transaction_balance.h"

TRANSACTION_BALANCE_ROW *transaction_balance_row_new( void )
{
	TRANSACTION_BALANCE_ROW *p;

	if ( ! ( p = (TRANSACTION_BALANCE_ROW *)
			calloc( 1,
			sizeof( TRANSACTION_BALANCE_ROW ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;

} /* transaction_balance_calloc() */

TRANSACTION_BALANCE_BLOCK *transaction_balance_block_new( void )
{
	TRANSACTION_BALANCE_BLOCK *p;

	if ( ! ( p = (TRANSACTION_BALANCE_BLOCK *)
			calloc(	1,
				sizeof( TRANSACTION_BALANCE_BLOCK ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;

} /* transaction_balance_block_new() */

TRANSACTION_BALANCE *transaction_balance_calloc( void )
{
	TRANSACTION_BALANCE *p;

	if ( ! ( p = (TRANSACTION_BALANCE *)
				calloc( 1, sizeof( TRANSACTION_BALANCE ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;

} /* transaction_balance_calloc() */

TRANSACTION_BALANCE *transaction_balance_new(
					char *application_name,
					char *begin_date,
					double cash_ending_balance )
{
	TRANSACTION_BALANCE *p;

	p = transaction_balance_calloc();

	p->input.begin_date = begin_date;
	p->input.cash_ending_balance = cash_ending_balance;

	p->input.transaction_balance_row_list =
		transaction_balance_fetch_row_list(
			application_name,
			p->input.begin_date );

	return p;

} /* transaction_balance_new() */

TRANSACTION_BALANCE_ROW *transaction_balance_prior_fetch(
					char *application_name,
					char *transaction_date_time )
{
	char *prior_transaction_date_time;
	char *select;
	char *folder;
	char where[ 512 ];
	char sys_string[ 1024 ];

	select = "max( transaction_date_time )";
	folder = "bank_upload_transaction_balance";

	sprintf( where,
		 "transaction_date_time < '%s'",
		 transaction_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	prior_transaction_date_time = pipe2string( sys_string );

	if ( !prior_transaction_date_time || !*prior_transaction_date_time )
	{
		return (TRANSACTION_BALANCE_ROW *)0;
	}
	else
	{
		return transaction_balance_transaction_date_time_fetch(
				application_name,
				prior_transaction_date_time );
	}


} /* transaction_balance_prior_fetch() */

TRANSACTION_BALANCE_ROW *transaction_balance_transaction_date_time_fetch(
					char *application_name,
					char *transaction_date_time )
{
	char *select;
	char *folder;
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *input_row;

	select = TRANSACTION_BALANCE_SELECT;
	folder = "bank_upload_transaction_balance";

	sprintf( where,
		 "transaction_date_time = '%s'",
		 transaction_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	input_row = pipe2string( sys_string );

	if ( !input_row || !*input_row )
	{
		return (TRANSACTION_BALANCE_ROW *)0;
	}
	else
	{
		return transaction_balance_parse_row( input_row );
	}

} /* transaction_balance_transaction_date_time_fetch() */

LIST *transaction_balance_fetch_row_list(
					char *application_name,
					char *begin_date )
{
	TRANSACTION_BALANCE_ROW *row;
	char *select;
	char *folder;
	char where[ 512 ];
	char input_buffer[ 1024 ];
	char sys_string[ 1024 ];
	FILE *input_pipe;
	LIST *transaction_balance_row_list;

	select = TRANSACTION_BALANCE_SELECT;
	folder = "bank_upload_transaction_balance";

	sprintf( where,
		 "transaction_date_time >= '%s'",
		 begin_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=transaction_date_time	",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );

	transaction_balance_row_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		row = transaction_balance_parse_row( input_buffer );

		list_append_pointer( transaction_balance_row_list, row );
	}

	pclose( input_pipe );

	return transaction_balance_row_list;

} /* transaction_balance_fetch_row_list() */

TRANSACTION_BALANCE_ROW *transaction_balance_parse_row(
				char *input_buffer )
{
	TRANSACTION_BALANCE_ROW *r;
	char piece_buffer[ 512 ];

	r = transaction_balance_row_new();

	if ( character_count( FOLDER_DATA_DELIMITER, input_buffer ) != 9 )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: not 7 delimiters of (%c) in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 FOLDER_DATA_DELIMITER,
			 input_buffer );
		return (TRANSACTION_BALANCE_ROW *)0;
	}

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	r->transaction_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	r->bank_date = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	r->bank_description = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	r->full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	r->street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	r->transaction_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	r->bank_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	r->balance = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	r->bank_running_balance = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
	r->sequence_number = atoi( piece_buffer );

	return r;

} /* transaction_balance_parse_row() */

LIST *transaction_balance_get_inbalance_block_list(
					LIST *transaction_balance_row_list )
{
	LIST *block_list;
	TRANSACTION_BALANCE_ROW *current_row;
	TRANSACTION_BALANCE_ROW *prior_row = {0};
	TRANSACTION_BALANCE_BLOCK *block;

	if ( !list_rewind( transaction_balance_row_list ) )
		return (LIST *)0;

	block_list = list_new();

	block = (TRANSACTION_BALANCE_BLOCK *)0;

	do {
		current_row = list_get( transaction_balance_row_list );

		/* If not inside a block */
		/* --------------------- */
		if ( !block )
		{
			if ( !timlib_dollar_virtually_same(
				current_row->balance,
				current_row->bank_running_balance ) )
			{
				prior_row = current_row;
				continue;
			}

			/* Now inside a block */
			/* ------------------ */
			block = transaction_balance_block_new();
			block->begin_transaction_balance = current_row;
			block->block_count = 1;
			block->is_inbalance = 1;
			prior_row = current_row;
			continue;
		}
		else
		/* ------------------- */
		/* Else inside a block */
		/* ------------------- */
		{
			if ( timlib_dollar_virtually_same(
				current_row->balance,
				current_row->bank_running_balance ) )
			{
				/* Still inside a block */
				/* -------------------- */
				block->block_count++;
				prior_row = current_row;
				continue;
			}

			/* -------------------------------------------- */
			/* The current row will be the first row in the */
			/* next block.					*/
			/* -------------------------------------------- */
			if ( prior_row )
				block->end_transaction_balance = prior_row;
			else
				block->end_transaction_balance = current_row;

			list_append_pointer( block_list, block );

			prior_row = current_row;

			/* No longer inside a block */
			/* ------------------------ */
			block = (TRANSACTION_BALANCE_BLOCK *)0;
		}

	} while( list_next( transaction_balance_row_list ) );

	if ( block && timlib_dollar_virtually_same(
			current_row->balance,
			current_row->bank_running_balance ) )
	{
		block->end_transaction_balance = current_row;
		block->block_count++;
		list_append_pointer( block_list, block );
	}

	return block_list;

} /* transaction_balance_get_inbalance_block_list() */

LIST *transaction_balance_get_outbalance_block_list(
					LIST *transaction_balance_row_list )
{
	LIST *block_list;
	TRANSACTION_BALANCE_ROW *current_row;
	TRANSACTION_BALANCE_BLOCK *block;

	if ( !list_rewind( transaction_balance_row_list ) )
		return (LIST *)0;

	block_list = list_new();

	block = (TRANSACTION_BALANCE_BLOCK *)0;

	do {
		current_row = list_get( transaction_balance_row_list );

		/* If not inside a block */
		/* --------------------- */
		if ( !block )
		{
			if ( timlib_dollar_virtually_same(
				current_row->balance,
				current_row->bank_running_balance ) )
			{
				continue;
			}

			/* Now inside a block */
			/* ------------------ */
			block = transaction_balance_block_new();
			block->begin_transaction_balance = current_row;
			block->block_count = 1;
			continue;
		}
		else
		/* ------------------- */
		/* Else inside a block */
		/* ------------------- */
		{
			if ( !timlib_dollar_virtually_same(
				current_row->balance,
				current_row->bank_running_balance ) )
			{
				/* Still inside a block */
				/* -------------------- */
				block->block_count++;
				continue;
			}

			block->end_transaction_balance = current_row;
			block->block_count++;
			list_append_pointer( block_list, block );

			/* No longer inside a block */
			/* ------------------------ */
			block = (TRANSACTION_BALANCE_BLOCK *)0;
		}

	} while( list_next( transaction_balance_row_list ) );

	if ( block && !timlib_dollar_virtually_same(
			current_row->balance,
			current_row->bank_running_balance ) )
	{
		block->end_transaction_balance = current_row;
		block->block_count++;
		list_append_pointer( block_list, block );
	}

	return block_list;

} /* transaction_balance_get_outbalance_block_list() */

LIST *transaction_balance_get_merged_block_list(
					LIST *inbalance_block_list,
					LIST *outbalance_block_list )
{
	TRANSACTION_BALANCE_BLOCK *inblock;
	TRANSACTION_BALANCE_BLOCK *outblock;
	LIST *return_list;

	if ( !list_rewind( inbalance_block_list )
	||   !list_rewind( outbalance_block_list ) )
	{
		if ( list_length( inbalance_block_list ) )
			return inbalance_block_list;

		if ( list_length( outbalance_block_list ) )
			return outbalance_block_list;

		return (LIST *)0;
	}

	return_list = list_new();

	while ( 1 ) {

		inblock = list_get( inbalance_block_list );
		outblock = list_get( outbalance_block_list );

		if ( strcmp(	inblock->
					begin_transaction_balance->
					transaction_date_time,
				outblock->
					begin_transaction_balance->
					transaction_date_time ) < 0 )
		{
			list_append_pointer( return_list, inblock );

			if ( !list_next( inbalance_block_list ) )
			{
				list_append_current_list(
					return_list,
					outbalance_block_list );
				break;
			}
		}
		else
		{
			list_append_pointer( return_list, outblock );

			if ( !list_next( outbalance_block_list ) )
			{
				list_append_current_list(
					return_list,
					inbalance_block_list );
				break;
			}
		}

	} /* break in middle */

	return return_list;

} /* transaction_balance_get_merged_block_list() */

double transaction_balance_calculate_anomaly_balance_difference(
			double balance,
			double bank_running_balance )
{
	return balance - bank_running_balance;

}

void transaction_balance_row_stdout(
			TRANSACTION_BALANCE_ROW *row )
{
	char destination[ 1024 ];

	printf( "Transaction date time: %s\n", row->transaction_date_time );
	printf( "Bank date: %s\n", row->bank_date );

	printf( "Bank description: %s\n",
		left_string(	destination,
				row->bank_description /* source */,
				60 /* how_many */ ) );

	printf( "Full name: %s\n", row->full_name );
	printf( "Cash transaction amount: %.2lf\n", row->transaction_amount );
	printf( "Bank load amount: %.2lf\n", row->bank_amount );
	printf( "Cash running balance: %.2lf\n", row->balance );
	printf( "Bank running balance: %.2lf\n", row->bank_running_balance );
	printf( "Sequence number: %d\n", row->sequence_number );
	printf( "\n" );

} /* transaction_balance_row_stdout() */

