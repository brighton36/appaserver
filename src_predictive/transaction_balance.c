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
	TRANSACTION_BALANCE_ROW *row;
	TRANSACTION_BALANCE_BLOCK *block;

	if ( !list_rewind( transaction_balance_row_list ) )
		return (LIST *)0;

	block_list = list_new();

	block = (TRANSACTION_BALANCE_BLOCK *)0;

	do {
		row = list_get( transaction_balance_row_list );

		if ( !block )
		{
			if ( !timlib_dollar_virtually_same(
				row->balance,
				row->bank_running_balance ) )
			{
				continue;
			}

			block = transaction_balance_block_new();
			block->begin_transaction_balance = row;
			block->block_count = 1;
			block->is_inbalance = 1;
			continue;
		}
		else
		{
			if ( timlib_dollar_virtually_same(
				row->balance,
				row->bank_running_balance ) )
			{
				block->block_count++;
				continue;
			}

			block->end_transaction_balance = row;
			list_append_pointer( block_list, block );
			block = (TRANSACTION_BALANCE_BLOCK *)0;
		}

	} while( list_next( transaction_balance_row_list ) );

	if ( block && timlib_dollar_virtually_same(
			row->balance,
			row->bank_running_balance ) )
	{
		block->end_transaction_balance = row;
		list_append_pointer( block_list, block );
	}

	return block_list;

} /* transaction_balance_get_inbalance_block_list() */

LIST *transaction_balance_get_outbalance_block_list(
					LIST *transaction_balance_row_list )
{
	LIST *block_list;
	TRANSACTION_BALANCE_ROW *row;
	TRANSACTION_BALANCE_BLOCK *block;

	if ( !list_rewind( transaction_balance_row_list ) )
		return (LIST *)0;

	block_list = list_new();

	block = (TRANSACTION_BALANCE_BLOCK *)0;

	do {
		row = list_get( transaction_balance_row_list );

		if ( !block )
		{
			if ( timlib_dollar_virtually_same(
				row->balance,
				row->bank_running_balance ) )
			{
				continue;
			}

			block = transaction_balance_block_new();
			block->begin_transaction_balance = row;
			block->block_count = 1;
			continue;
		}
		else
		{
			if ( !timlib_dollar_virtually_same(
				row->balance,
				row->bank_running_balance ) )
			{
				block->block_count++;
				continue;
			}

			block->end_transaction_balance = row;
			list_append_pointer( block_list, block );
			block = (TRANSACTION_BALANCE_BLOCK *)0;
		}

	} while( list_next( transaction_balance_row_list ) );

	if ( block && !timlib_dollar_virtually_same(
			row->balance,
			row->bank_running_balance ) )
	{
		block->end_transaction_balance = row;
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

