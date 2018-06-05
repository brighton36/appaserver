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
#include "date.h"
#include "tax.h"

TAX *tax_new(			char *application_name,
				char *fund_name,
				char *begin_date_string,
				char *end_date_string,
				char *tax_form )
{
	TAX *t;
	char *checking_account;

	checking_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			LEDGER_CASH_KEY,
			0 /* not warning_only */ );

#ifdef NOT_DEFINED
	char *depreciation_account;
	depreciation_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			LEDGER_ACCUMULATED_KEY,
			0 /* not warning_only */ );
#endif

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

	if ( !t->tax_input.tax_form )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: tax_form_new() returned null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_length( t->tax_input.tax_form->tax_form_line_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty TAX_FORM_LINE_ACCOUNT table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->tax_input.cash_transaction_list =
		tax_fetch_account_transaction_list(
			application_name,
			begin_date_string,
			end_date_string,
			checking_account );

	if ( !list_length( t->tax_input.cash_transaction_list ) )
	{
		return (TAX *)0;
	}

/*
	t->tax_input.depreciation_transaction_list =
		tax_fetch_account_transaction_list(
			application_name,
			begin_date_string,
			end_date_string,
			depreciation_account );
*/

	t->tax_process.unaccounted_journal_ledger_list = list_new();

	/* ------------------------------------------------------------ */
	/* Receives t->tax_input.tax_form->tax_form_line_list		*/
	/* with tax_form_line_account_list->journal_ledger_list built.	*/
	/* ------------------------------------------------------------ */
	t->tax_process.tax_form_line_list =
		tax_process_set_journal_ledger_list(
			t->tax_process.unaccounted_journal_ledger_list,
			t->tax_input.tax_form->tax_form_line_list,
			t->tax_input.cash_transaction_list,
			checking_account );

/*
	t->tax_process.tax_form_line_list =
		tax_process_set_journal_ledger_list(
			t->tax_process.unaccounted_journal_ledger_list,
			t->tax_input.tax_form->tax_form_line_list,
			t->tax_input.depreciation_transaction_list,
			depreciation_account );
*/

	if ( !t->tax_process.tax_form_line_list )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: tax_process_get_tax_form_line_list() returned null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_length( t->tax_process.tax_form_line_list ) )
	{
		return (TAX *)0;
	}

	t->tax_process.tax_form = t->tax_input.tax_form->tax_form;

	/* Also accumulates tax_form_account_total */
	/* --------------------------------------- */
	tax_process_accumulate_tax_form_line_total(
		t->tax_process.tax_form_line_list );

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

TAX_FORM_LINE *tax_form_line_new(	char *tax_form_line,
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

			if ( !tax_form_line->tax_form_line_account_list )
			{
				tax_form_line->tax_form_line_account_list =
					list_new();
			}

			list_append_pointer(
				tax_form_line->tax_form_line_account_list,
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

LIST *tax_fetch_account_transaction_list(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *account_name )
{
	char where_clause[ 1024 ];

	sprintf( where_clause,
		 "transaction_date_time >= '%s' and			"
		 "transaction_date_time <= '%s 23:59:59' and		"
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
		 account_name );

	return ledger_fetch_transaction_list(
			application_name,
			where_clause );

} /* tax_fetch_account_transaction_list() */

TAX_FORM_LINE_ACCOUNT *tax_form_line_account_seek(
				LIST *tax_form_line_list,
				char *account_name )
{
	TAX_FORM_LINE *tax_form_line;
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;
	LIST *tax_form_line_account_list;

	if ( !list_rewind( tax_form_line_list ) )
		return (TAX_FORM_LINE_ACCOUNT *)0;

	do {
		tax_form_line =
			list_get_pointer(
				tax_form_line_list );

		tax_form_line_account_list =
			tax_form_line->
				tax_form_line_account_list;

		if ( !list_rewind( tax_form_line_account_list ) )
			continue;

		do {
			tax_form_line_account =
				list_get_pointer(
					tax_form_line_account_list );

			if ( timlib_strcmp( tax_form_line_account->
						account_name,
					    account_name ) == 0 )
			{
				return tax_form_line_account;
			}
		} while( list_next( tax_form_line_account_list ) );

	} while( list_next( tax_form_line_list ) );

	return (TAX_FORM_LINE_ACCOUNT *)0;

} /* tax_form_line_account_seek() */

LIST *tax_process_set_journal_ledger_list(
				LIST *unaccounted_journal_ledger_list,
				/* ------------- */
				/* Returns this. */
				/* ------------- */
				LIST *tax_form_line_list,
				LIST *transaction_list,
				char *account_name )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( !list_rewind( transaction_list ) )
		return tax_form_line_list;

	do {
		transaction = list_get_pointer( transaction_list );

		if ( !list_rewind( transaction->journal_ledger_list ) )
			continue;

		do {
			journal_ledger =
				list_get_pointer(
					transaction->journal_ledger_list );

			/* ------------------------------------ */
			/* Ignore the cash and accumulated	*/
			/* depreciation entries. We want the	*/
			/* other ones.				*/
			/* ------------------------------------ */
			if ( timlib_strcmp(
				journal_ledger->account_name,
				account_name ) == 0 )
			{
				continue;
			}

			tax_form_line_account =
				tax_form_line_account_seek(
					tax_form_line_list,
					journal_ledger->account_name );

			if ( !tax_form_line_account
			&&   unaccounted_journal_ledger_list )
			{
				list_append_pointer(
					unaccounted_journal_ledger_list,
					journal_ledger );
				continue;
			}

			if ( !tax_form_line_account->
				journal_ledger_list )
			{
				tax_form_line_account->
					journal_ledger_list =
						list_new();
			}

			journal_ledger->rental_property_street_address =
				transaction->rental_property_street_address;

			list_append_pointer(
				tax_form_line_account->
					journal_ledger_list,
				journal_ledger );

		} while( list_next( transaction->journal_ledger_list ) );

	} while( list_next( transaction_list ) );

	return tax_form_line_list;

} /* tax_process_set_journal_ledger_list() */

void tax_process_accumulate_tax_form_line_total(
			LIST *tax_form_line_list )
{
	TAX_FORM_LINE *tax_form_line;
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;
	JOURNAL_LEDGER *ledger;
	LIST *tax_form_line_account_list;
	LIST *journal_ledger_list;
	double amount;

	if ( !list_rewind( tax_form_line_list ) ) return;

	do {
		tax_form_line =
			list_get_pointer(
				tax_form_line_list );

		tax_form_line_account_list =
			tax_form_line->
				tax_form_line_account_list;

		if ( !list_rewind( tax_form_line_account_list ) ) continue;

		do {
			tax_form_line_account =
				list_get_pointer(
					tax_form_line_account_list );

			journal_ledger_list =
				tax_form_line_account->
					journal_ledger_list;

			if ( !list_rewind( journal_ledger_list ) ) continue;

			do {
				ledger =
					list_get_pointer(
						journal_ledger_list );

				amount =
					ledger_debit_credit_get_amount(
						ledger->debit_amount,
						ledger->credit_amount,
						1 - tax_form_line_account->
							accumulate_debit );

				tax_form_line->
					tax_form_line_total +=
						amount;

				tax_form_line_account->
					tax_form_account_total +=
						amount;

			} while( list_next( journal_ledger_list ) );

		} while( list_next( tax_form_line_account_list ) );

	} while( list_next( tax_form_line_list ) );

} /* tax_process_accumulate_tax_form_line_total() */

LIST *tax_get_rental_property_string_list(	char *application_name,
						char *begin_date_string,
						char *end_date_string )
{
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *select;

	select = "rental_property_street_address";

	sprintf( where,
		 "( property_acquired_date is null		"
		 "  or property_acquired_date >= '%s' ) and	"
		 "( property_disposal_date is null		"
		 "  or property_disposal_date <= '%s' )		",
		 begin_date_string,
		 end_date_string );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=rental_property		"
		 "			where=\"%s\"			"
		 "			order=property_acquired_date	",
		 application_name,
		 select,
		 where );

	return pipe2list( sys_string );

} /* tax_get_rental_property_string_list() */

TAX_OUTPUT_RENTAL_PROPERTY *tax_output_rental_property_new(
				char *rental_property_street_address )
{
	TAX_OUTPUT_RENTAL_PROPERTY *t;

	if ( ! ( t = calloc( 1, sizeof ( TAX_OUTPUT_RENTAL_PROPERTY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->rental_property_street_address = rental_property_street_address;

	return t;

} /* tax_output_rental_property_new() */

TAX_FORM_LINE_RENTAL *tax_form_line_rental_new(
				char *tax_form_line,
				char *tax_form_description,
				LIST *tax_form_line_account_list )
{
	TAX_FORM_LINE_RENTAL *t;

	if ( ! ( t = calloc( 1, sizeof ( TAX_FORM_LINE_RENTAL ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->tax_form_line = tax_form_line;
	t->tax_form_description = tax_form_description;
	t->tax_form_line_account_list = tax_form_line_account_list;

	return t;

} /* tax_form_line_rental_new() */

LIST *tax_form_line_rental_get_empty_property_list(
			LIST *rental_property_street_address_list )
{
	TAX_OUTPUT_RENTAL_PROPERTY *tax_output_rental_property;
	char *rental_property_street_address;
	LIST *rental_property_list;

	if ( !list_rewind( rental_property_street_address_list ) )
		return (LIST *)0;

	rental_property_list = list_new();

	do {
		rental_property_street_address =
			list_get_pointer(
				rental_property_street_address_list );

		tax_output_rental_property =
			tax_output_rental_property_new(
				rental_property_street_address );

		list_append_pointer(
			rental_property_list,
			tax_output_rental_property );

	} while( list_next( rental_property_street_address_list ) );

	return rental_property_list;

} /* tax_form_line_rental_get_empty_property_list() */

LIST *tax_get_tax_form_line_rental_list(
			LIST *tax_form_line_list,
			LIST *rental_property_street_address_list )
{
	TAX_FORM_LINE *tax_form_line;
	TAX_FORM_LINE_RENTAL *tax_form_line_rental;
	LIST *tax_form_line_rental_list;

	if ( !list_rewind( tax_form_line_list ) ) return (LIST *)0;

	tax_form_line_rental_list = list_new();

	do {
		tax_form_line = list_get( tax_form_line_list );

		if ( !list_length(
			tax_form_line->
				tax_form_line_account_list ) )
		{
			continue;
		}

		tax_form_line_rental =
			tax_form_line_rental_new(
				tax_form_line->tax_form_line,
				tax_form_line->tax_form_description,
				tax_form_line->tax_form_line_account_list );

		list_append_pointer(
			tax_form_line_rental_list,
			tax_form_line_rental );

		tax_form_line_rental->rental_property_list =
			tax_form_line_rental_get_empty_property_list(
				rental_property_street_address_list );

	} while( list_next( tax_form_line_list ) );

	return tax_form_line_rental_list;

} /* tax_get_tax_form_line_rental_list() */

void tax_line_rental_list_accumulate_line_total(
				LIST *tax_form_line_rental_list )
{
	TAX_FORM_LINE_RENTAL *tax_form_line_rental;

	if ( !list_rewind( tax_form_line_rental_list ) ) return;

	do {
		tax_form_line_rental =
			list_get_pointer(
				tax_form_line_rental_list );

		tax_rental_property_list_accumulate_line_total(
			tax_form_line_rental->rental_property_list,
			tax_form_line_rental->tax_form_line_account_list );

	} while( list_next( tax_form_line_rental_list ) );

} /* tax_line_rental_list_accumulate_line_total() */

void tax_form_line_address_rental_property_list_set(
			LIST *rental_property_list,
			double debit_amount,
			double credit_amount,
			char *rental_property_street_address,
			boolean accumulate_debit )
{
	TAX_OUTPUT_RENTAL_PROPERTY *tax_output_rental_property;

	if ( !list_rewind( rental_property_list ) ) return;

	do {
		tax_output_rental_property =
			list_get_pointer(
				rental_property_list );

		if ( timlib_strcmp( 
				rental_property_street_address,
				tax_output_rental_property->
					rental_property_street_address ) == 0 )
		{
			tax_output_rental_property->
				tax_form_line_total +=
					ledger_debit_credit_get_amount(
						debit_amount,
						credit_amount,
						accumulate_debit );
			return;
		}

	} while( list_next( rental_property_list ) );

	fprintf( stderr,
		 "ERROR in %s/%s()/%d: cannot find rental property = %s.\n",
		 __FILE__,
		 __FUNCTION__,
		 __LINE__,
		 rental_property_street_address );

	exit( 1 );

} /* tax_form_line_address_rental_property_list_set() */

void tax_form_line_distribute_rental_property_list_set(
			LIST *rental_property_list,
			double debit_amount,
			double credit_amount,
			boolean accumulate_debit,
			double denominator )
{
	TAX_OUTPUT_RENTAL_PROPERTY *tax_output_rental_property;

	if ( !list_rewind( rental_property_list ) ) return;

	do {
		tax_output_rental_property =
			list_get_pointer(
				rental_property_list );

		tax_output_rental_property->
			tax_form_line_total +=
				( ledger_debit_credit_get_amount(
					debit_amount,
					credit_amount,
					accumulate_debit ) /
				  denominator );

	} while( list_next( rental_property_list ) );

} /* tax_form_line_distribute_rental_property_list_set() */

void tax_rental_journal_ledger_list_accumulate_line_total(
				LIST *rental_property_list,
				LIST *journal_ledger_list,
				boolean accumulate_debit )
{
	JOURNAL_LEDGER *ledger;
	double denominator;

	if ( !list_rewind( journal_ledger_list ) ) return;

	if ( !list_length( rental_property_list ) ) return;

	denominator =
		(double)
		list_length(
			rental_property_list );

	do {
		ledger = list_get_pointer( journal_ledger_list );

		if ( ledger->rental_property_street_address
		&&   *ledger->rental_property_street_address )
		{
			tax_form_line_address_rental_property_list_set(
				rental_property_list,
				ledger->debit_amount,
				ledger->credit_amount,
				ledger->rental_property_street_address,
				accumulate_debit );
		}
		else
		{
			tax_form_line_distribute_rental_property_list_set(
				rental_property_list,
				ledger->debit_amount,
				ledger->credit_amount,
				accumulate_debit,
				denominator );
		}

	} while( list_next( journal_ledger_list ) );

} /* tax_rental_journal_ledger_list_accumulate_line_total() */

void tax_rental_property_list_accumulate_line_total(
				LIST *rental_property_list,
				LIST *tax_form_line_account_list )
{
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( !list_rewind( tax_form_line_account_list ) ) return;

	do {
		tax_form_line_account =
			list_get_pointer(
				tax_form_line_account_list );

		tax_rental_journal_ledger_list_accumulate_line_total(
				rental_property_list,
				tax_form_line_account->journal_ledger_list,
				tax_form_line_account->accumulate_debit );

	} while( list_next( tax_form_line_account_list ) );

} /* tax_rental_property_list_accumulate_line_total() */

boolean tax_calculate_real_estate_parse(
				int *service_month,
				int *service_year,
				int *sale_month,
				int *sale_year,
				char *service_placement_date_string,
				char *sale_date_string )
{
	DATE *service_placement_date;
	DATE *sale_date = {0};

	*service_month = 0;
	*service_year = 0;
	*sale_month = 0;
	*sale_year = 0;

	if ( ! ( service_placement_date =
			date_yyyy_mm_dd_new(
				service_placement_date_string,
				date_get_utc_offset() ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid service_placement_date = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 service_placement_date_string );

		return 0;
	}

	*service_month = date_get_month( service_placement_date );
	*service_year = date_get_year( service_placement_date );
	date_free( service_placement_date );

	if ( sale_date_string && *sale_date_string )
	{
		if ( ! ( sale_date =
				date_yyyy_mm_dd_new(
					sale_date_string,
					date_get_utc_offset() ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid sale_date = (%s)\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	sale_date_string );

			return 0;
		}

		*sale_month = date_get_month( sale_date );
		*sale_year = date_get_year( sale_date );
		date_free( sale_date );
	}

	return 1;

} /* tax_calculate_real_estate_parse() */

double tax_calculate_real_estate_recovery(
				double cost_basis,
				char *service_placement_date_string,
				char *sale_date_string,
				double recovery_period_years,
				int current_year )
{
	double recovery_amount = 0.0;
	int service_month = 0;
	int service_year = 0;
	int sale_month = 0;
	int sale_year = 0;
	int recovery_period_months;
	int recovery_period_semi_months;
	double percent_per_year;
	double percent_per_semi_month;
	unsigned int recovery_months_as_of_december;
	unsigned int recovery_months_extra_year;
	double applicable_rate = 0.0;

	if ( !tax_calculate_real_estate_parse(
				&service_month,
				&service_year,
				&sale_month,
				&sale_year,
				service_placement_date_string,
				sale_date_string ) )
	{
		return 0.0;
	}

	recovery_period_months = (int)(recovery_period_years * 12.0);
	recovery_period_semi_months = recovery_period_months * 2;
	percent_per_year = 1.0 / recovery_period_years;
	percent_per_semi_month = 1.0 / (double)recovery_period_semi_months;

	recovery_months_as_of_december =
		( current_year * 12 - service_year * 12 ) +
		( ( 12 - service_month ) + 1 );

	recovery_months_extra_year =
		recovery_period_months + 12;

	if ( sale_year == current_year
	&&   recovery_months_as_of_december <= recovery_period_months )
	{
		applicable_rate =
			(double)( ( ( sale_month - 1 ) * 2 ) + 1 ) *
			percent_per_semi_month;
	}
	else
	if ( sale_year )
	{
		return 0.0;
	}
	else
	if ( recovery_months_as_of_december <= 12 )
	{
		applicable_rate =
			(double)( ( ( 12 - service_month ) * 2 ) + 1 ) *
			percent_per_semi_month;
	}
	else
	if ( recovery_months_as_of_december > 12
	&&   recovery_months_as_of_december <= recovery_period_months )
	{
		applicable_rate = percent_per_year;
	}
	else
	if ( recovery_months_as_of_december > recovery_period_months
	&&   recovery_months_as_of_december <= recovery_months_extra_year )
	{
		if ( timlib_double_is_integer( recovery_period_years ) )
		{
			applicable_rate =
				(double)
				( ( service_month * 2 ) - 1 ) *
				percent_per_semi_month;
		}
		else
		{
			if ( service_month <= 6 )
			{
				applicable_rate =
				(double)( 12 + ( service_month * 2 ) - 1 ) *
				percent_per_semi_month;
			}
			else
			{
				applicable_rate =
				(double)( ( service_month - 6 ) * 2 - 1 ) *
				percent_per_semi_month;
			}
		}
	}

	recovery_amount = cost_basis * applicable_rate;

	return recovery_amount;

} /* tax_calculate_real_estate_recovery() */

