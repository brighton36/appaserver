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
				char *fund_name,
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

	t->tax_input.tax_form = tax_form_new( application_name, tax_form );

	t->tax_input.cash_transaction_list =
		tax_fetch_cash_transaction_list(
			application_name,
			fund_name,
			begin_date_string,
			end_date_string );

	return t;

} /* tax_new() */

TAX_FORM_LINE_ACCOUNT *tax_form_line_account_new(
					char *account_name )
{
	TAX_FORM_LINE_ACCOUNT *t;

	if ( ! ( t = calloc( 1, sizeof( TAX_FORM_LINE_ACCOUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->account_name = account_name;

	return t;

} /* tax_form_line_account_new() */
	
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
	TAX_FORM_LINE *tax_form_line = {0};
	LIST *tax_form_line_list;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char *input_buffer_ptr;
	char tax_form_line_name[ 128 ];
	char tax_form_description[ 128 ];
	char itemize_accounts_yn[ 8 ];
	TAX_FORM_LINE_ACCOUNT *account;

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

			account =
				tax_form_line_account_new(
					strdup( input_buffer_ptr
						/* account_name */ ) );

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

			account->accumulate_debit =
				ledger_account_get_accumulate_debit(
					application_name,
					account->account_name );

			if ( !tax_form_line->account_list )
			{
				tax_form_line->account_list =
					list_new();
			}

			list_append_pointer(
				tax_form_line->account_list,
				account );
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

LIST *tax_fetch_cash_transaction_list(
			char *application_name,
			char *fund_name,
			char *begin_date_string,
			char *end_date_string )
{
	char *checking_account;
	char where_clause[ 1024 ];

	checking_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			LEDGER_CASH_KEY,
			0 /* not warning_only */ );

	sprintf( where_clause,
		 "transaction_date_time >= '%s' and			"
		 "transaction_date_time <= '%s 23:59:58' and		"
		 "exists (select 1					"
		 "	  from journal_ledger				"
		 "	  where transaction.full_name =			"
		 "		journal_ledger.full_name and		"
		 "	        transaction.street_address =		"
		 "		journal_ledger.street_address and	"
		 "	        transaction.transaction_date_time =	"
		 "		journal_ledger.transaction_date_time and"
		 "		journal_ledger.account = '%s')		",
		 begin_date_string,
		 end_date_string,
		 checking_account );

	return ledger_fetch_transaction_list(
			application_name,
			where_clause );

} /* tax_fetch_cash_transaction_list() */

