/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/post_change_customer_payment.c	*/
/* --------------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "inventory.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "ledger.h"
#include "customer.h"
#include "purchase.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_customer_payment_date_time_update(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time );

void post_change_customer_payment_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time );

void post_change_customer_payment_delete(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time );

void post_change_customer_payment_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *payment_date_time;
	char *state;
	char *preupdate_payment_date_time;
	char *database_string = {0};

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s application full_name street_address sale_date_time payment_date_time state preupdate_payment_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	payment_date_time = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_payment_date_time = argv[ 7 ];

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_customer_payment_insert(
				application_name,
				full_name,
				street_address,
				sale_date_time,
				payment_date_time );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_customer_payment_update(
				application_name,
				full_name,
				street_address,
				sale_date_time,
				payment_date_time,
				preupdate_payment_date_time );
	}
	else
	if ( strcmp( state, "predelete" ) == 0 )
	/* -------------------------------------------- */
	/* Need to execute on predelete to get		*/
	/* CUSTOMER_PAYMENT.transaction_date_time. 	*/
	/* -------------------------------------------- */
	{
		post_change_customer_payment_delete(
				application_name,
				full_name,
				street_address,
				sale_date_time,
				payment_date_time );
	}

	return 0;

} /* main() */

void post_change_customer_payment_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time )
{
	CUSTOMER_SALE *customer_sale;
	CUSTOMER_PAYMENT *customer_payment;
	LIST *propagate_account_list;

	if ( !( customer_sale =
			customer_sale_new(
				application_name,
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		return;
	}

	customer_sale_update(
		customer_sale->sum_extension,
		customer_sale->database_sum_extension,
		customer_sale->sales_tax,
		customer_sale->database_sales_tax,
		customer_sale->invoice_amount,
		customer_sale->database_invoice_amount,
		customer_sale->completed_date_time,
		customer_sale->
			database_completed_date_time,
		customer_sale->shipped_date_time,
		customer_sale->database_shipped_date_time,
		customer_sale->arrived_date,
		customer_sale->database_arrived_date,
		customer_sale->total_payment,
		customer_sale->database_total_payment,
		customer_sale->amount_due,
		customer_sale->database_amount_due,
		customer_sale->transaction_date_time,
		customer_sale->
			database_transaction_date_time,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		application_name );

	if ( ! ( customer_payment =
			customer_payment_seek(
				customer_sale->payment_list,
				payment_date_time ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot seek payment_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
	}

	if ( customer_payment->transaction_date_time )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: not expecting transaction_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 customer_payment->transaction_date_time );
		exit( 1 );
	}

	customer_payment->transaction_date_time =
		customer_payment->payment_date_time;
	
	customer_payment->transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			customer_payment->transaction_date_time,
			CUSTOMER_PAYMENT_MEMO );
	
	ledger_transaction_insert(
		application_name,
		customer_payment->transaction->full_name,
		customer_payment->transaction->street_address,
		customer_payment->transaction->transaction_date_time,
		customer_payment->payment_amount /* transaction_amount */,
		customer_payment->transaction->memo,
		0 /* check_number */,
		1 /* lock_transaction */ );

	if ( ( propagate_account_list =
		customer_payment_journal_ledger_refresh(
			application_name,
			customer_sale->fund_name,
			customer_payment->transaction->full_name,
			customer_payment->transaction->street_address,
			customer_payment->
				transaction->
				transaction_date_time,
			customer_payment->payment_amount ) ) )
	{
		ledger_account_list_propagate(
			propagate_account_list,
			application_name );
	}

	customer_payment_update(
		application_name,
		full_name,
		street_address,
		sale_date_time,
		payment_date_time,
		customer_payment->transaction_date_time,
		customer_payment->database_transaction_date_time );

} /* post_change_customer_payment_insert() */

void post_change_customer_payment_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *payment_date_time )
{
	CUSTOMER_SALE *customer_sale;
	CUSTOMER_PAYMENT *customer_payment;
	LIST *propagate_account_list;

	customer_sale =
		customer_sale_new(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	if ( !customer_sale )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find customer sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( ! ( customer_payment =
			customer_payment_seek(
				customer_sale->payment_list,
				payment_date_time ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot seek payment_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
	}

	list_delete_current( customer_sale->payment_list );

	customer_sale->total_payment =
		customer_get_total_payment(
			customer_sale->payment_list );

	customer_sale->invoice_amount =
		customer_sale_get_invoice_amount(
			&customer_sale->sum_inventory_extension,
			&customer_sale->sum_fixed_service_extension,
			&customer_sale->sum_hourly_service_extension,
			&customer_sale->sum_extension,
			&customer_sale->sales_tax,
			customer_sale->shipping_revenue,
			customer_sale->inventory_sale_list,
			customer_sale->specific_inventory_sale_list,
			customer_sale->fixed_service_sale_list,
			customer_sale->hourly_service_sale_list,
			customer_sale->full_name,
			customer_sale->street_address,
			application_name );

	customer_sale->amount_due =
		CUSTOMER_GET_AMOUNT_DUE(
			customer_sale->invoice_amount,
			customer_sale->total_payment );

	customer_sale_update(
		customer_sale->sum_extension,
		customer_sale->database_sum_extension,
		customer_sale->sales_tax,
		customer_sale->database_sales_tax,
		customer_sale->invoice_amount,
		customer_sale->database_invoice_amount,
		customer_sale->completed_date_time,
		customer_sale->
			database_completed_date_time,
		customer_sale->shipped_date_time,
		customer_sale->database_shipped_date_time,
		customer_sale->arrived_date,
		customer_sale->database_arrived_date,
		customer_sale->total_payment,
		customer_sale->database_total_payment,
		customer_sale->amount_due,
		customer_sale->database_amount_due,
		customer_sale->transaction_date_time,
		customer_sale->
			database_transaction_date_time,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		application_name );

	if ( !customer_payment->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot have empty transaction for payment_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
	}

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			customer_payment->transaction->full_name,
			customer_payment->transaction->street_address,
			customer_payment->
				transaction->
				transaction_date_time );

	/* ---------------------------------------------------- */
	/* customer_payment_journal_ledger_refresh() executes	*/
	/* ledger_delete(	application_name,		*/
	/*			LEDGER_FOLDER_NAME,		*/
	/* ---------------------------------------------------- */
	if ( ( propagate_account_list =
		customer_payment_journal_ledger_refresh(
			application_name,
			customer_sale->fund_name,
			customer_payment->transaction->full_name,
			customer_payment->transaction->street_address,
			customer_payment->
				transaction->
				transaction_date_time,
			0.0 /* payment_amount */ ) ) )
	{
		ledger_account_list_propagate(
			propagate_account_list,
			application_name );
	}

} /* post_change_customer_payment_delete() */

void post_change_customer_payment_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time )
{
	enum preupdate_change_state payment_date_time_change_state;
	CUSTOMER_SALE *customer_sale;
	CUSTOMER_PAYMENT *customer_payment;

	customer_sale =
		customer_sale_new(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	if ( !customer_sale )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find customer sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( ! ( customer_payment =
			customer_payment_seek(
				customer_sale->payment_list,
				payment_date_time ) ) )
	{
		post_change_customer_payment_date_time_update(
			application_name,
			customer_sale->fund_name,
			full_name,
			street_address,
			sale_date_time,
			"0000-00-00 00:00:00" /* payment_date_time */,
			preupdate_payment_date_time );

		return;
	}

	if ( !customer_payment->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot empty transaction for payment_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
	}

	payment_date_time_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_payment_date_time,
			customer_payment->transaction_date_time
				/* postupdate_data */,
			"preupdate_payment_date_time" );

	if (	payment_date_time_change_state ==
		from_something_to_something_else )
	{
		post_change_customer_payment_date_time_update(
			application_name,
			customer_sale->fund_name,
			full_name,
			street_address,
			sale_date_time,
			payment_date_time,
			preupdate_payment_date_time );
	}

} /* post_change_customer_payment_update() */

void post_change_customer_payment_date_time_update(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time )
{
	char *checking_account;
	char *account_receivable_account;
	char *propagate_transaction_date_time;

	ledger_transaction_generic_update(
		application_name,
		full_name,
		street_address,
		preupdate_payment_date_time,
		"transaction_date_time",
		payment_date_time );

	ledger_journal_generic_update(
		application_name,
		full_name,
		street_address,
		preupdate_payment_date_time,
		"transaction_date_time",
		payment_date_time );

	if ( strcmp(	preupdate_payment_date_time,
			payment_date_time ) < 0 )
	{
		propagate_transaction_date_time =
			preupdate_payment_date_time;
	}
	else
	{
		propagate_transaction_date_time =
			payment_date_time;
	}

	ledger_get_customer_payment_account_names(
		&checking_account,
		&account_receivable_account,
		application_name,
		fund_name );

	ledger_propagate(	application_name,
				propagate_transaction_date_time,
				checking_account );

	ledger_propagate(	application_name,
				propagate_transaction_date_time,
				account_receivable_account );

	customer_payment_update(
		application_name,
		full_name,
		street_address,
		sale_date_time,
		payment_date_time,
		payment_date_time
			/* transaction_date_time */,
		preupdate_payment_date_time
			/* database_transaction_date_time */ );

} /* post_change_customer_payment_date_time_update() */

