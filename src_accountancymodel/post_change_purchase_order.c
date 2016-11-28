/* ---------------------------------------------------------------	*/
/* src_accountancymodel/post_change_purchase_order.c			*/
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
#include "inventory.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "entity.h"
#include "date.h"
#include "ledger.h"
#include "purchase.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_purchase_order_new_transaction(
			PURCHASE_ORDER *purchase_order,
			char *transaction_date_time,
			char *application_name );

void post_change_purchase_order_changed_rule_to_null(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_changed_to_FOB_destination(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_changed_to_FOB_shipping(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_FOB_shipping_new_title_passage_rule(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_fixed_arrived_date_time(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_insert(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_FOB_shipping_mistakenly_shipped(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_mistakenly_arrived(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_FOB_shipping_fixed_shipped_date(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_FOB_shipping_just_arrived(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_just_arrived(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_FOB_shipping_just_shipped(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_update(
			PURCHASE_ORDER *purchase_order,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *preupdate_title_passage_rule,
			char *preupdate_shipped_date,
			char *preupdate_arrived_date_time,
			char *application_name );

void post_change_purchase_order_insert_FOB_shipping(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_insert_FOB_destination(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

void post_change_purchase_order_insert_title_passage_null(
			PURCHASE_ORDER *purchase_order,
			char *application_name );

int main( int argc, char **argv )
{
	char *application_name = {0};
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_title_passage_rule;
	char *preupdate_shipped_date;
	char *preupdate_arrived_date_time;
	char *database_string = {0};
	PURCHASE_ORDER *purchase_order;

	if ( argc > 1 )
	{
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
	}

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s application full_name street_address purchase_date_time state preupdate_full_name preupdate_street_address preupdate_title_passage_rule preupdate_shipped_date preupdate_arrived_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	purchase_date_time = argv[ 4 ];
	state = argv[ 5 ];
	preupdate_full_name = argv[ 6 ];
	preupdate_street_address = argv[ 7 ];
	preupdate_title_passage_rule = argv[ 8 ];
	preupdate_shipped_date = argv[ 9 ];
	preupdate_arrived_date_time = argv[ 10 ];

	/* -------------------------------------------- */
	/* Only execute state=predelete because we have	*/
	/* PURCHASE_ORDER.transaction_date_time.	*/
	/* -------------------------------------------- */
	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	/* ------------------------------------------------------------ */
	/* If changed ENTITY.full_name or ENTITY.street address,	*/
	/* then this process is executed because of propagation.	*/
	/* ------------------------------------------------------------ */
	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

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

	if ( strcmp( state, "predelete" ) == 0 )
	{
		if ( purchase_order->transaction )
		{
			purchase_order_transaction_delete_with_propagate(
				application_name,
				purchase_order->fund_name,
				full_name,
				street_address,
				purchase_order->transaction_date_time );

			purchase_order->transaction = (TRANSACTION *)0;
			purchase_order->transaction_date_time = (char *)0;
		}
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_purchase_order_update(
			purchase_order,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_title_passage_rule,
			preupdate_shipped_date,
			preupdate_arrived_date_time,
			application_name );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_purchase_order_insert(
			purchase_order,
			application_name );
	}
	else
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: unrecognized state = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 state );
		exit( 1 );
	}

	if ( strcmp( state, "predelete" ) != 0 )
	{
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
	}

	return 0;

} /* main() */

void post_change_purchase_order_insert(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	if ( purchase_order->title_passage_rule == FOB_shipping )
	{
		post_change_purchase_order_insert_FOB_shipping(
			purchase_order,
			application_name );
	}
	else
	if ( purchase_order->title_passage_rule == FOB_destination )
	{
		post_change_purchase_order_insert_FOB_destination(
			purchase_order,
			application_name );
	}
	else
	{
		post_change_purchase_order_insert_title_passage_null(
			purchase_order,
			application_name );
	}

} /* post_change_purchase_order_insert() */

void post_change_purchase_order_just_arrived(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	inventory_purchase_arrived_quantity_update_with_propagate(
		purchase_order->inventory_purchase_list,
		application_name,
		0 /* don't force_not_latest */ );

	if ( purchase_order->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: not expecting a transaction for this purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	post_change_purchase_order_new_transaction(
			purchase_order,
			purchase_order->arrived_date_time,
			application_name );

} /* post_change_purchase_order_just_arrived() */

void post_change_purchase_order_FOB_shipping_just_arrived(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	/* If it arrived, then it must have been shipped. */
	/* ---------------------------------------------- */
	if ( !purchase_order->shipped_date )
	{
		char buffer[ 16 ];

		if ( purchase_order->transaction )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: A transaction exists without a shipped_date.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		column( buffer, 0, purchase_order->arrived_date_time );
		purchase_order->shipped_date = strdup( buffer );
	}

	inventory_purchase_arrived_quantity_update_with_propagate(
		purchase_order->inventory_purchase_list,
		application_name,
		0 /* don't force_not_latest */ );

	if ( !purchase_order->transaction )
	{
		purchase_order->transaction_date_time =
			ledger_get_shipped_date_transaction_date_time(
				purchase_order->shipped_date );
	
		purchase_order->transaction =
			ledger_transaction_new(
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				PURCHASE_ORDER_MEMO );
	
		ledger_transaction_insert(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			0.0 /* transaction_amount */,
			purchase_order->transaction->memo,
			0 /* check_number */ );
	}

} /* post_change_purchase_order_FOB_shipping_just_arrived() */

void post_change_purchase_order_FOB_shipping_just_shipped(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	purchase_order->transaction_date_time =
		ledger_get_shipped_date_transaction_date_time(
			purchase_order->shipped_date );
	
	purchase_order->transaction =
		ledger_transaction_new(
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			PURCHASE_ORDER_MEMO );

	ledger_transaction_insert(
		application_name,
		purchase_order->transaction->full_name,
		purchase_order->transaction->street_address,
		purchase_order->transaction->transaction_date_time,
		0.0 /* transaction_amount */,
		purchase_order->transaction->memo,
		0 /* check_number */ );

} /* post_change_purchase_order_FOB_shipping_just_shipped() */

void post_change_purchase_order_FOB_shipping_mistakenly_shipped(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	purchase_order_transaction_delete_with_propagate(
		application_name,
		purchase_order->fund_name,
		purchase_order->transaction->full_name,
		purchase_order->transaction->street_address,
		purchase_order->transaction->transaction_date_time );

	purchase_order->transaction = (TRANSACTION *)0;
	purchase_order->transaction_date_time = (char *)0;

} /* post_change_purchase_order_FOB_shipping_mistakenly_shipped() */

void post_change_purchase_order_mistakenly_arrived(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	INVENTORY_PURCHASE *inventory_purchase;

	if ( list_rewind( purchase_order->inventory_purchase_list ) )
	{
		do {
			inventory_purchase =
				list_get(
					purchase_order->
						inventory_purchase_list );

			inventory_purchase->arrived_quantity = 0;

			if (	inventory_purchase->arrived_quantity !=
				inventory_purchase->database_arrived_quantity )
			{
				inventory_purchase_arrived_quantity_update(
					application_name,
					inventory_purchase->full_name,
					inventory_purchase->street_address,
					inventory_purchase->purchase_date_time,
					inventory_purchase->inventory_name,
					inventory_purchase->arrived_quantity,
					inventory_purchase->
						database_arrived_quantity );

				inventory_purchase->database_arrived_quantity =
					inventory_purchase->arrived_quantity;
			}

		} while( list_next( purchase_order->inventory_purchase_list ) );

		inventory_purchase_layers_propagate(
			application_name,
			purchase_order->inventory_purchase_list,
			1 /* force_not_latest */ );
	}

	if ( purchase_order->title_passage_rule != FOB_shipping )
	{
		purchase_order_transaction_delete_with_propagate(
			application_name,
			purchase_order->fund_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time );

		purchase_order->transaction_date_time = (char *)0;
		purchase_order->transaction = (TRANSACTION *)0;
	}

	if ( list_length( purchase_order->fixed_asset_purchase_list ) )
	{
		purchase_fixed_asset_list_depreciation_delete(
				purchase_order->fixed_asset_purchase_list,
				application_name,
				purchase_order->fund_name );
	}

} /* post_change_purchase_order_mistakenly_arrived() */

void post_change_purchase_order_FOB_shipping_fixed_shipped_date(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	char *new_transaction_date_time;
	char *propagate_transaction_date_time;
	char sys_string[ 1024 ];

	if ( !purchase_order->transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty purchase_order->transaction.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	new_transaction_date_time =
		ledger_get_shipped_date_transaction_date_time(
			purchase_order->shipped_date );

	ledger_transaction_generic_update(
		application_name,
		purchase_order->transaction->full_name,
		purchase_order->transaction->street_address,
		purchase_order->transaction->transaction_date_time,
		"transaction_date_time",
		new_transaction_date_time );

	ledger_journal_generic_update(
		application_name,
		purchase_order->transaction->full_name,
		purchase_order->transaction->street_address,
		purchase_order->transaction->transaction_date_time,
		"transaction_date_time",
		new_transaction_date_time );

	if ( strcmp(	purchase_order->transaction_date_time,
			new_transaction_date_time ) < 0 )
	{
		propagate_transaction_date_time =
			purchase_order->transaction_date_time;
	}
	else
	{
		propagate_transaction_date_time = new_transaction_date_time;
	}

	sprintf( sys_string,
		 "propagate_purchase_order_accounts %s \"%s\" \"%s\"",
		 application_name,
		 purchase_order->fund_name,
		 propagate_transaction_date_time );

	system( sys_string );

	purchase_order->transaction_date_time =
	purchase_order->transaction->transaction_date_time =
		new_transaction_date_time;

} /* post_change_purchase_order_FOB_shipping_fixed_shipped_date() */

void post_change_purchase_order_insert_FOB_shipping(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	/* If it arrived, then it must have been shipped. */
	/* ---------------------------------------------- */
	if ( purchase_order->arrived_date_time )
	{
		char buffer[ 16 ];

		column( buffer, 0, purchase_order->arrived_date_time );
		purchase_order->shipped_date = strdup( buffer );
	}

	/* ---------------------------------------------------- */
	/* For propagate inventory see:				*/
	/* post_change_inventory_purchase_insert_FOB_shipping() */
	/* ---------------------------------------------------- */

	if ( purchase_order->shipped_date )
	{
		purchase_order->transaction_date_time =
			ledger_get_shipped_date_transaction_date_time(
				purchase_order->shipped_date );

		purchase_order->transaction =
			ledger_transaction_new(
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				PURCHASE_ORDER_MEMO );

		ledger_transaction_insert(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			0.0 /* transaction_amount */,
			purchase_order->transaction->memo,
			0 /* check_number */ );

		/* ---------------------------------------------------- */
		/* For propagate accounts see:				*/
		/* post_change_inventory_purchase_insert_FOB_shipping() */
		/* ---------------------------------------------------- */
	}

} /* post_change_purchase_order_insert_FOB_shipping() */

void post_change_purchase_order_insert_FOB_destination(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	if ( purchase_order->arrived_date_time )
	{
		purchase_order->transaction_date_time =
			purchase_order->arrived_date_time;

		purchase_order->transaction =
			ledger_transaction_new(
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				PURCHASE_ORDER_MEMO );

		ledger_transaction_insert(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			0.0 /* transaction_amount */,
			purchase_order->transaction->memo,
			0 /* check_number */ );
	}

} /* post_change_purchase_order_insert_FOB_destination() */

void post_change_purchase_order_insert_title_passage_null(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	if ( !purchase_order->arrived_date_time )
	{
		purchase_order->arrived_date_time =
			purchase_order->purchase_date_time;
	}

	purchase_order->transaction_date_time =
		purchase_order->arrived_date_time;

	purchase_order->transaction =
		ledger_transaction_new(
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			PURCHASE_ORDER_MEMO );

	ledger_transaction_insert(
		application_name,
		purchase_order->transaction->full_name,
		purchase_order->transaction->street_address,
		purchase_order->transaction->transaction_date_time,
		0.0 /* transaction_amount */,
		purchase_order->transaction->memo,
		0 /* check_number */ );

} /* post_change_purchase_order_insert_title_passage_null() */

void post_change_purchase_order_update(
			PURCHASE_ORDER *purchase_order,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *preupdate_title_passage_rule,
			char *preupdate_shipped_date,
			char *preupdate_arrived_date_time,
			char *application_name )
{
	enum preupdate_change_state full_name_change_state;
	enum preupdate_change_state street_address_change_state;
	enum preupdate_change_state shipped_date_change_state;
	enum preupdate_change_state arrived_date_time_change_state;
	enum preupdate_change_state title_passage_rule_change_state;

	full_name_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_full_name,
			purchase_order->full_name,
			"preupdate_full_name" );

	street_address_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_street_address,
			purchase_order->street_address,
			"preupdate_street_address" );

	shipped_date_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_shipped_date,
			(purchase_order->shipped_date)
				? purchase_order->shipped_date
				: "",
			"preupdate_shipped_date" );

	arrived_date_time_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_arrived_date_time,
			(purchase_order->arrived_date_time)
				? purchase_order->arrived_date_time
				: "",
			"preupdate_arrived_date_time" );

	title_passage_rule_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_title_passage_rule,
			entity_get_title_passage_rule_string(
				purchase_order->title_passage_rule ),
			"preupdate_title_passage_rule" );

	if ( full_name_change_state == from_something_to_something_else
	||   street_address_change_state == from_something_to_something_else )
	{
		if ( purchase_order->transaction_date_time )
		{
			ledger_entity_update(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				preupdate_full_name,
				preupdate_street_address );
		}
	}

	if ( shipped_date_change_state == from_null_to_something
	&&   purchase_order->title_passage_rule == FOB_shipping )
	{
		post_change_purchase_order_FOB_shipping_just_shipped(
			purchase_order,
			application_name );
	}

	if ( shipped_date_change_state == from_something_to_something_else
	&&   purchase_order->title_passage_rule == FOB_shipping )
	{
		post_change_purchase_order_FOB_shipping_fixed_shipped_date(
			purchase_order,
			application_name );
	}

	if ( shipped_date_change_state == from_something_to_null
	&&   purchase_order->title_passage_rule == FOB_shipping )
	{
		post_change_purchase_order_FOB_shipping_mistakenly_shipped(
			purchase_order,
			application_name );
	}

	if ( arrived_date_time_change_state == from_null_to_something )
	{
		if ( purchase_order->title_passage_rule == FOB_shipping )
		{
			post_change_purchase_order_FOB_shipping_just_arrived(
				purchase_order,
				application_name );
		}
		else
		{
			post_change_purchase_order_just_arrived(
				purchase_order,
				application_name );
		}
	}

	if (	arrived_date_time_change_state ==
		from_something_to_something_else )
	{
		post_change_purchase_order_fixed_arrived_date_time(
			purchase_order,
			application_name );
	}

	if ( arrived_date_time_change_state == from_something_to_null )
	{
		post_change_purchase_order_mistakenly_arrived(
			purchase_order,
			application_name );
	}

	if ( title_passage_rule_change_state == from_null_to_something
	&&   purchase_order->title_passage_rule == FOB_shipping )
	{
		post_change_purchase_order_FOB_shipping_new_title_passage_rule(
			purchase_order,
			application_name );
	}

	if (	title_passage_rule_change_state ==
		from_something_to_something_else )
	{
		if ( purchase_order->title_passage_rule == FOB_shipping )
		{
			post_change_purchase_order_changed_to_FOB_shipping(
				purchase_order,
				application_name );
		}
		else
		{
			post_change_purchase_order_changed_to_FOB_destination(
				purchase_order,
				application_name );
		}
	}

	if ( title_passage_rule_change_state == from_something_to_null )
	{
		post_change_purchase_order_changed_rule_to_null(
			purchase_order,
			application_name );
	}

} /* post_change_purchase_order_update() */

void post_change_purchase_order_changed_rule_to_null(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	char *transaction_date_time;

	if ( !purchase_order->arrived_date_time )
	{
		purchase_order->arrived_date_time =
			purchase_order->purchase_date_time;
	}

	if ( purchase_order->transaction )
	{
		char *propagate_transaction_date_time;

		transaction_date_time =
			purchase_order->arrived_date_time;

		ledger_transaction_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->
				transaction->
				transaction_date_time,
			"transaction_date_time",
			transaction_date_time );

		ledger_journal_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->
				transaction->
				transaction_date_time,
			"transaction_date_time",
			transaction_date_time );

		if ( strcmp(	purchase_order->transaction_date_time,
				transaction_date_time ) < 0 )
		{
			propagate_transaction_date_time =
				purchase_order->transaction_date_time;
		}
		else
		{
			propagate_transaction_date_time =
				transaction_date_time;
		}

		entity_propagate_purchase_order_ledger_accounts(
			application_name,
			purchase_order->fund_name,
		 	propagate_transaction_date_time );

		purchase_order->transaction_date_time =
		purchase_order->transaction->transaction_date_time =
			transaction_date_time;
	}
	else
	{
		purchase_order->transaction_date_time =
			purchase_order->arrived_date_time;

		purchase_order->transaction =
			ledger_transaction_new(
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				PURCHASE_ORDER_MEMO );

		ledger_transaction_insert(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			0.0 /* transaction_amount */,
			purchase_order->transaction->memo,
			0 /* check_number */ );

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
				purchase_order->supply_purchase_list,
				purchase_order->service_purchase_list,
				purchase_order->fixed_asset_purchase_list );

		ledger_account_list_balance_update(
			purchase_order->propagate_account_list,
			application_name,
			purchase_order->transaction_date_time );
	}

	if ( purchase_order->shipped_date )
	{
		purchase_order->shipped_date = (char *)0;
	}

} /* post_change_purchase_order_changed_rule_to_null() */

void post_change_purchase_order_changed_to_FOB_destination(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	char *transaction_date_time;

	if ( purchase_order->transaction )
	{
		if ( purchase_order->arrived_date_time )
		{
			char *propagate_transaction_date_time;

			transaction_date_time =
				purchase_order->arrived_date_time;

			ledger_transaction_generic_update(
				application_name,
				purchase_order->transaction->full_name,
				purchase_order->transaction->street_address,
				purchase_order->
					transaction->
					transaction_date_time,
				"transaction_date_time",
				transaction_date_time );

			ledger_journal_generic_update(
				application_name,
				purchase_order->transaction->full_name,
				purchase_order->transaction->street_address,
				purchase_order->
					transaction->
					transaction_date_time,
				"transaction_date_time",
				transaction_date_time );

			if ( strcmp(	purchase_order->transaction_date_time,
					transaction_date_time ) < 0 )
			{
				propagate_transaction_date_time =
					purchase_order->transaction_date_time;
			}
			else
			{
				propagate_transaction_date_time =
					transaction_date_time;
			}

			entity_propagate_purchase_order_ledger_accounts(
				application_name,
				purchase_order->fund_name,
		 		propagate_transaction_date_time );

			purchase_order->transaction_date_time =
			purchase_order->transaction->transaction_date_time =
				transaction_date_time;
		}
		else
		{
			purchase_order_transaction_delete_with_propagate(
				application_name,
				purchase_order->fund_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time );

			purchase_order->transaction = (TRANSACTION *)0;
			purchase_order->transaction_date_time = (char *)0;
		}
	}

} /* post_change_purchase_order_changed_to_FOB_destination() */

void post_change_purchase_order_changed_to_FOB_shipping(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	char *transaction_date_time;

	if ( purchase_order->transaction )
	{
		char *propagate_transaction_date_time;
		char sys_string[ 1024 ];

		transaction_date_time =
			ledger_get_shipped_date_transaction_date_time(
				purchase_order->shipped_date );

		ledger_transaction_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			"transaction_date_time",
			transaction_date_time );

		ledger_journal_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			"transaction_date_time",
			transaction_date_time );

		if ( strcmp( purchase_order->transaction_date_time,
				transaction_date_time ) < 0 )
		{
			propagate_transaction_date_time =
				purchase_order->transaction_date_time;
		}
		else
		{
			propagate_transaction_date_time =
				transaction_date_time;
		}

		sprintf(sys_string,
		 	"propagate_purchase_order_accounts %s \"%s\" \"%s\"",
		 	application_name,
			purchase_order->fund_name,
		 	propagate_transaction_date_time );

		system( sys_string );

		purchase_order->transaction_date_time =
		purchase_order->transaction->transaction_date_time =
			transaction_date_time;
	}
	else
	if ( purchase_order->shipped_date
	&&   *purchase_order->shipped_date )
	{
		purchase_order->transaction_date_time =
			ledger_get_shipped_date_transaction_date_time(
				purchase_order->shipped_date );

		purchase_order->transaction =
			ledger_transaction_new(
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				PURCHASE_ORDER_MEMO );

		ledger_transaction_insert(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			0.0 /* transaction_amount */,
			purchase_order->transaction->memo,
			0 /* check_number */ );

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
				purchase_order->supply_purchase_list,
				purchase_order->service_purchase_list,
				purchase_order->fixed_asset_purchase_list );

		ledger_account_list_balance_update(
			purchase_order->propagate_account_list,
			application_name,
			purchase_order->transaction_date_time );
	}

} /* post_change_purchase_order_changed_to_FOB_shipping() */

void post_change_purchase_order_FOB_shipping_new_title_passage_rule(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	char *propagate_transaction_date_time;
	char sys_string[ 1024 ];

	if ( !purchase_order->shipped_date )
	{
		char shipped_date[ 16 ];

		if ( !purchase_order->arrived_date_time )
		{
			purchase_order->arrived_date_time =
				purchase_order->purchase_date_time;
		}

		column( shipped_date, 0, purchase_order->arrived_date_time );

		purchase_order->shipped_date = strdup( shipped_date );
	}

	purchase_order->transaction_date_time =
		ledger_get_shipped_date_transaction_date_time(
			purchase_order->shipped_date );

	if ( purchase_order->transaction )
	{
		ledger_transaction_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			"transaction_date_time",
			purchase_order->transaction_date_time );
	
		ledger_journal_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			"transaction_date_time",
			purchase_order->transaction_date_time );
	
		if ( strcmp( purchase_order->
				transaction->
				transaction_date_time,
				purchase_order->transaction_date_time ) < 0 )
		{
			propagate_transaction_date_time =
				purchase_order->
					transaction->
					transaction_date_time;
		}
		else
		{
			propagate_transaction_date_time =
				purchase_order->transaction_date_time;
		}

		sprintf(sys_string,
		 	"propagate_purchase_order_accounts %s \"%s\" \"%s\"",
		 	application_name,
			purchase_order->fund_name,
		 	propagate_transaction_date_time );

		system( sys_string );

		purchase_order->transaction->transaction_date_time =
			purchase_order->transaction_date_time;
	}
	else
	{
		post_change_purchase_order_new_transaction(
			purchase_order,
			purchase_order->transaction_date_time,
			application_name );
	}

} /* post_change_purchase_order_FOB_shipping_new_title_passage_rule() */

void post_change_purchase_order_fixed_arrived_date_time(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	inventory_purchase_arrived_quantity_update_with_propagate(
		purchase_order->inventory_purchase_list,
		application_name,
		1 /* force_not_latest */ );

	if ( purchase_order->title_passage_rule != FOB_shipping )
	{
		char *propagate_transaction_date_time;
		char sys_string[ 1024 ];

		if ( !purchase_order->transaction )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty purchase_order->transaction.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
			exit( 1 );
		}

		ledger_transaction_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			"transaction_date_time",
			purchase_order->arrived_date_time );

		ledger_journal_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			"transaction_date_time",
			purchase_order->arrived_date_time );

		if ( strcmp(	purchase_order->transaction_date_time,
				purchase_order->arrived_date_time ) < 0 )
		{
			propagate_transaction_date_time =
				purchase_order->transaction_date_time;
		}
		else
		{
			propagate_transaction_date_time =
				purchase_order->arrived_date_time;
		}

		sprintf(sys_string,
		 	"propagate_purchase_order_accounts %s \"%s\" \"%s\"",
		 	application_name,
			purchase_order->fund_name,
		 	propagate_transaction_date_time );

		system( sys_string );

		purchase_order->transaction_date_time =
		purchase_order->transaction->transaction_date_time =
			purchase_order->arrived_date_time;
	}

	if ( list_length( purchase_order->fixed_asset_purchase_list ) )
	{
		purchase_fixed_asset_list_depreciation_method_update(
				purchase_order->fixed_asset_purchase_list,
				purchase_order->arrived_date_time,
				application_name,
				purchase_order->fund_name );
	}

} /* post_change_purchase_order_fixed_arrived_date_time() */

void post_change_purchase_order_new_transaction(
			PURCHASE_ORDER *purchase_order,
			char *transaction_date_time,
			char *application_name )
{
	purchase_order->transaction_date_time = transaction_date_time;

	purchase_order->transaction =
		ledger_transaction_new(
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			PURCHASE_ORDER_MEMO );
	
	ledger_transaction_insert(
		application_name,
		purchase_order->transaction->full_name,
		purchase_order->transaction->street_address,
		purchase_order->transaction->transaction_date_time,
		0.0 /* transaction_amount */,
		purchase_order->transaction->memo,
		0 /* check_number */ );

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
			purchase_order->supply_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->fixed_asset_purchase_list );

	ledger_account_list_balance_update(
		purchase_order->propagate_account_list,
		application_name,
		purchase_order->transaction_date_time );

} /* post_change_purchase_order_new_transaction() */

