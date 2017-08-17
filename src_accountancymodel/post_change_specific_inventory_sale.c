/* ---------------------------------------------------------------	*/
/* src_accountancymodel/post_change_specific_inventory_sale.c		*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "entity.h"
#include "inventory.h"
#include "ledger.h"
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_inventory_sale_delete(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );

void post_change_inventory_sale_insert_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				char *serial_number );

/*
void post_change_inventory_sale_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				char *serial_number );
*/

int main( int argc, char **argv )
{
	char *database_string = {0};
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *inventory_name;
	char *serial_number;
	char *state;

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s application full_name street_address sale_date_time inventory_name serial_number state\n",
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
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	inventory_name = argv[ 5 ];
	serial_number = argv[ 6 ];
	state = argv[ 7 ];

	/* If change full_name or street address only. */
	/* --------------------------------------------- */
	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "delete" ) == 0 )
	{
		post_change_inventory_sale_delete(
			application_name,
			full_name,
			street_address,
			sale_date_time );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_inventory_sale_insert_update(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			inventory_name,
			serial_number );
	}
	else
	{
		post_change_inventory_sale_insert_update(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			inventory_name,
			serial_number );
	}

	return 0;

} /* main() */

void post_change_inventory_sale_insert_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name,
			char *serial_number )
{
	CUSTOMER_SALE *customer_sale;
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;

	if ( ! (  customer_sale =
			customer_sale_new(
				application_name,
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: customer_sale_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	if ( ! ( specific_inventory_sale =
			customer_specific_inventory_sale_seek(
				customer_sale->specific_inventory_sale_list,
				inventory_name,
				serial_number ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 serial_number );
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

	customer_specific_inventory_update(
		application_name,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		specific_inventory_sale->inventory_name,
		specific_inventory_sale->serial_number,
		specific_inventory_sale->extension,
		specific_inventory_sale->database_extension );

	if ( customer_sale->transaction_date_time )
	{
		customer_sale->transaction =
			ledger_customer_sale_build_transaction(
				application_name,
				customer_sale->transaction->full_name,
				customer_sale->transaction->street_address,
				customer_sale->transaction->
					transaction_date_time,
				customer_sale->transaction->memo,
				customer_sale->inventory_sale_list,
				customer_sale->sum_inventory_extension,
				specific_inventory_sale->extension,
				customer_sale->sum_fixed_service_extension,
				customer_sale->sum_hourly_service_extension,
				customer_sale->sales_tax,
				customer_sale->shipping_revenue,
				customer_sale->invoice_amount,
				customer_sale->fund_name );

		ledger_transaction_refresh(
			application_name,
			customer_sale->full_name,
			customer_sale->street_address,
			customer_sale->transaction_date_time,
			customer_sale->transaction->transaction_amount,
			customer_sale->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			customer_sale->transaction->journal_ledger_list );
	}

} /* post_change_inventory_sale_insert_update() */

void post_change_inventory_sale_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	CUSTOMER_SALE *customer_sale;

	if ( ! (  customer_sale =
			customer_sale_new(
				application_name,
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: customer_sale_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	/* ---------------------------- */
	/* Expect to change:		*/
	/* sum_extension,		*/
	/* sum_inventory_extension,	*/
	/* invoice_amount,		*/
	/* amount_due			*/
	/* ---------------------------- */

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

	if ( customer_sale->transaction_date_time )
	{
		customer_sale->transaction =
			ledger_customer_sale_build_transaction(
				application_name,
				customer_sale->transaction->full_name,
				customer_sale->transaction->street_address,
				customer_sale->transaction->
					transaction_date_time,
				customer_sale->transaction->memo,
				customer_sale->inventory_sale_list,
				customer_sale->sum_inventory_extension,
				0.0 /* specific_inventory_sale_extension */,
				customer_sale->sum_fixed_service_extension,
				customer_sale->sum_hourly_service_extension,
				customer_sale->sales_tax,
				customer_sale->shipping_revenue,
				customer_sale->invoice_amount,
				customer_sale->fund_name );

		ledger_transaction_refresh(
			application_name,
			customer_sale->full_name,
			customer_sale->street_address,
			customer_sale->transaction_date_time,
			customer_sale->transaction->transaction_amount,
			customer_sale->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			customer_sale->transaction->journal_ledger_list );
	}

} /* post_change_inventory_sale_delete() */

