/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/print_checks.c			*/
/* -------------------------------------------------------------------- */
/* This is the appaserver print_checks ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include "timlib.h"
#include "ledger.h"
#include "appaserver_library.h"
#include "piece.h"
#include "date.h"
#include "print_checks.h"

PRINT_CHECKS *print_checks_new(	char *application_name,
				LIST *full_name_list,
				LIST *street_address_list,
				int starting_check_number,
				double dialog_box_check_amount )
{
	PRINT_CHECKS *p;

	if ( ! ( p = (PRINT_CHECKS *)calloc( 1, sizeof( PRINT_CHECKS ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	p->current_liability_account_list =
		print_checks_get_current_liability_account_list(
			application_name );

	p->entity_check_amount_list =
		print_checks_get_entity_check_amount_list(
			application_name,
			full_name_list,
			street_address_list,
			starting_check_number );

	print_checks_set_entity_account_debit_list(
		p->entity_check_amount_list,
		application_name,
		p->current_liability_account_list,
		dialog_box_check_amount );

	return p;

} /* print_checks_new() */

ENTITY_CHECK_AMOUNT *print_checks_entity_check_amount_new(
				char *full_name,
				char *street_address )
{
	ENTITY_CHECK_AMOUNT *p;

	if ( ! ( p =
		(ENTITY_CHECK_AMOUNT *)
			calloc(	1,
				sizeof( ENTITY_CHECK_AMOUNT ) ) ) )
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

} /* print_checks_entity_check_amount_new() */

ENTITY_ACCOUNT_DEBIT *print_checks_entity_account_debit_new(
				char *account_name )
{
	ENTITY_ACCOUNT_DEBIT *p;

	if ( ! ( p =
		(ENTITY_ACCOUNT_DEBIT *)
			calloc(	1,
				sizeof( ENTITY_ACCOUNT_DEBIT ) ) ))
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	p->account_name = account_name;
	return p;

} /* print_checks_entity_account_debit_new() */

LIST *print_checks_get_current_liability_account_list(
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
	select = ledger_account_get_select();
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
				&account->display_order,
				&account->hard_coded_account_key,
				&account->tax_form_category_name,
				input_buffer );

		account->journal_ledger_list =
			print_checks_get_after_balance_zero_journal_ledger_list(
				application_name,
				account->account_name );

		list_append_pointer( account_list, account );
	}

	pclose( input_pipe );
	return account_list;

} /* print_checks_get_current_liability_account_list() */

LIST *print_checks_get_after_balance_zero_journal_ledger_list(
				char *application_name,
				char *account_name )
{
	char *transaction_date_time;

	transaction_date_time =
		ledger_get_latest_zero_balance_transaction_date_time(
			application_name,
			account_name );

	return ledger_get_journal_ledger_list(
				application_name,
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				transaction_date_time,
				account_name );

} /* print_checks_get_after_balance_zero_journal_ledger_list() */

LIST *print_checks_get_entity_check_amount_list(
			char *application_name,
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number )
{
	char *full_name;
	char *street_address;
	ENTITY_CHECK_AMOUNT *entity_check_amount;
	LIST *entity_check_amount_list;

	if ( !list_rewind( full_name_list ) ) return (LIST *)0;

	list_rewind( street_address_list );
	entity_check_amount_list = list_new();

	do {
		full_name = list_get_pointer( full_name_list );
		street_address = list_get_pointer( street_address_list );

		entity_check_amount =
			print_checks_get_entity_check_amount(
				application_name,
				full_name,
				street_address );

		if ( !entity_check_amount )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot get entity_check_amount for (%s/%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 full_name,
				 street_address );
			exit( 1 );
		}

		entity_check_amount->check_number = starting_check_number++;

		list_append_pointer(
			entity_check_amount_list,
			entity_check_amount );

		list_next( street_address_list );

	} while( list_next( full_name_list ) );

	return entity_check_amount_list;

} /* print_checks_get_entity_check_amount_list() */

ENTITY_CHECK_AMOUNT *print_checks_get_entity_check_amount(
					char *application_name,
					char *full_name,
					char *street_address )
{
	char sys_string[ 128 ];
	char *results;
	char input_full_name[ 128 ];
	char input_street_address_balance[ 32 ];
	char input_street_address[ 128 ];
	char input_check_amount[ 16 ];
	static LIST *entity_record_list = {0};
	ENTITY_CHECK_AMOUNT *entity_check_amount;

	if ( !entity_record_list )
	{
		sprintf( sys_string,
	 		"populate_print_checks_entity.sh %s",
	 		application_name );

		entity_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( entity_record_list ) )
		return (ENTITY_CHECK_AMOUNT *)0;

	do {
		results = list_get_pointer( entity_record_list );

		if ( character_count(
			FOLDER_DATA_DELIMITER,
			results ) != 1 )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: not one delimiter in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 results );

			exit( 1 );
		}

		piece(	input_full_name,
			FOLDER_DATA_DELIMITER,
			results,
			0 );

		piece(	input_street_address_balance,
			FOLDER_DATA_DELIMITER,
			results,
			1 );

		piece(	input_street_address,
			'[',
			input_street_address_balance,
			0 );

		if ( strcmp( input_full_name, full_name ) == 0
		&&   strcmp( input_street_address, street_address ) == 0 )
		{
			piece(	input_check_amount,
				'[',
				input_street_address_balance,
				1 );

			entity_check_amount =
				print_checks_entity_check_amount_new(
					strdup( full_name ),
					strdup( street_address ) );

			entity_check_amount->drop_down_check_amount =
				atof( input_check_amount );

			return entity_check_amount;
		}

	} while( list_next( entity_record_list ) );

	return (ENTITY_CHECK_AMOUNT *)0;

} /* print_checks_get_entity_check_amount() */

void print_checks_set_entity_account_debit_list(
				LIST *entity_check_amount_list,
				char *application_name,
				LIST *current_liability_account_list,
				double dialog_box_check_amount )
{
	ENTITY_CHECK_AMOUNT *entity_check_amount;

	if ( !list_rewind( entity_check_amount_list ) ) return;

	do {
		entity_check_amount =
			list_get_pointer(
				entity_check_amount_list );

		entity_check_amount->entity_account_debit_list =
			print_checks_get_entity_account_debit_list(
				&entity_check_amount->loss_amount,
				current_liability_account_list,
				entity_check_amount->full_name,
				entity_check_amount->street_address,
				dialog_box_check_amount,
				entity_check_amount->drop_down_check_amount );

		entity_check_amount->purchase_order_list =
			print_checks_get_purchase_order_list(
				application_name,
				current_liability_account_list,
				entity_check_amount->full_name,
				entity_check_amount->street_address,
				dialog_box_check_amount,
				entity_check_amount->drop_down_check_amount );

	} while( list_next( entity_check_amount_list ) );

} /* print_checks_set_entity_account_debit_list() */

ENTITY_ACCOUNT_DEBIT *
	print_checks_get_or_set_entity_account_debit(
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
		print_checks_entity_account_debit_new(
			account_name );

	list_append_pointer(	entity_account_debit_list,
				entity_account_debit );

	return entity_account_debit;

} /* print_checks_get_or_set_entity_account_debit() */

LIST *print_checks_get_purchase_order_list(
				char *application_name,
				LIST *current_liability_account_list,
				char *full_name,
				char *street_address,
				double dialog_box_check_amount,
				double drop_down_check_amount )
{
	LIST *purchase_order_list;
	ACCOUNT *account;
	double remaining_check_amount;

	if ( !list_rewind( current_liability_account_list ) )
		return (LIST *)0;

	if ( dialog_box_check_amount )
		remaining_check_amount = dialog_box_check_amount;
	else
		remaining_check_amount = drop_down_check_amount;

	purchase_order_list = list_new();

	do {
		account = list_get_pointer( current_liability_account_list );

		print_checks_set_purchase_order_list(
			purchase_order_list,
			&remaining_check_amount,
			application_name,
			account->journal_ledger_list,
			full_name,
			street_address );

	} while( list_next( current_liability_account_list ) );

	return purchase_order_list;

} /* print_checks_get_purchase_order_list() */

LIST *print_checks_get_entity_account_debit_list(
				double *loss_amount,
				LIST *current_liability_account_list,
				char *full_name,
				char *street_address,
				double dialog_box_check_amount,
				double drop_down_check_amount )
{
	LIST *entity_account_debit_list;
	ACCOUNT *account;
	double remaining_check_amount;

	if ( !list_rewind( current_liability_account_list ) )
		return (LIST *)0;

	if ( dialog_box_check_amount )
		remaining_check_amount = dialog_box_check_amount;
	else
		remaining_check_amount = drop_down_check_amount;

	entity_account_debit_list = list_new();

	do {
		account = list_get_pointer( current_liability_account_list );

		print_checks_set_single_check_entity_account_debit_list(
			entity_account_debit_list,
			&remaining_check_amount,
			account->journal_ledger_list,
			full_name,
			street_address,
			account->account_name );

	} while( list_next( current_liability_account_list ) );

	if ( remaining_check_amount <= 0.01 )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: returned a negative remaining_check_amount = %.2lf.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 remaining_check_amount );
	}

	if ( !timlib_dollar_virtually_same(
		remaining_check_amount,
		0.0 ) )
	{
		*loss_amount = remaining_check_amount;
	}

	return entity_account_debit_list;

} /* print_checks_get_entity_account_debit_list() */

void print_checks_set_purchase_order_list(
			LIST *purchase_order_list,
			double *remaining_check_amount,
			char *application_name,
			LIST *journal_ledger_list,
			char *full_name,
			char *street_address )
{
	PURCHASE_ORDER *purchase_order;
	JOURNAL_LEDGER *journal_ledger;
	double entity_account_credit_balance;

	entity_account_credit_balance =
		print_checks_get_entity_account_credit_balance(
			journal_ledger_list,
			full_name,
			street_address );

	if ( entity_account_credit_balance <= 0.0 ) return;

	*remaining_check_amount -= entity_account_credit_balance;

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
				journal_ledger->transaction_date_time ) ) )
		{
			/* Fetch it again to get everything. */
			/* --------------------------------- */
			purchase_order =
				purchase_order_new(
					application_name,
					purchase_order->full_name,
					purchase_order->street_address,
					purchase_order->purchase_date_time );

			list_append_pointer(
				purchase_order_list,
				purchase_order );
		}

		entity_account_credit_balance -= journal_ledger->credit_amount;

		if ( timlib_dollar_virtually_same(
			entity_account_credit_balance,
			0.0 ) )
		{
			return;
		}

	} while( list_previous( journal_ledger_list ) );

} /* print_checks_set_purchase_order_list() */

void print_checks_set_single_check_entity_account_debit_list(
			LIST *entity_account_debit_list,
			double *remaining_check_amount,
			LIST *journal_ledger_list,
			char *full_name,
			char *street_address,
			char *account_name )
{
	ENTITY_ACCOUNT_DEBIT *entity_account_debit;
	double entity_account_credit_balance;

	entity_account_credit_balance =
		print_checks_get_entity_account_credit_balance(
			journal_ledger_list,
			full_name,
			street_address );

	if ( entity_account_credit_balance <= 0.0 ) return;

	*remaining_check_amount -= entity_account_credit_balance;

	entity_account_debit =
		print_checks_get_or_set_entity_account_debit(
			entity_account_debit_list,
			account_name );

	entity_account_debit->debit_amount += entity_account_credit_balance;

} /* print_checks_set_single_check_entity_account_debit_list() */

void print_checks_insert_transaction_journal_ledger(
			char *application_name,
			LIST *entity_check_amount_list,
			double dialog_box_check_amount )
{
	ENTITY_ACCOUNT_DEBIT *entity_account_debit;
	ENTITY_CHECK_AMOUNT *entity_check_amount;
	LIST *entity_account_debit_list;
	char *propagate_transaction_date_time = {0};
	LIST *distinct_account_name_list;
	TRANSACTION *transaction;

	distinct_account_name_list = list_new();

	list_append_pointer( distinct_account_name_list, "uncleared_checks" );

	if ( !list_rewind( entity_check_amount_list ) ) return;

	do {
		entity_check_amount =
			list_get(
				entity_check_amount_list );

		if ( !entity_check_amount->transaction_date_time )
			continue;

		entity_account_debit_list =
			entity_check_amount->
				entity_account_debit_list;

		if ( !list_rewind( entity_account_debit_list  ) )
		{
			fprintf( stderr,
			 	 "ERROR in %s/%s()/%d: empty list.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
			exit( 1 );
		}

		transaction =
			ledger_transaction_new(
				entity_check_amount->full_name,
				entity_check_amount->street_address,
				entity_check_amount->transaction_date_time,
				PRINT_CHECKS_MEMO );
	
		ledger_transaction_insert(
			application_name,
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			(dialog_box_check_amount)
				? dialog_box_check_amount
				: entity_check_amount->drop_down_check_amount,
			transaction->memo,
			entity_check_amount->check_number,
			0 /* not lock_transaction */ );

		ledger_journal_ledger_insert(
			application_name,
			entity_check_amount->full_name,
			entity_check_amount->street_address,
			transaction->transaction_date_time,
			"uncleared_checks",
			(dialog_box_check_amount)
				? dialog_box_check_amount
				: entity_check_amount->drop_down_check_amount,
			0 /* not is_debit */ );

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
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
				continue;
			}

			ledger_journal_ledger_insert(
				application_name,
				entity_check_amount->full_name,
				entity_check_amount->street_address,
				transaction->transaction_date_time,
				entity_account_debit->account_name,
				entity_account_debit->debit_amount,
				1 /* is_debit */ );

			list_append_unique_string(
				distinct_account_name_list,
				entity_account_debit->account_name );

		} while( list_next( entity_account_debit_list ) );

	} while( list_next( entity_check_amount_list ) );

	ledger_propagate_account_name_list(
		application_name,
		propagate_transaction_date_time,
		distinct_account_name_list );

} /* print_checks_insert_transaction_journal_ledger() */

double print_checks_get_entity_account_credit_balance(
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
			credit_balance += journal_ledger->credit_amount;
		else
			credit_balance -= journal_ledger->debit_amount;

	} while( list_next( journal_ledger_list ) );

	return credit_balance;

} /* print_checks_get_entity_account_credit_balance() */

void print_checks_subtract_purchase_order_amount_due(
			LIST *entity_check_amount_list,
			char *application_name )
{
	ENTITY_CHECK_AMOUNT *entity_check_amount;
	PURCHASE_ORDER *purchase_order;
	char *checking_account;
	char *uncleared_checks_account;
	char *account_payable_account;

	if ( !list_rewind( entity_check_amount_list ) ) return;

	do {
		entity_check_amount = list_get( entity_check_amount_list );


		if ( !list_rewind( entity_check_amount->purchase_order_list ) )
			continue;

		do {
			purchase_order =
				list_get( entity_check_amount->
						purchase_order_list );


			account_payable_account = (char *)0;

			ledger_get_vendor_payment_account_names(
				&checking_account,
				&uncleared_checks_account,
				&account_payable_account,
				application_name,
				purchase_order->fund_name );

			if ( !account_payable_account )
			{
				fprintf( stderr,
"ERRROR in %s/%s()/%d: cannot get account_payable_account for fund = (%s)\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 purchase_order->fund_name );
				exit( 1 );
			}

			print_checks_subtract_amount_due(
				entity_check_amount->
					entity_account_debit_list,
				account_payable_account,
				purchase_order->amount_due );

		} while( list_next( entity_check_amount->
					purchase_order_list ) );

	} while( list_next( entity_check_amount_list ) );

} /* print_checks_subtract_purchase_order_amount_due() */

void print_checks_subtract_amount_due(
				LIST *entity_account_debit_list,
				char *account_payable_account,
				double amount_due )
{
	ENTITY_ACCOUNT_DEBIT *entity_account_debit;

	if ( ! ( entity_account_debit =
			print_checks_entity_account_debit_seek(
				entity_account_debit_list,
				account_payable_account ) ) )
	{
		fprintf( stderr,
"ERRROR in %s/%s()/%d: cannot get entity_account_debit for account = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 account_payable_account );
		exit( 1 );
	}

	entity_account_debit->debit_amount -= amount_due;

} /* print_checks_subtract_amount_due() */

ENTITY_ACCOUNT_DEBIT *print_checks_entity_account_debit_seek(
				LIST *entity_account_debit_list,
				char *account_payable_account )
{
	ENTITY_ACCOUNT_DEBIT *entity_account_debit;

	if ( !list_rewind( entity_account_debit_list ) )
		return (ENTITY_ACCOUNT_DEBIT *)0;

	do {
		entity_account_debit = list_get( entity_account_debit_list );

		if ( strcmp(	account_payable_account,
				entity_account_debit->account_name ) == 0 )
		{
			return entity_account_debit;
		}

	} while( list_next( entity_account_debit_list ) );

	return (ENTITY_ACCOUNT_DEBIT *)0;

} /* print_checks_entity_account_debit_seek() */

void print_checks_set_transaction_date_time(
			LIST *entity_check_amount_list )
{
	ENTITY_CHECK_AMOUNT *entity_check_amount;
	ENTITY_ACCOUNT_DEBIT *entity_account_debit;
	DATE *transaction_date_time;
	char *transaction_date_time_string;
	LIST *entity_account_debit_list;

	if ( !list_rewind( entity_check_amount_list ) ) return;

	transaction_date_time = date_now_new();

	do {
		entity_check_amount =
			list_get(
				entity_check_amount_list );

		entity_account_debit_list =
			entity_check_amount->
				entity_account_debit_list;

		if ( !list_rewind( entity_account_debit_list  ) )
		{
			fprintf( stderr,
			 	 "ERROR in %s/%s()/%d: empty list.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
			exit( 1 );
		}

		do {
			entity_account_debit =
				list_get(
					entity_account_debit_list );

			if ( !timlib_dollar_virtually_same(
				entity_account_debit->debit_amount,
				0.0 ) )
			{
				transaction_date_time_string =
					date_display_yyyy_mm_dd_colon_hms(
						transaction_date_time );

				entity_check_amount->
					transaction_date_time =
						transaction_date_time_string;

				date_increment_seconds(
					transaction_date_time,
					1 );

				break;
			}

		} while( list_next( entity_account_debit_list ) );

	} while( list_next( entity_check_amount_list ) );

} /* print_checks_set_transaction_date_time() */

