/* ---------------------------------------------------------------	*/
/* src_accountancymodel/post_change_fixed_asset_purchase.c		*/
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
#include "column.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "inventory.h"
#include "entity.h"
#include "ledger.h"
#include "purchase.h"
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
/*
void post_change_fixed_asset_purchase_transaction_update(
			PURCHASE_ORDER *purchase_order,
			char *application_name,
			char *asset_name,
			char *serial_number,
			char *preupdate_asset_name,
			char *preupdate_serial_number );
*/

void post_change_fixed_asset_purchase_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

void post_change_fixed_asset_purchase_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

void post_change_fixed_asset_purchase_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *preupdate_asset_name,
			char *preupdate_serial_number,
			char *preupdate_declining_balance_n,
			char *preupdate_depreciation_method,
			char *preupdate_extension );

int main( int argc, char **argv )
{
	char *database_string = {0};
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *asset_name;
	char *serial_number;
	char *state;
	char *preupdate_asset_name;
	char *preupdate_serial_number;
	char *preupdate_extension;
	char *preupdate_declining_balance_n;
	char *preupdate_depreciation_method;

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( argc != 13 )
	{
		fprintf( stderr,
"Usage: %s application full_name street_address purchase_date_time asset_name serial_number state preupdate_asset_name preupdate_serial_number preupdate_extension preupdate_declining_balance_n preupdate_depreciation_method\n",
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
	purchase_date_time = argv[ 4 ];
	asset_name = argv[ 5 ];
	serial_number = argv[ 6 ];
	state = argv[ 7 ];
	preupdate_asset_name = argv[ 8 ];
	preupdate_serial_number = argv[ 9 ];
	preupdate_extension = argv[ 10 ];
	preupdate_declining_balance_n = argv[ 11 ];
	preupdate_depreciation_method = argv[ 12 ];

	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "delete" ) == 0 )
	{
		post_change_fixed_asset_purchase_delete(
			application_name,
			full_name,
			street_address,
			purchase_date_time );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_fixed_asset_purchase_insert(
			application_name,
			full_name,
			street_address,
			purchase_date_time );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_fixed_asset_purchase_update(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			serial_number,
			preupdate_asset_name,
			preupdate_serial_number,
			preupdate_declining_balance_n,
			preupdate_depreciation_method,
			preupdate_extension );
	}

	return 0;

} /* main() */

void post_change_fixed_asset_purchase_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	PURCHASE_ORDER *purchase_order;

	purchase_order =
		purchase_order_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time );

	if ( !purchase_order )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

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

	if ( !purchase_order->transaction ) return;

	ledger_transaction_amount_update(
		application_name,
		purchase_order->transaction->full_name,
		purchase_order->transaction->street_address,
		purchase_order->transaction->transaction_date_time,
		purchase_order->purchase_amount /* transaction_amount */,
		0.0 /* database_transaction_amount */ );

	purchase_order->propagate_account_list =
		purchase_order_journal_ledger_refresh(
			application_name,
			purchase_order->fund_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->sum_specific_inventory_unit_cost,
			purchase_order->sum_supply_extension,
			purchase_order->sum_service_extension,
			purchase_order->sales_tax,
			purchase_order->freight_in,
			purchase_order->purchase_amount,
			purchase_order->inventory_purchase_list,
			purchase_order->supply_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->purchase_asset_account_list );

	ledger_account_list_propagate(
		purchase_order->propagate_account_list,
		application_name );

} /* post_change_fixed_asset_purchase_insert() */

void post_change_fixed_asset_purchase_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	PURCHASE_ORDER *purchase_order;

	purchase_order =
		purchase_order_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time );

	if ( !purchase_order )
	{
		fprintf( stderr,
			 "ERROR In %s/%s()/%d: cannot find purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	purchase_order->purchase_amount =
		purchase_order_get_purchase_amount(
			&purchase_order->sum_inventory_extension,
			&purchase_order->sum_specific_inventory_unit_cost,
			&purchase_order->sum_supply_extension,
			&purchase_order->sum_service_extension,
			&purchase_order->sum_fixed_asset_extension,
			&purchase_order->sum_prepaid_asset_extension,
			&purchase_order->sum_extension,
			purchase_order->inventory_purchase_list,
			purchase_order->specific_inventory_purchase_list,
			purchase_order->supply_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->fixed_asset_purchase_list,
			purchase_order->prepaid_asset_purchase_list,
			purchase_order->sales_tax,
			purchase_order->freight_in );

	purchase_order->amount_due =
		PURCHASE_GET_AMOUNT_DUE(
			purchase_order->purchase_amount,
			purchase_order->sum_payment_amount );

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
			purchase_order->
				database_transaction_date_time,
			purchase_order->arrived_date_time,
			purchase_order->
				database_arrived_date_time,
			purchase_order->shipped_date,
			purchase_order->database_shipped_date );

	if ( !purchase_order->transaction ) return;

	purchase_order->propagate_account_list =
		purchase_order_journal_ledger_refresh(
			application_name,
			purchase_order->fund_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->sum_specific_inventory_unit_cost,
			purchase_order->sum_supply_extension,
			purchase_order->sum_service_extension,
			purchase_order->sales_tax,
			purchase_order->freight_in,
			purchase_order->purchase_amount,
			purchase_order->inventory_purchase_list,
			purchase_order->supply_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->purchase_asset_account_list );

	ledger_account_list_propagate(
		purchase_order->propagate_account_list,
		application_name );

} /* post_change_fixed_asset_purchase_delete() */

void post_change_fixed_asset_purchase_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *preupdate_asset_name,
			char *preupdate_serial_number,
			char *preupdate_declining_balance_n,
			char *preupdate_depreciation_method,
			char *preupdate_extension )
{
	PURCHASE_ORDER *purchase_order;
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	enum preupdate_change_state declining_balance_n_change_state;
	enum preupdate_change_state depreciation_method_change_state;
	enum preupdate_change_state asset_name_change_state;
	enum preupdate_change_state serial_number_change_state;
	enum preupdate_change_state extension_change_state;

	purchase_order =
		purchase_order_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time );

	if ( !purchase_order )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !( purchase_fixed_asset =
		purchase_fixed_asset_list_seek(
			purchase_order->fixed_asset_purchase_list,
			asset_name,
			serial_number ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot seek fixed_asset_purchase_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

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

	depreciation_method_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_depreciation_method,
			(purchase_fixed_asset->depreciation_method)
				? purchase_fixed_asset->depreciation_method
				: ""
				/* postupdate_data */,
			"preupdate_depreciation_method" );

	declining_balance_n_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_declining_balance_n,
			(char *)0 /* postupdate_data */,
			"preupdate_declining_balance_n" );

	asset_name_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_asset_name,
			asset_name /* postupdate_data */,
			"preupdate_asset_name" );

	serial_number_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_serial_number,
			serial_number /* postupdate_data */,
			"preupdate_serial_number" );

	extension_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_extension,
			(char *)0 /* postupdate_data */,
			"preupdate_extension" );

	if ( serial_number_change_state )
	{
		/* do nothing */
	}


	if (	declining_balance_n_change_state ==
		from_something_to_something_else )
	{
		purchase_depreciation_update_and_transaction_propagate(
			purchase_fixed_asset,
			purchase_order->arrived_date_time,
			application_name,
			purchase_order->fund_name );
	}

	if (	depreciation_method_change_state ==
		from_something_to_something_else
	||  	depreciation_method_change_state ==
		from_something_to_null )
	{
		purchase_depreciation_update_and_transaction_propagate(
			purchase_fixed_asset,
			purchase_order->arrived_date_time,
			application_name,
			purchase_order->fund_name );
	}

	if ( !purchase_order->transaction ) return;

	if (	asset_name_change_state ==
		from_something_to_something_else )
	{
		char *preupdate_account_name;

		ledger_propagate(
			application_name,
			purchase_order->transaction_date_time,
			purchase_fixed_asset->account_name );

		if ( ! ( preupdate_account_name =
				purchase_fixed_asset_get_account_name(
					application_name,
					preupdate_asset_name ) ) )
		{
			fprintf( stderr,
"Warning in %s/%s()/%d: cannot get account_name for asset_name = (%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 preupdate_asset_name );
			return;
		}

		if ( strcmp(	preupdate_account_name,
				purchase_fixed_asset->account_name ) != 0 )
		{
			ledger_propagate(
				application_name,
				purchase_order->transaction_date_time,
				preupdate_account_name );
		}
	}

	if (	extension_change_state ==
		from_something_to_something_else )
	{
		ledger_transaction_amount_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			purchase_order->purchase_amount
				/* transaction_amount */,
			0.0 /* database_transaction_amount */ );

		purchase_order->propagate_account_list =
			purchase_order_journal_ledger_refresh(
				application_name,
				purchase_order->fund_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->
					sum_specific_inventory_unit_cost,
				purchase_order->sum_supply_extension,
				purchase_order->sum_service_extension,
				purchase_order->sales_tax,
				purchase_order->freight_in,
				purchase_order->purchase_amount,
				purchase_order->inventory_purchase_list,
				purchase_order->supply_purchase_list,
				purchase_order->service_purchase_list,
				purchase_order->purchase_asset_account_list );

		ledger_account_list_propagate(
			purchase_order->propagate_account_list,
			application_name );
	}

} /* post_change_fixed_asset_purchase_update() */

