/* ---------------------------------------------------------------	*/
/* src_accountancymodel/post_change_service_purchase.c			*/
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
#include "inventory.h"
#include "entity.h"
#include "ledger.h"
#include "purchase.h"
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_service_purchase_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

void post_change_service_purchase_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

void post_change_service_purchase_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *account,
			char *preupdate_account );

int main( int argc, char **argv )
{
	char *database_string = {0};
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *account;
	char *state;
	char *preupdate_account;

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s application full_name street_address purchase_date_time account state preupdate_account\n",
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
	account = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_account = argv[ 7 ];

	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "delete" ) == 0 )
	{
		post_change_service_purchase_delete(
			application_name,
			full_name,
			street_address,
			purchase_date_time );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_service_purchase_insert(
			application_name,
			full_name,
			street_address,
			purchase_date_time );
	}

	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_service_purchase_update(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			account,
			preupdate_account);
	}

	return 0;

} /* main() */

void post_change_service_purchase_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *account,
			char *preupdate_account )
{
	PURCHASE_ORDER *purchase_order;
	enum preupdate_change_state account_change_state;

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

/*
	purchase_order->purchase_amount =
		purchase_order_get_purchase_amount(
			&purchase_order->sum_inventory_extension,
			&purchase_order->sum_specific_inventory_unit_cost,
			&purchase_order->sum_supply_extension,
			&purchase_order->sum_service_extension,
			&purchase_order->sum_fixed_asset_extension,
			&purchase_order->sum_extension,
			purchase_order->inventory_purchase_list,
			purchase_order->specific_inventory_purchase_list,
			purchase_order->supply_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->fixed_asset_purchase_list,
			purchase_order->sales_tax,
			purchase_order->freight_in );
*/

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

	purchase_order->propagate_account_list =
		purchase_order_journal_ledger_refresh(
			application_name,
			purchase_order->fund_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->sum_inventory_extension,
			purchase_order->sum_specific_inventory_unit_cost,
			purchase_order->sum_supply_extension,
			purchase_order->sum_service_extension,
			purchase_order->sum_fixed_asset_extension,
			purchase_order->sales_tax,
			purchase_order->freight_in,
			purchase_order->purchase_amount,
			purchase_order->service_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->fixed_asset_purchase_list );

	ledger_account_list_balance_update(
			purchase_order->propagate_account_list,
			application_name );

	account_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_account,
			account /* postupdate_data */,
			"preupdate_account" );

	if ( account_change_state == from_something_to_something_else )
	{
		ledger_propagate(
			application_name,
			purchase_order->
				transaction->
				transaction_date_time,
			preupdate_account );
	}

} /* post_change_service_purchase_update() */

void post_change_service_purchase_insert(
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

/*
	purchase_order->purchase_amount =
		purchase_order_get_purchase_amount(
			&purchase_order->sum_inventory_extension,
			&purchase_order->sum_specific_inventory_unit_cost,
			&purchase_order->sum_supply_extension,
			&purchase_order->sum_service_extension,
			&purchase_order->sum_fixed_asset_extension,
			&purchase_order->sum_extension,
			purchase_order->inventory_purchase_list,
			purchase_order->specific_inventory_purchase_list,
			purchase_order->supply_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->fixed_asset_purchase_list,
			purchase_order->sales_tax,
			purchase_order->freight_in );
*/

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

	purchase_order->propagate_account_list =
		purchase_order_journal_ledger_refresh(
			application_name,
			purchase_order->fund_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->sum_inventory_extension,
			purchase_order->sum_specific_inventory_unit_cost,
			purchase_order->sum_supply_extension,
			purchase_order->sum_service_extension,
			purchase_order->sum_fixed_asset_extension,
			purchase_order->sales_tax,
			purchase_order->freight_in,
			purchase_order->purchase_amount,
			purchase_order->service_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->fixed_asset_purchase_list );

	ledger_account_list_balance_update(
			purchase_order->propagate_account_list,
			application_name );

} /* post_change_service_purchase_insert() */

void post_change_service_purchase_delete(
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

/*
	purchase_order->purchase_amount =
		purchase_order_get_purchase_amount(
			&purchase_order->sum_inventory_extension,
			&purchase_order->sum_specific_inventory_unit_cost,
			&purchase_order->sum_supply_extension,
			&purchase_order->sum_service_extension,
			&purchase_order->sum_fixed_asset_extension,
			&purchase_order->sum_extension,
			purchase_order->inventory_purchase_list,
			purchase_order->specific_inventory_purchase_list,
			purchase_order->supply_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->fixed_asset_purchase_list,
			purchase_order->sales_tax,
			purchase_order->freight_in );
*/

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

	if ( purchase_order->transaction )
	{
		purchase_order->propagate_account_list =
			purchase_order_journal_ledger_refresh(
				application_name,
				purchase_order->fund_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->sum_inventory_extension,
				purchase_order->
					sum_specific_inventory_unit_cost,
				purchase_order->sum_supply_extension,
				purchase_order->sum_service_extension,
				purchase_order->sum_fixed_asset_extension,
				purchase_order->sales_tax,
				purchase_order->freight_in,
				purchase_order->purchase_amount,
				purchase_order->service_purchase_list,
				purchase_order->service_purchase_list,
				purchase_order->fixed_asset_purchase_list );

		ledger_account_list_balance_update(
			purchase_order->propagate_account_list,
			application_name );
	}

} /* post_change_service_purchase_delete() */

