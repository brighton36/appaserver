/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/pay_liabilities.c		*/
/* -------------------------------------------------------------------- */
/* This is the appaserver pay_liabilities ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include "timlib.h"
#include "ledger.h"
#include "appaserver_library.h"
#include "piece.h"
#include "date.h"
#include "pay_liabilities.h"

double pay_liabilities_get_sum_balance(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address )
{
	char sys_string[ 128 ];
	char *record;
	char input_full_name[ 128 ];
	char input_street_address_balance[ 128 ];
	char input_street_address[ 128 ];
	char input_balance[ 16 ];
	static LIST *entity_record_list = {0};

	if ( !entity_record_list )
	{
		sprintf( sys_string,
		 	"populate_print_checks_entity %s '%s'",
		 	application_name,
			fund_name );

		entity_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( entity_record_list ) ) return 0.0;

	do {
		record = list_get_pointer( entity_record_list );

		if ( character_count(
			FOLDER_DATA_DELIMITER,
			record ) != 1 )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: not one delimiter in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 record );

			return 0.0;
		}

		piece(	input_full_name,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	input_street_address_balance,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		piece(	input_street_address,
			'[',
			input_street_address_balance,
			0 );

		piece( input_balance, '[', input_street_address_balance, 1 );

		if ( strcmp( input_full_name, full_name ) == 0
		&&   strcmp( input_street_address, street_address ) == 0 )
		{
			return atof( input_balance );
		}

	} while( list_next( entity_record_list ) );

	return 0.0;

} /* pay_liabilities_get_sum_balance() */

PAY_LIABILITIES *pay_liabilities_calloc( void )
{
	PAY_LIABILITIES *p;

	if ( ! ( p = (PAY_LIABILITIES *)
			calloc( 1, sizeof( PAY_LIABILITIES ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return p;

} /* pay_liabilities_calloc() */

PAY_LIABILITIES *pay_liabilities_new(
				char *application_name,
				char *fund_name,
				LIST *full_name_list,
				LIST *street_address_list,
				int starting_check_number,
				double dialog_box_payment_amount )
{
	PAY_LIABILITIES *p;
	char *checking_account = {0};
	char *uncleared_checks_account = {0};
	char *account_payable_name = {0};

	p = pay_liabilities_calloc();

	/* Input */
	/* ===== */
	p->input.starting_check_number = starting_check_number;
	p->input.dialog_box_payment_amount = dialog_box_payment_amount;

	p->input.loss_account_name =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			LEDGER_LOSS_KEY,
			0 /* not warning_only */ );

	ledger_get_vendor_payment_account_names(
				&checking_account,
				&uncleared_checks_account,
				&account_payable_name,
				application_name,
				fund_name );

	if ( starting_check_number )
		p->input.credit_account_name = uncleared_checks_account;
	else
		p->input.credit_account_name = checking_account;

	p->input.liability_account_entity_list =
		pay_liabilities_fetch_liability_account_entity_list(
				application_name );

	/* --------------------------------------- */
	/* Get sum_balance for the posted entities */
	/* from the displayed dialog box.	   */
	/* --------------------------------------- */
	p->input.entity_payable_list =
		pay_liabilities_input_get_entity_payable_list(
			application_name,
			fund_name,
			full_name_list,
			street_address_list );

	/* -------------------------------------------------------- */
	/* Fetch all the current liability accounts and all the     */
	/* journal ledger rows following the last zero balance row. */
	/* -------------------------------------------------------- */
	p->input.current_liability_account_list =
		pay_liabilities_fetch_current_liability_account_list(
			application_name );

	p->input.purchase_order_list =
		purchase_get_amount_due_purchase_order_list(
			application_name );

	/* Process */
	/* ======= */

	/* Rearrange the account list into an entity list. */
	/* ----------------------------------------------- */
	p->process.current_liability_entity_list =
		pay_liabilities_get_current_liability_entity_list(
			p->input.current_liability_account_list );

	/* Process the LIABILITY_ACCOUNT_ENTITY records. */
	/* --------------------------------------------- */
	p->process.liability_account_entity_list =
		pay_liabilities_get_liability_account_entity_list(
			p->input.liability_account_entity_list,
			p->input.entity_payable_list,
			p->input.dialog_box_payment_amount );

	/* Process the regular records. */
	/* ---------------------------- */
	p->process.entity_payable_list =
		pay_liabilities_process_get_entity_payable_list(
			p->input.entity_payable_list,
			p->process.current_liability_entity_list,
			p->input.purchase_order_list,
			p->input.dialog_box_payment_amount );

	/* Output */
	/* ------ */

	return p;

} /* pay_liabilities_new() */

LIST *pay_liabilities_get_current_liability_entity_list(
			LIST *current_liability_account_list )
{
	LIST *current_liability_entity_list;
	ENTITY *entity;
	ACCOUNT *account;
	ACCOUNT *new_account;
	LIST *journal_ledger_list;
	JOURNAL_LEDGER *journal_ledger;
	double difference;

	if ( !list_rewind( current_liability_account_list ) )
		return (LIST *)0;

	current_liability_entity_list = list_new();

	do {
		account = list_get_pointer( current_liability_account_list );

		journal_ledger_list = account->journal_ledger_list;

		if ( !list_rewind( journal_ledger_list ) ) continue;

		do {
			journal_ledger =
				list_get_pointer(
					journal_ledger_list );

			entity = entity_get_or_set(
					current_liability_entity_list,
					journal_ledger->full_name,
					journal_ledger->street_address,
					0 /* not with_strdup */ );

			difference =	journal_ledger->credit_amount -
					journal_ledger->debit_amount;

			if ( !entity->liability_account_list )
				entity->liability_account_list =
					list_new();

			new_account =
				ledger_account_get_or_set(
					entity->liability_account_list,
					account->account_name );

			entity->sum_balance += difference;
			new_account->balance += difference;

			if ( !new_account->journal_ledger_list )
				new_account->journal_ledger_list =
					list_new();

			list_append_pointer(
				new_account->journal_ledger_list,
				journal_ledger );

		} while( list_next( journal_ledger_list ) );

	} while( list_next( current_liability_account_list ) );

	return current_liability_entity_list;

} /* pay_liabilities_get_current_liability_entity_list() */

LIST *pay_liabilities_process_get_entity_payable_list(
			LIST *input_entity_payable_list,
			LIST *current_liability_entity_list,
			LIST *input_purchase_order_list,
			double dialog_box_payment_amount )
{
	LIST *entity_payable_list;
	ENTITY *entity;
	ENTITY_PAYABLE *entity_payable;
	ENTITY_PAYABLE *new_entity_payable;

	if ( !list_rewind( input_entity_payable_list ) )
		return (LIST *)0;

	entity_payable_list = list_new();

	do {
		entity_payable =
			list_get_pointer(
				input_entity_payable_list );

		if ( ! ( entity =
				entity_seek(
					current_liability_entity_list,
					entity_payable->full_name,
					entity_payable->street_address ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot find (%s/%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
				entity_payable->full_name,
				entity_payable->street_address );
			exit( 1 );
		}

		new_entity_payable = pay_liabilities_entity_payable_calloc();

		memcpy(	new_entity_payable,
			entity_payable,
			sizeof( ENTITY_PAYABLE ) );

		list_append_pointer( 
			entity_payable_list,
			new_entity_payable );

		if ( dialog_box_payment_amount )
		{
			new_entity_payable->payment_amount =
				dialog_box_payment_amount;
		}
		else
		{
			new_entity_payable->payment_amount =
				entity_payable->sum_balance;
		}

		if (	new_entity_payable->payment_amount >
			new_entity_payable->sum_balance )
		{
			new_entity_payable->loss_amount =
				new_entity_payable->payment_amount -
				new_entity_payable->sum_balance;
		}

		new_entity_payable->current_liability_account_list =
			pay_liabilities_distribute_liability_account_list(
				entity->liability_account_list,
				new_entity_payable->payment_amount );

		new_entity_payable->purchase_order_list =
			pay_liabilities_distribute_purchase_order_list(
				input_purchase_order_list,
				new_entity_payable->payment_amount,
				new_entity_payable->full_name,
				new_entity_payable->street_address );

	} while( list_next( input_entity_payable_list ) );

	return entity_payable_list;

} /* pay_liabilities_process_get_entity_payable_list() */

LIST *pay_liabilities_distribute_purchase_order_list(
				LIST *input_purchase_order_list,
				double payment_amount,
				char *full_name,
				char *street_address )
{
	LIST *purchase_order_list;
	PURCHASE_ORDER *purchase_order;
	PURCHASE_ORDER *new_purchase_order;

	if ( !list_rewind( input_purchase_order_list ) ) return (LIST *)0;

	purchase_order_list = list_new();

	do {
		purchase_order = list_get_pointer( input_purchase_order_list );

		if ( ( strcmp( purchase_order->full_name, full_name ) != 0 )
		||   ( strcmp( purchase_order->street_address,
		       street_address ) != 0 ) )
		{
			continue;
		}

		new_purchase_order = purchase_order_calloc();

		memcpy(	new_purchase_order,
			purchase_order,
			sizeof( PURCHASE_ORDER ) );

		list_append_pointer(
			purchase_order_list,
			new_purchase_order );

		/* If made a partial payment */
		/* ------------------------- */
		if ( payment_amount < purchase_order->amount_due )
		{
			new_purchase_order->amount_due -= payment_amount;
			payment_amount = 0.0;
		}
		else
		{
			new_purchase_order->amount_due = 0.0;
			payment_amount -= purchase_order->amount_due;
		}

		if ( timlib_dollar_virtually_same( payment_amount, 0.0 )
		||   payment_amount <= 0.0 )
		{
			break;
		}

	} while( list_next( input_purchase_order_list ) );

	return purchase_order_list;

} /* pay_liabilities_distribute_purchase_order_list() */

LIST *pay_liabilities_fetch_current_liability_account_list(
				char *application_name )
{
	LIST *account_list;
	ACCOUNT *account;
	char *select;
	char *where;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;

	account_list = list_new();

	select =
		ledger_account_get_select(
			application_name );

	where =
"subclassification = 'current_liability' and account <> 'uncleared_checks'";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=account		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		account = ledger_account_calloc();

		ledger_account_parse(
				&account->account_name,
				&account->fund_name,
				&account->subclassification_name,
				&account->hard_coded_account_key,
				input_buffer );

		account->journal_ledger_list =
			ledger_get_after_balance_zero_journal_ledger_list(
				application_name,
				account->account_name );

		if ( list_length( account->journal_ledger_list ) )
		{
			list_append_pointer( account_list, account );
		}
	}

	pclose( input_pipe );
	return account_list;

} /* pay_liabilities_fetch_current_liability_account_list() */

LIST *pay_liabilities_fetch_liability_account_entity_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char account_name[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char *folder;
	char *select;
	ENTITY *entity;
	JOURNAL_LEDGER *latest_ledger;
	ACCOUNT *account;
	LIST *liability_account_entity_list = list_new();

	folder = "liability_account_entity";
	select = "account,full_name,street_address";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder );

	input_pipe = popen( sys_string, "r" );

	while( timlib_get_line( input_buffer, input_pipe, 512 ) )
	{
		piece( account_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );

		if ( ! ( latest_ledger =
				ledger_get_latest_ledger(
					application_name,
					account_name,
					(char *)0 /* as_of_date */ ) ) )
		{
			continue;
		}

		if ( !latest_ledger->balance ) continue;

		piece( full_name, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		piece( street_address, FOLDER_DATA_DELIMITER, input_buffer, 2 );

		entity =
			entity_get_or_set(
				liability_account_entity_list,
				full_name,
				street_address,
				1 /* with_strdup */ );

		entity->sum_balance += latest_ledger->balance;

		account = ledger_account_new( strdup( account_name ) );
		account->latest_ledger = latest_ledger;
		account->balance = account->latest_ledger->balance;

		/* Prior assignment assigns the local memory. */
		/* ------------------------------------------ */
		account->latest_ledger->account_name = account->account_name;

		if ( !entity->liability_account_list )
			entity->liability_account_list = list_new();

		list_append_pointer( entity->liability_account_list, account );
	}

	pclose( input_pipe );

	return liability_account_entity_list;

} /* pay_liabilities_fetch_liability_account_entity_list() */

LIST *pay_liabilities_get_liability_account_entity_list(
		LIST *input_liability_account_entity_list,
		LIST *input_entity_payable_list,
		double dialog_box_payment_amount )
{
	LIST *liability_account_entity_list;
	ENTITY *entity;
	ENTITY *new_entity;
	ENTITY_PAYABLE *entity_payable;

	if ( !list_rewind( input_liability_account_entity_list ) )
		return (LIST *)0;

	liability_account_entity_list = list_new();

	do {
		entity =
			list_get_pointer(
				input_liability_account_entity_list );

		new_entity = entity_calloc();

		memcpy( new_entity, entity, sizeof( ENTITY ) );

		list_append_pointer( 
			liability_account_entity_list,
			new_entity );

		if ( dialog_box_payment_amount )
		{
			new_entity->payment_amount = dialog_box_payment_amount;
			goto distribute;
		}

		if ( ! ( entity_payable =
				pay_liabilities_entity_payable_seek(
					input_entity_payable_list,
					new_entity->full_name,
					new_entity->street_address ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot seek entity_payable(%s/%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 new_entity->full_name,
				 new_entity->street_address );
			exit( 1 );
		}

		new_entity->payment_amount =
			entity_payable->sum_balance;

distribute:
		new_entity->liability_account_list =
			pay_liabilities_distribute_liability_account_list(
				entity->liability_account_list,
				new_entity->payment_amount );

		if (	new_entity->payment_amount >
			new_entity->sum_balance )
		{
			new_entity->loss_amount =
				new_entity->payment_amount -
				new_entity->sum_balance;
		}
			
	} while( list_next( input_liability_account_entity_list ) );

	return liability_account_entity_list;

} /* pay_liabilities_get_liability_account_entity_list() */

LIST *pay_liabilities_distribute_liability_account_list(
			LIST *liability_account_list,
			double entity_payment_amount )
{
	LIST *return_liability_account_list;
	ACCOUNT *account;
	ACCOUNT *new_account;

	if ( !list_rewind( liability_account_list ) ) return (LIST *)0;

	return_liability_account_list = list_new();

	do {
		account = list_get_pointer( liability_account_list );

		if ( !account->balance )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: balance is zero for account = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 account->account_name );
			exit( 1 );
		}

		new_account = ledger_account_calloc();

		memcpy( new_account, account, sizeof( ACCOUNT ) );

		/* If partial payment. */
		/* ------------------- */
		if ( entity_payment_amount < account->balance )
		{
			new_account->payment_amount =
				entity_payment_amount;

			entity_payment_amount = 0.0;
		}
		else
		{
			new_account->payment_amount =
				account->balance;

			entity_payment_amount -= account->balance;
		}

		list_append_pointer(
			return_liability_account_list,
			new_account );

		if ( timlib_dollar_virtually_same(
			entity_payment_amount, 0.0 ) )
		{
			break;
		}

	} while( list_next( liability_account_list ) );

	return return_liability_account_list;

} /* pay_liabilities_distribute_liability_account_list() */

ENTITY_PAYABLE *pay_liabilities_entity_payable_calloc( void )
{
	ENTITY_PAYABLE *p;

	if ( ! ( p =
		(ENTITY_PAYABLE *)
			calloc(	1,
				sizeof( ENTITY_PAYABLE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return p;

} /* pay_liabilities_entity_payable_calloc() */

ENTITY_PAYABLE *pay_liabilities_entity_payable_new(
				char *full_name,
				char *street_address )
{
	ENTITY_PAYABLE *p;

	p = pay_liabilities_entity_payable_calloc();

	if ( ! ( p =
		(ENTITY_PAYABLE *)
			calloc(	1,
				sizeof( ENTITY_PAYABLE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	p->full_name = full_name;
	p->street_address = street_address;
	return p;

} /* pay_liabilities_entity_payable_new() */

LIST *pay_liabilities_input_get_entity_payable_list(
			char *application_name,
			char *fund_name,
			LIST *full_name_list,
			LIST *street_address_list )
{
	char *full_name;
	char *street_address;
	ENTITY_PAYABLE *entity_payable;
	LIST *entity_payable_list;

	if ( !list_rewind( full_name_list ) ) return (LIST *)0;

	list_rewind( street_address_list );
	entity_payable_list = list_new();

	do {
		full_name = list_get_pointer( full_name_list );
		street_address = list_get_pointer( street_address_list );

		entity_payable =
			pay_liabilities_entity_payable_new(
				full_name,
				street_address );

		if ( ! ( entity_payable->sum_balance =
				pay_liabilities_get_sum_balance(
					application_name,
					fund_name,
					full_name,
					street_address ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get sum_balance for %s/%s.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 full_name,
				 street_address );
			exit( 1 );
		}

		list_append_pointer(
			entity_payable_list,
			entity_payable );

		list_next( street_address_list );

	} while( list_next( full_name_list ) );

	return entity_payable_list;

} /* pay_liabilities_input_get_entity_payable_list() */

ENTITY_PAYABLE *pay_liabilities_entity_payable_seek(
				LIST *entity_payable_list,
				char *full_name,
				char *street_address )
{
	ENTITY_PAYABLE *entity_payable;

	if ( !list_rewind( entity_payable_list ) )
		return (ENTITY_PAYABLE *)0;

	do {
		entity_payable = list_get( entity_payable_list );

		if ( timlib_strcmp(
				full_name,
				entity_payable->full_name ) == 0
		&&   timlib_strcmp(
				street_address,
				entity_payable->street_address ) == 0 )
		{
			return entity_payable;
		}

	} while( list_next( entity_payable_list ) );

	return (ENTITY_PAYABLE *)0;

} /* pay_liabilities_entity_payable_seek() */

#ifdef NOT_DEFINED
ENTITY_ACCOUNT_DEBIT *
	pay_liabilities_get_or_set_entity_account_debit(
				LIST *entity_account_debit_list,
				char *account_name )
{
	ENTITY_ACCOUNT_DEBIT *entity_account_debit;

	if ( !entity_account_debit_list )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: null entity_account_debit_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( entity_account_debit_list ) )
	{
		goto set_list;
	}

	do {
		entity_account_debit =
			list_get_pointer( 
				entity_account_debit_list );

		if ( strcmp(
			entity_account_debit->account_name,
			account_name ) == 0 )
		{
			return entity_account_debit;
		}

	} while( list_next( entity_account_debit_list ) );

set_list:

	entity_account_debit =
		pay_liabilities_entity_account_debit_new(
			account_name );

	list_append_pointer(	entity_account_debit_list,
				entity_account_debit );

	return entity_account_debit;

} /* pay_liabilities_get_or_set_entity_account_debit() */

LIST *pay_liabilities_fetch_purchase_order_list(
				char *application_name )
{
	LIST *purchase_order_list;

	purchase_order_list = list_new();

	do {
		account = list_get_pointer( current_liability_account_list );

		pay_liabilities_set_purchase_order_list(
			purchase_order_list,
			remaining_check_amount,
			application_name,
			account->journal_ledger_list,
			full_name,
			street_address );

	} while( list_next( current_liability_account_list ) );

	return purchase_order_list;

} /* pay_liabilities_fetch_purchase_order_list() */

LIST *pay_liabilities_get_entity_account_debit_list(
				double *remaining_check_amount,
				LIST *current_liability_account_list,
				char *full_name,
				char *street_address )
{
	LIST *entity_account_debit_list;
	ACCOUNT *account;

	if ( !list_rewind( current_liability_account_list ) )
		return (LIST *)0;

	if ( timlib_dollar_virtually_same(
		*remaining_check_amount,
		0.0 ) )
	{
		return (LIST *)0;
	}

	entity_account_debit_list = list_new();

	do {
		account = list_get_pointer( current_liability_account_list );

		pay_liabilities_set_entity_account_debit_list(
			entity_account_debit_list,
			remaining_check_amount,
			account->journal_ledger_list,
			full_name,
			street_address,
			account->account_name );

		if ( timlib_dollar_virtually_same(
			*remaining_check_amount,
			0.0 ) )
		{
			return entity_account_debit_list;
		}

	} while( list_next( current_liability_account_list ) );

	if ( *remaining_check_amount <= -0.01 )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: returned a negative remaining_check_amount = %.2lf.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 *remaining_check_amount );
	}

	return entity_account_debit_list;

} /* pay_liabilities_get_entity_account_debit_list() */

void pay_liabilities_set_purchase_order_list(
			LIST *purchase_order_list,
			double *remaining_check_amount,
			char *application_name,
			LIST *journal_ledger_list,
			char *full_name,
			char *street_address )
{
	PURCHASE_ORDER *purchase_order;
	JOURNAL_LEDGER *journal_ledger;

	/* Set the purchase_order_list */
	/* --------------------------- */
	list_go_tail( journal_ledger_list );

	do {
		journal_ledger =
			list_get_pointer(
				journal_ledger_list );

		if ( strcmp(	journal_ledger->full_name,
				full_name ) != 0
		||   strcmp(	journal_ledger->street_address,
				street_address ) != 0 )
		{
			continue;
		}

		if ( ( purchase_order =
			purchase_order_transaction_date_time_fetch(
				application_name,
				full_name,
				street_address,
				journal_ledger->transaction_date_time ) )
		&&     purchase_order->amount_due )
		{
			list_append_pointer(
				purchase_order_list,
				purchase_order );

			*remaining_check_amount -=
				purchase_order->amount_due;

			if ( *remaining_check_amount <= 0.0 )
			{
				return;
			}
		}

	} while( list_previous( journal_ledger_list ) );

} /* pay_liabilities_set_purchase_order_list() */

void pay_liabilities_set_entity_account_debit_list(
			LIST *entity_account_debit_list,
			double *remaining_check_amount,
			LIST *journal_ledger_list,
			char *full_name,
			char *street_address,
			char *account_name )
{
	ENTITY_ACCOUNT_DEBIT *entity_account_debit;
	double entity_account_credit_balance;

	if ( timlib_dollar_virtually_same(
		*remaining_check_amount,
		0.0 ) )
	{
		return;
	}

	entity_account_credit_balance =
		pay_liabilities_get_entity_account_credit_balance(
			journal_ledger_list,
			full_name,
			street_address );

	/* ----------------------------------------------------	*/
	/* A negative value indicates a liability debit balance.*/
	/* ---------------------------------------------------- */
	if ( entity_account_credit_balance <= 0.0 )
	{
		return;
	}

	entity_account_debit =
		pay_liabilities_get_or_set_entity_account_debit(
			entity_account_debit_list,
			account_name );

	if ( entity_account_credit_balance <= *remaining_check_amount )
	{
		entity_account_debit->debit_amount =
			entity_account_credit_balance;

		*remaining_check_amount -= entity_account_credit_balance;
	}
	else
	/* ---------------------------- */
	/* If making a partial payment. */
	/* ---------------------------- */
	{
		entity_account_debit->debit_amount =
			*remaining_check_amount;

		*remaining_check_amount = 0.0;
	}

} /* pay_liabilities_set_entity_account_debit_list() */

double pay_liabilities_get_entity_account_credit_balance(
			LIST *journal_ledger_list,
			char *full_name,
			char *street_address )
{
	double credit_balance;
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( journal_ledger_list ) ) return 0.0;

	credit_balance = 0.0;

	do {
		journal_ledger =
			list_get_pointer(
				journal_ledger_list );

		if ( strcmp(	journal_ledger->full_name,
				full_name ) != 0
		||   strcmp(	journal_ledger->street_address,
				street_address ) != 0 )
		{
			continue;
		}

		if ( journal_ledger->credit_amount )
		{
			credit_balance += journal_ledger->credit_amount;
		}
		else
		{
			credit_balance -= journal_ledger->debit_amount;
		}

	} while( list_next( journal_ledger_list ) );

	return credit_balance;

} /* pay_liabilities_get_entity_account_credit_balance() */

/* Returns seconds_to_add */
/* ---------------------- */
int pay_liabilities_insert_vendor_payment(
				char **propagate_transaction_date_time,
				double *remaining_check_amount,
				LIST *purchase_order_list,
				char *application_name,
				char *credit_account_name,
				char *account_payable_name,
				int seconds_to_add,
				int check_number,
				char *memo )
{
	PURCHASE_ORDER *purchase_order;
	DATE *transaction_date_time;
	char *transaction_date_time_string;
	TRANSACTION *transaction;

	if ( !list_rewind( purchase_order_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty purchase_order_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	transaction_date_time = date_now_new( date_get_utc_offset() );

	if ( !memo || !*memo || strcmp( memo, "memo" ) == 0 )
		memo = PAY_LIABILITIES_MEMO;

	do {
		purchase_order = list_get( purchase_order_list );

		date_increment_seconds(
			transaction_date_time,
			seconds_to_add,
			date_get_utc_offset() );

		transaction_date_time_string =
			date_display_yyyy_mm_dd_colon_hms(
				transaction_date_time );

		transaction =
			ledger_transaction_new(
				purchase_order->full_name,
				purchase_order->street_address,
				transaction_date_time_string,
				memo );

		transaction_date_time_string =
		transaction->transaction_date_time =
		ledger_transaction_insert(
			application_name,
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			purchase_order->amount_due,
			transaction->memo,
			check_number,
			1 /* lock_transaction */ );

		ledger_journal_ledger_insert(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			transaction->transaction_date_time,
			account_payable_name,
			purchase_order->amount_due,
			1 /* is_debit */ );

		ledger_journal_ledger_insert(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			transaction->transaction_date_time,
			credit_account_name,
			purchase_order->amount_due,
			0 /* not is_debit */ );

		purchase_vendor_payment_insert(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->purchase_date_time,
			transaction_date_time_string
				/* payment_date_time */,
			purchase_order->amount_due,
			check_number,
			transaction_date_time_string );

		if ( !*propagate_transaction_date_time )
		{
			*propagate_transaction_date_time =
				transaction->transaction_date_time;
		}

		*remaining_check_amount -= purchase_order->amount_due;

		purchase_order->amount_due = 0.0;

		purchase_order_update(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->purchase_date_time,
			purchase_order->sum_extension,
			purchase_order->database_sum_extension,
			purchase_order->purchase_amount,
			purchase_order->database_purchase_amount,
			purchase_order->amount_due,
			purchase_order->database_amount_due,
			purchase_order->transaction_date_time,
			purchase_order->database_transaction_date_time,
			purchase_order->arrived_date_time,
			purchase_order->database_arrived_date_time,
			purchase_order->shipped_date,
			purchase_order->database_shipped_date );

		seconds_to_add++;

	} while( list_next( purchase_order_list ) );

	return seconds_to_add;

} /* pay_liabilities_insert_vendor_payment() */

char *pay_liabilities_entity_check_amount_list_display(
			LIST *entity_check_amount_list )
{
	ENTITY_CHECK_AMOUNT *entity_check_amount;
	char buffer[ 65536 ];
	char *ptr = buffer;

	*ptr = '\0';

	if ( list_rewind( entity_check_amount_list ) )
	{
		do {
			entity_check_amount =
				list_get(
					entity_check_amount_list );

			ptr += sprintf(
				ptr,
				"\nfull_name = %s;"
				"street_address = %s;"
				"sum_credit_amount_check_amount = %.2lf;"
				"check_number = %d;"
				"loss_amount = %.2lf\n",
				entity_check_amount->full_name,
				entity_check_amount->street_address,
				entity_check_amount->
					sum_credit_amount_check_amount,
				entity_check_amount->check_number,
				entity_check_amount->loss_amount );

			ptr += sprintf(
				ptr,
				"\n"
				"purchase_order_list: %s\n",
				purchase_order_list_display(
					entity_check_amount->
						purchase_order_list ) );

			ptr += sprintf(
				ptr,
				"\n"
				"entity_account_debit_list: %s\n",
				pay_liabilities_entity_account_debit_list_display(
					entity_check_amount->
						entity_account_debit_list ) );

		} while( list_next( entity_check_amount_list ) );

		ptr += sprintf( ptr, "\n" );
	}

	return strdup( buffer );

} /* pay_liabilities_entity_check_amount_list_display() */

char *pay_liabilities_entity_account_debit_list_display(
			LIST *entity_account_debit_list )
{
	ENTITY_ACCOUNT_DEBIT *entity_account_debit;
	char buffer[ 65536 ];
	char *ptr = buffer;

	*ptr = '\0';

	if ( list_rewind( entity_account_debit_list ) )
	{
		do {
			entity_account_debit =
				list_get(
					entity_account_debit_list );

			ptr += sprintf( ptr,
					"\n"
					"account_name = %s;"
					"debit_amount = %.2lf;",
					entity_account_debit->account_name,
					entity_account_debit->debit_amount );

		} while( list_next( entity_account_debit_list ) );

		ptr += sprintf( ptr, "\n" );
	}

	return strdup( buffer );

} /* pay_liabilities_entity_account_debit_list_display() */

char *pay_liabilities_current_liability_account_list_display(
			LIST *current_liability_account_list )
{
	ACCOUNT *account;
	char buffer[ 65536 ];
	char *ptr = buffer;

	*ptr = '\0';

	if ( list_rewind( current_liability_account_list ) )
	{
		do {
			account = list_get( current_liability_account_list );

			ptr += sprintf( ptr,
					"\n"
					"liability_account = %s;"
					"journal_ledger_list = %s;",
					account->account_name,
					ledger_journal_ledger_list_display(
						account->
							journal_ledger_list ) );

		} while( list_next( current_liability_account_list ) );

		ptr += sprintf( ptr, "\n" );
	}

	return strdup( buffer );

} /* pay_liabilities_current_liability_account_list_display() */

char *pay_liabilities_display( PAY_LIABILITIES *pay_liabilities )
{
	char buffer[ 65536 ];
	char *ptr = buffer;

	*ptr = '\0';

	ptr += sprintf(
		ptr,
		"current_liability_account_list: %s\n",
		pay_liabilities_current_liability_account_list_display(
			pay_liabilities->current_liability_account_list ) );

	ptr += sprintf(
		ptr,
		"entity_check_amount_list: %s\n",
		pay_liabilities_entity_check_amount_list_display(
			pay_liabilities->entity_check_amount_list ) );

	return strdup( buffer );

} /* pay_liabilities_display() */

boolean pay_liabilities_insert_entity_check_amount_list(
				char *application_name,
				char *fund_name,
				LIST *entity_check_amount_list,
				double dialog_box_payment_amount,
				char *memo,
				int starting_check_number )
{
	ENTITY_CHECK_AMOUNT *entity_check_amount;
	LIST *distinct_account_name_list;
	char *propagate_transaction_date_time = {0};
	char *checking_account = {0};
	char *uncleared_checks_account = {0};
	char *account_payable_name = {0};
	char *credit_account_name;
	int seconds_to_add = 0;
	double check_amount;

	distinct_account_name_list = list_new();

	ledger_get_vendor_payment_account_names(
				&checking_account,
				&uncleared_checks_account,
				&account_payable_name,
				application_name,
				fund_name );

	if ( starting_check_number )
		credit_account_name = uncleared_checks_account;
	else
		credit_account_name = checking_account;

	list_append_pointer(
		distinct_account_name_list,
		credit_account_name );

	list_append_pointer(
		distinct_account_name_list,
		account_payable_name );

	if ( !list_rewind( entity_check_amount_list ) ) return 0;

	do {
		entity_check_amount =
			list_get(
				entity_check_amount_list );

		check_amount =
			(dialog_box_payment_amount)
				? dialog_box_payment_amount
				: entity_check_amount->
					sum_credit_amount_check_amount;

		seconds_to_add +=
			pay_liabilities_insert_entity_check_amount(
				distinct_account_name_list,
				&propagate_transaction_date_time,
				application_name,
				entity_check_amount,
				check_amount,
				credit_account_name,
				account_payable_name,
				seconds_to_add,
				fund_name,
				memo );

	} while( list_next( entity_check_amount_list ) );

	if ( propagate_transaction_date_time )
	{
		ledger_propagate_account_name_list(
			application_name,
			propagate_transaction_date_time,
			distinct_account_name_list );
	}

	return 1;

} /* pay_liabilities_insert_entity_check_amount_list() */

/* Returns seconds_to_add */
/* ---------------------- */
int pay_liabilities_insert_entity_check_amount(
				LIST *distinct_account_name_list,
				char **propagate_transaction_date_time,
				char *application_name,
				ENTITY_CHECK_AMOUNT *entity_check_amount,
				double check_amount,
				char *credit_account_name,
				char *account_payable_name,
				int seconds_to_add,
				char *fund_name,
				char *memo )
{
	if ( list_length( entity_check_amount->purchase_order_list ) )
	{
		seconds_to_add +=
			pay_liabilities_insert_vendor_payment(
				propagate_transaction_date_time,
				&check_amount /* remaining_check_amount */,
				entity_check_amount->
					purchase_order_list,
				application_name,
				credit_account_name,
				account_payable_name,
				seconds_to_add,
				entity_check_amount->check_number,
				memo );
	}

	if ( list_length( entity_check_amount->entity_account_debit_list ) )
	{
		pay_liabilities_insert_entity_account_debit_list(
			distinct_account_name_list,
			propagate_transaction_date_time,
			check_amount /* remaining_check_amount */,
			application_name,
			entity_check_amount->
				entity_account_debit_list,
			entity_check_amount->check_number,
			credit_account_name,
			seconds_to_add,
			entity_check_amount->full_name,
			entity_check_amount->street_address,
			entity_check_amount->loss_amount,
			fund_name,
			memo );
	}

	return seconds_to_add;

} /* pay_liabilities_insert_entity_check_amount() */

void pay_liabilities_insert_entity_account_debit_list(
				LIST *distinct_account_name_list,
				char **propagate_transaction_date_time,
				double remaining_check_amount,
				char *application_name,
				LIST *entity_account_debit_list,
				int check_number,
				char *uncleared_checks_account,
				int seconds_to_add,
				char *full_name,
				char *street_address,
				double loss_amount,
				char *fund_name,
				char *memo )
{
	ENTITY_ACCOUNT_DEBIT *entity_account_debit;
	TRANSACTION *transaction;
	DATE *transaction_date_time;
	char *transaction_date_time_string;
	static char *loss_account = {0};

	if ( !list_rewind( entity_account_debit_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty entity_account_debit_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	transaction_date_time = date_now_new( date_get_utc_offset() );

	if ( !memo || !*memo || strcmp( memo, "memo" ) == 0 )
		memo = PAY_LIABILITIES_MEMO;

	date_increment_seconds(
		transaction_date_time,
		seconds_to_add,
		date_get_utc_offset() );

	transaction_date_time_string =
		date_display_yyyy_mm_dd_colon_hms(
			transaction_date_time );

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time_string,
			memo );

	transaction_date_time_string =
	transaction->transaction_date_time =
	ledger_transaction_insert(
		application_name,
		transaction->full_name,
		transaction->street_address,
		transaction->transaction_date_time,
		remaining_check_amount,
		transaction->memo,
		check_number,
		0 /* not lock_transaction */ );

	ledger_journal_ledger_insert(
		application_name,
		transaction->full_name,
		transaction->street_address,
		transaction->transaction_date_time,
		uncleared_checks_account,
		remaining_check_amount,
		0 /* not is_debit */ );

	if ( !*propagate_transaction_date_time )
	{
		*propagate_transaction_date_time =
			transaction->transaction_date_time;
	}

	do {
		entity_account_debit =
			list_get(
			entity_account_debit_list );

		if ( timlib_dollar_virtually_same(
			entity_account_debit->debit_amount,
			0.0 ) )
		{
			fprintf( stderr,
		"Warning in %s/%s()/%d: debit_amount shouldn't be 0.0\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
		}

		ledger_journal_ledger_insert(
			application_name,
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			entity_account_debit->account_name,
			entity_account_debit->debit_amount,
			1 /* is_debit */ );

		list_append_unique_string(
			distinct_account_name_list,
			entity_account_debit->account_name );

		remaining_check_amount -= entity_account_debit->debit_amount;

		if ( timlib_dollar_virtually_same(
			remaining_check_amount,
			0.0 ) )
		{
			break;
		}

	} while( list_next( entity_account_debit_list ) );

	if ( loss_amount )
	{
		if ( !loss_account )
		{
			loss_account =
				ledger_get_hard_coded_account_name(
					application_name,
					fund_name,
					LEDGER_LOSS_KEY,
					0 /* not warning_only */ );

			list_append_unique_string(
				distinct_account_name_list,
				loss_account );
		}

		ledger_journal_ledger_insert(
			application_name,
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			loss_account,
			loss_amount,
			1 /* is_debit */ );

	} /* if loss_amount */

	seconds_to_add++;

} /* pay_liabilities_insert_entity_account_debit_list() */

boolean pay_liabilities_set_liability_account_entity(
			LIST **entity_account_debit_list,
			double *loss_amount,
			char *full_name,
			char *street_address,
			double sum_credit_amount_check_amount,
			double check_amount,
			LIST *liability_account_entity_list )
{
	ENTITY *entity;
	ACCOUNT *account;
	ENTITY_ACCOUNT_DEBIT *entity_account_debit;

	if ( !list_rewind( liability_account_entity_list ) ) return 0;

	do {
		entity = list_get_pointer( liability_account_entity_list );

		if ( strcmp( entity->full_name, full_name ) != 0
		||   strcmp( entity->street_address, street_address ) != 0 )
		{
			continue;
		}

		if ( !list_rewind( entity->liability_account_list ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty liability_account_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		*loss_amount =
			check_amount -
			sum_credit_amount_check_amount;

		*entity_account_debit_list = list_new();

		do {
			account =
				list_get_pointer(
					entity->liability_account_list );

			entity_account_debit =
				pay_liabilities_entity_account_debit_new(
					account->account_name );

			if (	account->latest_ledger->balance >
				check_amount )
			{
				entity_account_debit->debit_amount =
					account->latest_ledger->balance -
					check_amount;
			}
			else
			{
				entity_account_debit->debit_amount =
					account->latest_ledger->balance;
			}

			check_amount -= account->latest_ledger->balance;

			list_append_pointer(
				*entity_account_debit_list,
				entity_account_debit );

			if ( check_amount <= 0 ) break;

		} while( list_next( entity->liability_account_list ) );

		return 1;

	} while( list_next( liability_account_entity_list ) );

	return 0;

} /* pay_liabilities_set_liability_account_entity() */

LIST *pay_liabilities_get_journal_ledger_list(
				double *remaining_payment_amount,
				char *application_name,
				char *fund_name,
				ENTITY_CHECK_AMOUNT *entity_check_amount,
				int starting_check_number )
{
	ENTITY_ACCOUNT_DEBIT *entity_account_debit;
	PURCHASE_ORDER *purchase_order;
	JOURNAL_LEDGER *ledger;
	char *checking_account = {0};
	char *uncleared_checks_account = {0};
	char *account_payable_name = {0};
	char *credit_account_name;
	char *loss_account_name;
	LIST *journal_ledger_list = list_new();

	ledger_get_vendor_payment_account_names(
				&checking_account,
				&uncleared_checks_account,
				&account_payable_name,
				application_name,
				fund_name );

	if ( starting_check_number )
		credit_account_name = uncleared_checks_account;
	else
		credit_account_name = checking_account;

{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: got length(entity_account_debit_list) = %d\n",
__FILE__,
__FUNCTION__,
__LINE__,
list_length( entity_check_amount->entity_account_debit_list ) );
m2( application_name, msg );
}
	if ( list_rewind( entity_check_amount->entity_account_debit_list ) )
	{
		do {
			entity_account_debit =
				list_get_pointer( 
					entity_check_amount->
						entity_account_debit_list );

			ledger =
				journal_ledger_new(
					entity_check_amount->full_name,
					entity_check_amount->street_address,
					(char *)0 /* transaction_date */,
					entity_account_debit->account_name );

			if (	entity_account_debit->debit_amount >
				*remaining_payment_amount )
			{
				ledger->debit_amount =
					entity_account_debit->debit_amount -
					*remaining_payment_amount;

				*remaining_payment_amount = 0.0;
			}
			else
			{
				ledger->debit_amount =
					entity_account_debit->debit_amount;

				*remaining_payment_amount -=
					entity_account_debit->debit_amount;
			}


			list_append_pointer( journal_ledger_list, ledger );
	
			if ( timlib_dollar_virtually_same(
				*remaining_payment_amount,
				0.0 ) )
			{
				break;
			}

		} while( list_next(
				entity_check_amount->
					entity_account_debit_list ) );
	}

{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: got length(purchase_order_list) = %d\n",
__FILE__,
__FUNCTION__,
__LINE__,
list_length( entity_check_amount->purchase_order_list ) );
m2( application_name, msg );
}
{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: got remaining_payment_amount = %.2lf\n",
__FILE__,
__FUNCTION__,
__LINE__,
*remaining_payment_amount );
m2( application_name, msg );
}
	if ( !timlib_dollar_virtually_same(
		*remaining_payment_amount,
		0.0 )
	&&   list_rewind( entity_check_amount->purchase_order_list ) )
	{
		do {
			purchase_order =
				list_get_pointer( 
					entity_check_amount->
						purchase_order_list );
			ledger =
				journal_ledger_new(
					entity_check_amount->full_name,
					entity_check_amount->street_address,
					(char *)0 /* transaction_date */,
					entity_account_debit->account_name );

			if (	purchase_order->amount_due >
				*remaining_payment_amount )
			{
				ledger->debit_amount =
					purchase_order->amount_due -
					*remaining_payment_amount;

				*remaining_payment_amount = 0.0;
			}
			else
			{
				ledger->debit_amount =
					purchase_order->amount_due;

				*remaining_payment_amount -=
					purchase_order->amount_due;
			}

			list_append_pointer( journal_ledger_list, ledger );
	
			if ( timlib_dollar_virtually_same(
				*remaining_payment_amount,
				0.0 ) )
			{
				break;
			}

		} while( list_next(
				entity_check_amount->
					purchase_order_list ) );
	}

	if ( !timlib_dollar_virtually_same(
		entity_check_amount->loss_amount,
		0.0 ) )
	{
		loss_account_name =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				LEDGER_LOSS_KEY,
				0 /* not warning_only */ );

		ledger =
			journal_ledger_new(
				entity_check_amount->full_name,
				entity_check_amount->street_address,
				(char *)0 /* transaction_date */,
				loss_account_name );

		ledger->credit_amount = entity_check_amount->loss_amount;
		list_append_pointer( journal_ledger_list, ledger );
	}

	ledger =
		journal_ledger_new(
			entity_check_amount->full_name,
			entity_check_amount->street_address,
			(char *)0 /* transaction_date */,
			credit_account_name );

	ledger->credit_amount =
		entity_check_amount->
			sum_credit_amount_check_amount;

	list_append_pointer( journal_ledger_list, ledger );

	return journal_ledger_list;

} /* pay_liabilities_get_journal_ledger_list() */

ENTITY_CHECK_AMOUNT *pay_liabilities_get_entity_check_amount(
					char *application_name,
					char *fund_name,
					char *full_name,
					char *street_address,
					LIST *current_liability_account_list,
					LIST *liability_account_entity_list,
					double dialog_box_payment_amount )
{
	char sys_string[ 128 ];
	char *entity_record;
	char input_full_name[ 128 ];
	char input_street_address_balance[ 256 ];
	char input_street_address[ 128 ];
	char input_check_amount[ 16 ];
	static LIST *entity_record_list = {0};
	ENTITY_CHECK_AMOUNT *entity_check_amount;
	double check_amount;

	if ( !entity_record_list )
	{
		sprintf( sys_string,
	 		"populate_pay_liabilities_entity %s '%s'",
	 		application_name,
			fund_name );

		entity_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( entity_record_list ) )
		return (ENTITY_CHECK_AMOUNT *)0;

	do {
		entity_record = list_get_pointer( entity_record_list );

		if ( character_count(
			FOLDER_DATA_DELIMITER,
			entity_record ) != 1 )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: not one delimiter in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 entity_record );

			exit( 1 );
		}

		piece(	input_full_name,
			FOLDER_DATA_DELIMITER,
			entity_record,
			0 );

		piece(	input_street_address_balance,
			FOLDER_DATA_DELIMITER,
			entity_record,
			1 );

		piece(	input_street_address,
			'[',
			input_street_address_balance,
			0 );

		piece(	input_check_amount,
			'[',
			input_street_address_balance,
			1 );

		if ( strcmp( input_full_name, full_name ) != 0
		||   strcmp( input_street_address, street_address ) != 0 )
		{
			continue;
		}

		entity_check_amount =
			pay_liabilities_entity_check_amount_new(
				strdup( full_name ),
				strdup( street_address ) );

		entity_check_amount->sum_credit_amount_check_amount =
			atof( input_check_amount );

		check_amount = (dialog_box_payment_amount)
				? dialog_box_payment_amount
				: entity_check_amount->
					sum_credit_amount_check_amount;

		if ( pay_liabilities_set_liability_account_entity(
			&entity_check_amount->entity_account_debit_list,
			&entity_check_amount->loss_amount,
			entity_check_amount->full_name,
			entity_check_amount->street_address,
			entity_check_amount->sum_credit_amount_check_amount,
			check_amount,
			liability_account_entity_list ) )
		{
			return entity_check_amount;
		}

		entity_check_amount->purchase_order_list =
			pay_liabilities_fetch_purchase_order_list(
				&check_amount
					/* remaining_check_amount */,
				application_name,
				current_liability_account_list,
				entity_check_amount->full_name,
				entity_check_amount->street_address );

		if ( !timlib_dollar_virtually_same(
			check_amount,
			0.0 ) )
		{
			entity_check_amount->
				entity_account_debit_list =
				     pay_liabilities_get_entity_account_debit_list(
						&check_amount
						   /* remaining_check_amount */,
						current_liability_account_list,
						entity_check_amount->full_name,
						entity_check_amount->
							street_address );
		}
	
		/* ------------------------------------------- */
		/* If dialog_box_payment_amount > amount_owed, */
		/* then still need to print the check, so      */
		/* record the difference as a loss.            */
		/* ------------------------------------------- */
		if ( check_amount >= 0.01 )
		{
			entity_check_amount->loss_amount = check_amount;
		}

		return entity_check_amount;

	} while( list_next( entity_record_list ) );

	return (ENTITY_CHECK_AMOUNT *)0;

} /* pay_liabilities_get_entity_check_amount() */

ENTITY *pay_liabilities_get_or_set_liability_account_entity(
				LIST *liability_account_entity_list,
				char *full_name,
				char *street_address )
{
	ENTITY *entity;

	if ( !liability_account_entity_list )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: null liability_account_entity_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( liability_account_entity_list ) )
	{
		goto set_list;
	}

	do {
		entity =
			list_get_pointer( 
				liability_account_entity_list );

		if ( timlib_strcmp(
			entity->full_name,
			full_name ) == 0
		&&   timlib_strcmp(
			entity->street_address,
			street_address ) == 0 )
		{
			return entity;
		}

	} while( list_next( liability_account_entity_list ) );

set_list:

	entity = entity_new(	strdup( full_name ),
				strdup( street_address ) );

	list_append_pointer(	liability_account_entity_list,
				entity );

	return entity;

} /* pay_liabilities_get_or_set_liability_account_entity() */

#endif
