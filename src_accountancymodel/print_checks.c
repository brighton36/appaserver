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
#include "print_checks.h"

PRINT_CHECKS *print_checks_new(	void )
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

	return p;

} /* print_checks_new() */

PRINT_CHECKS_ENTITY_CHECK_AMOUNT *print_checks_entity_check_amount_new(
				char *full_name,
				char *street_address )
{
	PRINT_CHECKS_ENTITY_CHECK_AMOUNT *p;

	if ( ! ( p =
		(PRINT_CHECKS_ENTITY_CHECK_AMOUNT *)
			calloc(	1,
				sizeof( PRINT_CHECKS_ENTITY_CHECK_AMOUNT ) ) ) )
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

PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *print_checks_entity_account_debit_new(
				char *full_name,
				char *street_address,
				char *account_name )
{
	PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *p;

	if ( ! ( p =
		(PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *)
			calloc(	1,
				sizeof( PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT ) ) ))
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
			LIST *street_address_list )
{
	char *full_name;
	char *street_address;
	PRINT_CHECKS_ENTITY_CHECK_AMOUNT *entity_check_amount;
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

		list_append_pointer(
			entity_check_amount_list,
			entity_check_amount );

		list_next( street_address_list );

	} while( list_next( full_name_list ) );

	return entity_check_amount_list;

} /* print_checks_get_entity_check_amount_list() */

PRINT_CHECKS_ENTITY_CHECK_AMOUNT *print_checks_get_entity_check_amount(
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
	PRINT_CHECKS_ENTITY_CHECK_AMOUNT *entity_check_amount;

	if ( !entity_record_list )
	{
		sprintf( sys_string,
	 		"populate_print_checks_entity.sh %s",
	 		application_name );

		entity_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( entity_record_list ) )
		return (PRINT_CHECKS_ENTITY_CHECK_AMOUNT *)0;

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

			entity_check_amount->check_amount =
				atof( input_check_amount );

			return entity_check_amount;
		}

	} while( list_next( entity_record_list ) );

	return (PRINT_CHECKS_ENTITY_CHECK_AMOUNT *)0;

} /* print_checks_get_entity_check_amount() */

LIST *print_checks_get_entity_account_debit_list(
				LIST *current_liability_account_list,
				LIST *entity_check_amount_list )
{
	LIST *entity_account_debit_list;
	ACCOUNT *account;
	JOURNAL_LEDGER *journal_ledger;
	PRINT_CHECKS_ENTITY_CHECK_AMOUNT *entity_check_amount;
	PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *entity_account_debit;

	if ( list_rewind( entity_check_amount_list ) ) return (LIST *)0;

	entity_account_debit_list = list_new();

	do {
		entity_check_amount =
			list_get_pointer(
				entity_check_amount_list );

		if ( ( entity_account_debit =
			print_checks_get_entity_account_debit(
				current_liability_account_list,
				entity_check_amount->full_name,
				entity_check_amount->street_address,
				entity_check_amount->check_amount ) ) )
		{
			list_append_pointer(
				entity_account_debit_list;
				entity_account_debit ):
		}

	} while( list_next( entity_check_amount_list ) );

	return entity_account_debit_list;

} /* print_checks_get_entity_account_debit_list() */

PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *
	print_checks_get_or_set_entity_account_debit(
				LIST *entity_account_debit_list,
				char *full_name,
				char *street_address,
				char *account_name )
{
	PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *entity_account_debit;

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
		goto skip_list;
	}

	do {
		entity_account_debit =
			list_get_pointer( 
				entity_account_debit_list );

		if ( strcmp(
			entity_account_debit->full_name,
			full_name ) == 0
		&&  strcmp(
			entity_account_debit->street_address,
			street_address ) == 0
		&&  strcmp(
			entity_account_debit->account_name,
			account_name ) == 0 )
		{
			return entity_account_debit;
		}

	} while( list_next( entity_account_debit_list ) );

skip_list:

	entity_account_debit =
		print_checks_entity_account_debit_new(
			full_name,
			street_address,
			account_name );

	list_append_pointer(	entity_account_debit_list,
				entity_account_debit );

	return entity_account_debit;

} /* print_checks_get_or_set_entity_account_debit() */

PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *
		print_checks_get_entity_account_debit(
				LIST *current_liability_account_list,
				char *full_name,
				char *street_address,
				double check_amount )
{
	ACCOUNT *account;
	PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *entity_account_debit = {0};

	if ( !list_rewind( current_liability_account_list ) )
		return (PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *)0;

	do {
		account = list_get_pointer( current_liability_account_list );

		entity_account_debit =
			print_checks_get_journal_ledger_entity_account_debit(
				entity_account_debit,
				&check_amount,
				full_name,
				street_address,
				account->account_name,
				account->journal_ledger_list );

	} while( list_next( current_liability_account_list ) );

	return entity_account_debit;

} /* print_checks_get_entity_account_debit() */

PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *
		print_checks_get_journal_ledger_entity_account_debit(
			PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *
				entity_account_debit,
			double *check_amount,
			char *full_name,
			char *street_address,
			char *account_name,
			LIST *journal_ledger_list )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( journal_ledger_list ) )
		return entity_account_debit;

	do {
	} while( list_next( journal_ledger_list ) );

	return entity_account_debit;

} /* print_checks_get_journal_ledger_entity_account_debit() */

