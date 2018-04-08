/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax.c				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver tax ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include "timlib.h"
#include "appaserver_library.h"
#include "piece.h"
#include "ledger.h"
#include "tax.h"

TAX *tax_new(			char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *tax_form )
{
	TAX *t;

	if ( ! ( t = calloc( 1, sizeof( TAX ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->begin_date_string = begin_date_string;
	t->end_date_string = end_date_string;

	t.tax_input->tax_form = tax_form_new( application_name, tax_form );

	return t;
} /* tax_new() */

TAX_FORM *tax_form_new(		char *application_name,
				char *tax_form )
{
	TAX_FORM *t;

	if ( ! ( t = calloc( 1, sizeof( TAX_FORM ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->tax_form = tax_form;

	t->tax_form_line_list =
		tax_form_fetch_line_list(
			application_name,
			t->tax_form );
	return t;

} /* tax_form_new() */

TAX_FORM_LINE *tax_form_line_new(	char *tax_form,
					char *tax_form_line,
					char *tax_form_description,
					boolean itemize_accounts )
{
	TAX_FORM_LINE *t;

	if ( ! ( t = calloc( 1, sizeof( TAX_FORM_LINE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->tax_form = tax_form;
	t->tax_form_line = tax_form_line;
	t->tax_form_description = tax_form_description;
	t->itemize_accounts = itemize_accounts;

	return t;

} /* tax_form_line_new() */

LIST *tax_form_fetch_line_list(		char *application_name,
					char *tax_form )
{
	LIST *account_list;
	TAX_FORM_LINE *tax_form_line = {0};
	LIST *tax_form_line_list;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char *input_buffer_ptr;
	char tax_form_line_name[ 128 ];
	char tax_form_description[ 128 ];
	char itemize_accounts_yn[ 8 ];
	ACCOUNT *account;

	/* Fetches account->latest_ledger */
	/* ------------------------------ */
	account_list =
		ledger_get_account_list(
			application_name,
			as_of_date );

	if ( !list_length( account_list ) ) return (LIST *)0;

	sprintf( sys_string,
		 "select_tax_form_line.sh %s \"%s\"",
		 application_name,
		 tax_form );

	input_pipe = popen( sys_string, "r" );

	tax_form_line_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( timlib_strncmp(
			input_buffer,
			TAX_FORM_LINE_KEY ) == 0 )
		{
			input_buffer_ptr =
				input_buffer +
				strlen( TAX_FORM_LINE_KEY );

			piece(	tax_form_line_name,
				FOLDER_DATA_DELIMITER,
				input_buffer_ptr,
				0 );
	
			piece(	tax_form_description,
				FOLDER_DATA_DELIMITER,
				input_buffer_ptr,
				1 );
	
			piece(	itemize_accounts_yn,
				FOLDER_DATA_DELIMITER,
				input_buffer_ptr,
				2 );
	
			tax_form_line =
				tax_form_line_new(
					tax_form,
					strdup( tax_form_line_name ),
					strdup( tax_form_description ),
					(*itemize_accounts_yn == 'y' ) );

			list_append_pointer(
				tax_form_line_list,
				tax_form_line );
		}
		else
		if ( timlib_strncmp(
			input_buffer,
			TAX_FORM_LINE_ACCOUNT_KEY ) == 0 )
		{
			input_buffer_ptr =
				input_buffer +
				strlen( TAX_FORM_LINE_ACCOUNT_KEY );

			if ( ! ( account =
					ledger_account_list_seek(
						account_list,
						input_buffer_ptr
							/* account_name */ ) ) )
			{
				fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot seek account = %s\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					input_buffer_ptr );

				pclose( input_pipe );
				exit( 1 );
			}

			if ( !account->latest_ledger
			||   timlib_double_virtually_same(
				account->latest_ledger->balance,
				0.0 ) )
			{
				continue;
			}

			if ( !tax_form_line )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: tax_form_line is null for (%s).\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					input_buffer_ptr );

				pclose( input_pipe );
				exit( 1 );
			}

			tax_form_line->tax_form_line_total +=
				account->latest_ledger->balance;

			list_add_pointer_in_order(
				tax_form_line->account_list,
				account,
				ledger_balance_match_function );
		}
		else
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: invalid input key for record = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );

			pclose( input_pipe );
			exit( 1 );
		}

	} /* while( get_line() ) */

	pclose( input_pipe );

	return tax_form_line_list;

} /* tax_form_fetch_line_list() */

LIST *tax_fetch_account_list(		char *application_name,
					char *tax_form_line )
{
	static LIST *local_account_list = {0};
	LIST *return_account_list;
	ACCOUNT *account;

	if ( !local_account_list )
	{
		local_account_list =
			tax_fetch_local_account_list(
				application_name );
	}

	if ( !list_rewind( local_account_list ) ) return (LIST *)0;

	return_account_list = list_new();

	do {
		account = list_get( local_account_list );

		if ( timlib_strcmp(
			account->tax_form_line,
			tax_form_line ) == 0 )
		{
			list_append_pointer( return_account_list, account );
		}

	} while( list_next( local_account_list ) );

	return return_account_list;

} /* tax_fetch_account_list() */

LIST *tax_fetch_local_account_list(
				char *application_name )
{
	ACCOUNT *account;
	char *select;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	LIST *account_list;
	FILE *input_pipe;

	account_list = list_new();
	select = tax_account_get_select( application_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=account		",
		 application_name,
		 select );

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

		account->accumulate_debit =
			ledger_account_get_accumulate_debit(
				application_name, account->account_name );

		if ( as_of_date )
		{
			account->latest_ledger =
				ledger_get_latest_ledger(
					application_name,
					account->account_name,
					as_of_date );
		}

		list_append_pointer( account_list, account );
	}

	pclose( input_pipe );
	return account_list;

} /* tax_fetch_local_account_list() */

char *tax_account_get_select( char *application_name )
{
	char *select;

	if ( ledger_fund_attribute_exists(
			application_name ) )
	{
		select =
"account.account,fund,subclassification,hard_coded_account_key,tax_form_line";
	}
	else
	{
		select =
"account.account,null,subclassification,hard_coded_account_key,tax_form_line";
	}

	return select;

} /* tax_account_get_select() */
