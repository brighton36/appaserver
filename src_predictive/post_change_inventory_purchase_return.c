/* ----------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_inventory_purchase_return.c */
/* ----------------------------------------------------------------------- */
/* 									   */
/* Freely available software: see Appaserver.org			   */
/* ----------------------------------------------------------------------- */

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

void post_change_inventory_purchase_return_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name,
				char *return_date_time );

void post_change_inventory_purchase_return_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name,
				char *return_date_time );

void post_change_inventory_purchase_return_delete(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name,
				char *return_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *inventory_name;
	char *return_date_time;
	char *state;
	char *preupdate_inventory_name;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s full_name street_address purchase_date_time inventory_name return_date_time state preupdate_inventory_name\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	purchase_date_time = argv[ 3 ];
	inventory_name = argv[ 4 ];
	return_date_time = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_inventory_name = argv[ 7 ];

	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

	/* ----------------------------------------------------------- */
	/* INVENTORY_PURCHASE_RETURN.transaction_date_time DOES exist, */
	/* so execute predelete.				       */
	/* ----------------------------------------------------------- */
	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
/*
		post_change_inventory_purchase_return_delete(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			inventory_name,
			return_date_time );
*/
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_inventory_purchase_return_insert(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			inventory_name,
			return_date_time );
	}
	else
	{
/*
		post_change_inventory_purchase_return_update(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			inventory_name,
			return_date_time,
			preupdate_inventory_name );
*/
	}

	return 0;

} /* main() */

void post_change_inventory_purchase_return_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name,
			char *return_date_time )
{
	PURCHASE_ORDER *purchase_order;
	INVENTORY_PURCHASE *inventory_purchase;
	INVENTORY_PURCHASE_RETURN *inventory_purchase_return;

	purchase_order =
		purchase_order_fetch_new(
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

	if ( ! ( inventory_purchase =
			inventory_purchase_list_seek(
				purchase_order->inventory_purchase_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot seek inventory_purchase = [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	if ( ! ( inventory_purchase_return =
			inventory_purchase_return_list_seek(
				inventory_purchase->
					inventory_purchase_return_list,
				return_date_time ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: cannot seek inventory_purchase_return = [%s/%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 return_date_time );
		exit( 1 );
	}

	if ( inventory_purchase_return->transaction )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: transaction exists for [%s/%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 return_date_time );
	}

	inventory_purchase_return->transaction =
		inventory_purchase_return_transaction_new(
			&inventory_purchase_return->transaction_date_time,
			application_name,
			(char *)0 /* fund_name */,
			inventory_purchase->full_name,
			inventory_purchase->street_address,
			inventory_purchase->unit_cost,
			inventory_purchase->inventory_account_name,
			inventory_purchase_return->return_date_time,
			inventory_purchase_return->returned_quantity,
			inventory_purchase_return->sales_tax );

} /* post_change_inventory_purchase_return_insert() */

#ifdef NOT_DEFINED
void post_change_inventory_purchase_return_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name,
			char *return_date_time,
			char *preupdate_inventory_name )
{
	PURCHASE_ORDER *purchase_order;
	INVENTORY_PURCHASE *inventory_purchase;
	enum preupdate_change_state inventory_name_change_state;

	inventory_name_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_inventory_name,
			inventory_name /* postupdate_data */,
			"preupdate_inventory_name" );

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

	if ( ! ( inventory_purchase =
			inventory_purchase_list_seek(
				purchase_order->inventory_purchase_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot seek inventory = [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	if ( purchase_order->transaction_date_time )
	{
		purchase_order->transaction =
			ledger_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );

		if ( !purchase_order->transaction )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty transaction.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		ledger_transaction_refresh(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->transaction->transaction_amount,
			purchase_order->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			purchase_order->transaction->journal_ledger_list );
	}

} /* post_change_inventory_purchase_return_update() */

void post_change_inventory_purchase_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name,
			char *preupdate_ordered_quantity,
			char *preupdate_arrived_quantity,
			char *preupdate_missing_quantity,
			char *preupdate_unit_cost,
			char *preupdate_inventory_name  )
{
	PURCHASE_ORDER *purchase_order;
	enum preupdate_change_state ordered_quantity_change_state;
	enum preupdate_change_state arrived_quantity_change_state;
	enum preupdate_change_state missing_quantity_change_state;
	enum preupdate_change_state unit_cost_change_state;

	ordered_quantity_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_ordered_quantity,
			(char *)0 /* postupdate_data */,
			"preupdate_ordered_quantity" );

	arrived_quantity_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_arrived_quantity,
			(char *)0 /* postupdate_data */,
			"preupdate_arrived_quantity" );

	missing_quantity_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_missing_quantity,
			(char *)0 /* postupdate_data */,
			"preupdate_missing_quantity" );

	unit_cost_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_unit_cost,
			(char *)0 /* postupdate_data */,
			"preupdate_unit_cost" );

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

	if ( ordered_quantity_change_state == from_something_to_something_else
	||   ordered_quantity_change_state == from_null_to_something )
	{
		post_change_inventory_purchase_ordered_quantity_update(
			purchase_order,
			inventory_name,
			application_name );
	}

	if ( arrived_quantity_change_state == from_something_to_something_else
	||   arrived_quantity_change_state == from_null_to_something )
	{
		post_change_inventory_purchase_arrived_quantity_update(
			purchase_order,
			inventory_name,
			arrived_quantity_change_state,
			application_name );
	}

	if ( missing_quantity_change_state == from_something_to_something_else
	||   missing_quantity_change_state == from_null_to_something )
	{
		post_change_inventory_purchase_missing_quantity_update(
			purchase_order,
			inventory_name,
			application_name );
	}

	if ( unit_cost_change_state == from_something_to_something_else
	||   unit_cost_change_state == from_null_to_something )
	{
		post_change_inventory_purchase_unit_cost_update(
			purchase_order,
			inventory_name,
			application_name );
	}

	if ( inventory_name_change_state == from_something_to_something_else )
	{
		post_change_inventory_purchase_inventory_name_update(
			purchase_order,
			inventory_name,
			preupdate_inventory_name,
			application_name );
	}

} /* post_change_inventory_purchase_update() */

void post_change_inventory_purchase_inventory_name_update(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			char *preupdate_inventory_name,
			char *application_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		preupdate_inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );

	system( sys_string );

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );

	system( sys_string );

	if ( purchase_order->transaction )
	{
		strcpy(sys_string,
"propagate_purchase_order_accounts ignored fund transaction_date_time" );

		system( sys_string );
	}

} /* post_change_inventory_purchase_inventory_name_update() */

void post_change_inventory_purchase_missing_quantity_update(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			char *application_name )
{
	char sys_string[ 1024 ];
	INVENTORY_PURCHASE *inventory_purchase;

	inventory_purchase =
		inventory_purchase_list_seek(
			purchase_order->inventory_purchase_list,
			inventory_name );

	inventory_purchase->quantity_on_hand =
		inventory_get_quantity_on_hand(
			inventory_purchase->arrived_quantity,
			inventory_purchase->missing_quantity );

	inventory_purchase_list_update(
		application_name,
		purchase_order->
			inventory_purchase_list );

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );

	system( sys_string );

	strcpy(sys_string,
"propagate_purchase_order_accounts ignored fund transaction_date_time" );

	system( sys_string );

} /* post_change_inventory_purchase_missing_quantity_update() */

void post_change_inventory_purchase_arrived_quantity_update(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			enum preupdate_change_state
				arrived_quantity_change_state,
			char *application_name )
{
	char sys_string[ 1024 ];

	if ( arrived_quantity_change_state == from_null_to_something
	&&   purchase_inventory_is_latest(
			application_name,
			inventory_name,
			purchase_order->purchase_date_time ) )
	{
		return;
	}

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );

	system( sys_string );

} /* post_change_inventory_purchase_arrived_quantity_update() */

void post_change_inventory_purchase_ordered_quantity_update(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			char *application_name )
{
	INVENTORY_PURCHASE *inventory_purchase;

	inventory_purchase =
		inventory_purchase_list_seek(
			purchase_order->inventory_purchase_list,
			inventory_name );

	inventory_purchase->extension =
		inventory_purchase_get_extension(
			inventory_purchase->ordered_quantity,
			inventory_purchase->unit_cost );

	inventory_purchase->quantity_on_hand =
		inventory_get_quantity_on_hand(
			inventory_purchase->arrived_quantity,
			inventory_purchase->missing_quantity );

	inventory_purchase_list_update(
		application_name,
		purchase_order->
			inventory_purchase_list );

	purchase_order->purchase_amount =
		purchase_order_get_purchase_amount(
			&purchase_order->sum_inventory_extension,
			&purchase_order->sum_specific_inventory_extension,
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

	if ( purchase_order->transaction_date_time )
	{
		purchase_order->transaction =
			ledger_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );

		if ( !purchase_order->transaction )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty transaction.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		ledger_transaction_refresh(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->transaction->transaction_amount,
			purchase_order->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			purchase_order->transaction->journal_ledger_list );
	}

} /* post_change_inventory_purchase_ordered_quantity_update() */

void post_change_inventory_purchase_unit_cost_update(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			char *application_name )
{
	INVENTORY_PURCHASE *inventory_purchase;

	inventory_purchase =
		inventory_purchase_list_seek(
			purchase_order->inventory_purchase_list,
			inventory_name );

	inventory_purchase->extension =
		inventory_purchase_get_extension(
			inventory_purchase->ordered_quantity,
			inventory_purchase->unit_cost );

	inventory_purchase_list_update(
			application_name,
			purchase_order->
				inventory_purchase_list );

	purchase_order->purchase_amount =
		purchase_order_get_purchase_amount(
			&purchase_order->sum_inventory_extension,
			&purchase_order->sum_specific_inventory_extension,
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

	if ( purchase_order->transaction_date_time )
	{
		purchase_order->transaction =
			ledger_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );

		if ( !purchase_order->transaction )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty transaction.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		ledger_transaction_refresh(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->transaction->transaction_amount,
			purchase_order->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			purchase_order->transaction->journal_ledger_list );
	}

} /* post_change_inventory_purchase_unit_cost_update() */

void post_change_inventory_purchase_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name )
{
	PURCHASE_ORDER *purchase_order;
	char sys_string[ 1024 ];

	purchase_order =
		purchase_order_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time );

	if ( !purchase_order )
	{
		sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" '' n",
	 		application_name,
	 		inventory_name );

		system( sys_string );

		exit( 0 );
	}

	purchase_order->purchase_amount =
		purchase_order_get_purchase_amount(
			&purchase_order->sum_inventory_extension,
			&purchase_order->sum_specific_inventory_extension,
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

	if ( purchase_order->transaction_date_time )
	{
		purchase_order->transaction =
			ledger_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );

		if ( purchase_order->transaction )
		{
			ledger_transaction_refresh(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->transaction_amount,
				purchase_order->transaction->memo,
				0 /* check_number */,
				1 /* lock_transaction */,
				purchase_order->
					transaction->
					journal_ledger_list );
		}
	}

	if ( purchase_order->arrived_date_time )
	{
		boolean is_latest;

		is_latest =
		 	purchase_inventory_is_latest(
				application_name,
				inventory_name,
				purchase_order->purchase_date_time );

		if ( is_latest )
		{
			sprintf( sys_string,
	"propagate_inventory_purchase_layers %s \"%s\" \"%s\" \"%s\" \"%s\" y",
		 		application_name,
		 		purchase_order->full_name,
		 		purchase_order->street_address,
		 		purchase_order->purchase_date_time,
		 		inventory_name );
		}
		else
		{
			sprintf( sys_string,
	"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 			application_name,
	 			inventory_name,
				(purchase_order->transaction_date_time)
					? purchase_order->transaction_date_time
					: "" );
		}

		system( sys_string );
	}

} /* post_change_inventory_purchase_delete() */

void post_change_inventory_purchase_insert_title_passage_rule_null(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			char *application_name )
{
	INVENTORY_PURCHASE *inventory_purchase;
	char sys_string[ 1024 ];
	boolean is_latest;

	is_latest =
		 purchase_inventory_is_latest(
			application_name,
			inventory_name,
			purchase_order->purchase_date_time );

	if ( ! ( inventory_purchase =
			inventory_purchase_list_seek(
				purchase_order->inventory_purchase_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot seek inventory_name = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	if ( !inventory_purchase->arrived_quantity )
	{
		inventory_purchase->arrived_quantity =
			inventory_purchase->ordered_quantity;
	}

	inventory_purchase->quantity_on_hand =
		inventory_get_quantity_on_hand(
			inventory_purchase->arrived_quantity,
			inventory_purchase->missing_quantity );

	inventory_purchase->average_unit_cost =
		inventory_purchase->capitalized_unit_cost;

	inventory_purchase_list_update(
		application_name,
		purchase_order->inventory_purchase_list );

	if ( purchase_order->transaction_date_time )
	{
		purchase_order->transaction =
			ledger_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );

		if ( !purchase_order->transaction )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty transaction.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		ledger_transaction_refresh(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->transaction->transaction_amount,
			purchase_order->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			purchase_order->transaction->journal_ledger_list );
	}

	if ( is_latest )
	{
		sprintf( sys_string,
	"propagate_inventory_purchase_layers %s \"%s\" \"%s\" \"%s\" \"%s\" y",
	 		application_name,
	 		purchase_order->full_name,
	 		purchase_order->street_address,
	 		purchase_order->purchase_date_time,
	 		inventory_name );
	}
	else
	{
		sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );
	}

	system( sys_string );

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

} /* post_change_inventory_purchase_insert_title_passage_rule_null() */
#endif
