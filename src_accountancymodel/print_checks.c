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
				char *fund_name,
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

	p->dialog_box_check_amount = dialog_box_check_amount;

	p->current_liability_account_list =
		print_checks_get_current_liability_account_list(
			application_name );

	p->entity_check_amount_list =
		print_checks_get_entity_check_amount_list(
			application_name,
			fund_name,
			full_name_list,
			street_address_list,
			starting_check_number,
			p->current_liability_account_list,
			p->dialog_box_check_amount );

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

	select =
		ledger_account_get_select();

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

} /* print_checks_get_current_liability_account_list() */

LIST *print_checks_get_entity_check_amount_list(
			char *application_name,
			char *fund_name,
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number,
			LIST *current_liability_account_list,
			double dialog_box_check_amount )
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
				fund_name,
				full_name,
				street_address,
				current_liability_account_list,
				dialog_box_check_amount );

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
					char *fund_name,
					char *full_name,
					char *street_address,
					LIST *current_liability_account_list,
					double dialog_box_check_amount )
{
	char sys_string[ 128 ];
	char *results;
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
	 		"populate_print_checks_entity.sh %s '%s'",
	 		application_name,
			fund_name );

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

			entity_check_amount->sum_credit_amount_check_amount =
				atof( input_check_amount );

			check_amount = (dialog_box_check_amount)
					? dialog_box_check_amount
					: entity_check_amount->
						sum_credit_amount_check_amount;

			entity_check_amount->purchase_order_list =
				print_checks_fetch_purchase_order_list(
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
				     print_checks_get_entity_account_debit_list(
						&check_amount
						   /* remaining_check_amount */,
						current_liability_account_list,
						entity_check_amount->full_name,
						entity_check_amount->
							street_address );
			}
	
			/* ----------------------------------------- */
			/* If dialog_box_check_amount > amount_owed, */
			/* then still need to print the check, so    */
			/* record the difference as a loss.          */
			/* ----------------------------------------- */
			if ( check_amount >= 0.01 )
			{
				entity_check_amount->loss_amount = check_amount;
			}

			return entity_check_amount;
		}

	} while( list_next( entity_record_list ) );

	return (ENTITY_CHECK_AMOUNT *)0;

} /* print_checks_get_entity_check_amount() */

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

LIST *print_checks_fetch_purchase_order_list(
				double *remaining_check_amount,
				char *application_name,
				LIST *current_liability_account_list,
				char *full_name,
				char *street_address )
{
	LIST *purchase_order_list;
	ACCOUNT *account;

	if ( !list_rewind( current_liability_account_list ) )
		return (LIST *)0;

	purchase_order_list = list_new();

	do {
		account = list_get_pointer( current_liability_account_list );

		print_checks_set_purchase_order_list(
			purchase_order_list,
			remaining_check_amount,
			application_name,
			account->journal_ledger_list,
			full_name,
			street_address );

	} while( list_next( current_liability_account_list ) );

	return purchase_order_list;

} /* print_checks_fetch_purchase_order_list() */

LIST *print_checks_get_entity_account_debit_list(
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

		print_checks_set_entity_account_debit_list(
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

} /* print_checks_set_purchase_order_list() */

void print_checks_set_entity_account_debit_list(
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
		print_checks_get_entity_account_credit_balance(
			journal_ledger_list,
			full_name,
			street_address );

	if ( timlib_dollar_virtually_same(
		entity_account_credit_balance,
		0.0 ) )
	{
		return;
	}

	entity_account_debit =
		print_checks_get_or_set_entity_account_debit(
			entity_account_debit_list,
			account_name );

	if ( entity_account_credit_balance <= *remaining_check_amount )
	{
		entity_account_debit->debit_amount =
			entity_account_credit_balance;

		*remaining_check_amount -= entity_account_credit_balance;
	}
	else
	{
		entity_account_debit->debit_amount =
			*remaining_check_amount;

		*remaining_check_amount = 0.0;
	}

} /* print_checks_set_entity_account_debit_list() */

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

/* Returns seconds_to_add */
/* ---------------------- */
int print_checks_insert_vendor_payment(
				char **propagate_transaction_date_time,
				double *remaining_check_amount,
				LIST *purchase_order_list,
				char *application_name,
				char *uncleared_checks_account,
				char *account_payable_account,
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

	if ( !memo || !*memo || strcmp( memo, "memo" ) == 0 )
		memo = PRINT_CHECKS_MEMO;

	transaction_date_time = date_now_new();

	do {
		purchase_order = list_get( purchase_order_list );

		date_increment_seconds(
			transaction_date_time,
			seconds_to_add );

		transaction_date_time_string =
			date_display_yyyy_mm_dd_colon_hms(
				transaction_date_time );

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

		transaction =
			ledger_transaction_new(
				purchase_order->full_name,
				purchase_order->street_address,
				transaction_date_time_string,
				memo );

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
			account_payable_account,
			purchase_order->amount_due,
			1 /* is_debit */ );

		ledger_journal_ledger_insert(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			transaction->transaction_date_time,
			uncleared_checks_account,
			purchase_order->amount_due,
			0 /* not is_debit */ );

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

} /* print_checks_insert_vendor_payment() */

char *print_checks_entity_check_amount_list_display(
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
				"loss_amount = %.2lf;"
				"transaction_date_time = %s\n",
				entity_check_amount->full_name,
				entity_check_amount->street_address,
				entity_check_amount->
					sum_credit_amount_check_amount,
				entity_check_amount->check_number,
				entity_check_amount->loss_amount,
				entity_check_amount->
					transaction_date_time );

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
				print_checks_entity_account_debit_list_display(
					entity_check_amount->
						entity_account_debit_list ) );

		} while( list_next( entity_check_amount_list ) );

		ptr += sprintf( ptr, "\n" );
	}

	return strdup( buffer );

} /* print_checks_entity_check_amount_list_display() */

char *print_checks_entity_account_debit_list_display(
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

} /* print_checks_entity_account_debit_list_display() */

char *print_checks_current_liability_account_list_display(
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

} /* print_checks_current_liability_account_list_display() */

char *print_checks_display( PRINT_CHECKS *print_checks )
{
	char buffer[ 65536 ];
	char *ptr = buffer;

	*ptr = '\0';

	ptr += sprintf(
		ptr,
		"current_liability_account_list: %s\n",
		print_checks_current_liability_account_list_display(
			print_checks->current_liability_account_list ) );

	ptr += sprintf(
		ptr,
		"entity_check_amount_list: %s\n",
		print_checks_entity_check_amount_list_display(
			print_checks->entity_check_amount_list ) );

	return strdup( buffer );

} /* print_checks_display() */

void print_checks_insert_entity_check_amount_list(
				char *application_name,
				char *fund_name,
				LIST *entity_check_amount_list,
				double dialog_box_check_amount,
				char *memo )
{
	ENTITY_CHECK_AMOUNT *entity_check_amount;
	LIST *distinct_account_name_list;
	char *propagate_transaction_date_time = {0};
	char *checking_account = {0};
	char *uncleared_checks_account = {0};
	char *account_payable_account = {0};
	int seconds_to_add = 0;
	double check_amount;

	distinct_account_name_list = list_new();

	ledger_get_vendor_payment_account_names(
				&checking_account,
				&uncleared_checks_account,
				&account_payable_account,
				application_name,
				fund_name );

	list_append_pointer(
		distinct_account_name_list,
		uncleared_checks_account );

	list_append_pointer(
		distinct_account_name_list,
		account_payable_account );

	if ( !list_rewind( entity_check_amount_list ) ) return;

	do {
		entity_check_amount =
			list_get(
				entity_check_amount_list );

		check_amount =
			(dialog_box_check_amount)
				? dialog_box_check_amount
				: entity_check_amount->
					sum_credit_amount_check_amount;

		seconds_to_add +=
			print_checks_insert_entity_check_amount(
				distinct_account_name_list,
				&propagate_transaction_date_time,
				application_name,
				entity_check_amount,
				check_amount,
				uncleared_checks_account,
				account_payable_account,
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

} /* print_checks_insert_entity_check_amount_list() */

/* Returns seconds_to_add */
/* ---------------------- */
int print_checks_insert_entity_check_amount(
				LIST *distinct_account_name_list,
				char **propagate_transaction_date_time,
				char *application_name,
				ENTITY_CHECK_AMOUNT *entity_check_amount,
				double check_amount,
				char *uncleared_checks_account,
				char *account_payable_account,
				int seconds_to_add,
				char *fund_name,
				char *memo )
{
	if ( list_length( entity_check_amount->purchase_order_list ) )
	{
		seconds_to_add +=
			print_checks_insert_vendor_payment(
				propagate_transaction_date_time,
				&check_amount /* remaining_check_amount */,
				entity_check_amount->
					purchase_order_list,
				application_name,
				uncleared_checks_account,
				account_payable_account,
				seconds_to_add,
				entity_check_amount->check_number,
				memo );
	}

	if ( list_length( entity_check_amount->entity_account_debit_list ) )
	{
		print_checks_insert_entity_account_debit_list(
			distinct_account_name_list,
			propagate_transaction_date_time,
			check_amount /* remaining_check_amount */,
			application_name,
			entity_check_amount->
				entity_account_debit_list,
			entity_check_amount->check_number,
			uncleared_checks_account,
			seconds_to_add,
			entity_check_amount->full_name,
			entity_check_amount->street_address,
			entity_check_amount->loss_amount,
			fund_name );
	}

	return seconds_to_add;

} /* print_checks_insert_entity_check_amount() */

void print_checks_insert_entity_account_debit_list(
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
				char *fund_name )
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

	transaction_date_time = date_now_new();

	date_increment_seconds(
		transaction_date_time,
		seconds_to_add );

	transaction_date_time_string =
		date_display_yyyy_mm_dd_colon_hms(
			transaction_date_time );

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time_string,
			PRINT_CHECKS_MEMO );

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
			"ERROR in %s/%s()/%d: debit_amount shouldn't be 0.0\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
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
					"loss_key",
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

} /* print_checks_insert_entity_account_debit_list() */

