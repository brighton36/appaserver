/* ---------------------------------------------------------------	*/
/* src_accountancymodel/propagate_inventory_purchase_layers.c		*/
/* ---------------------------------------------------------------	*/
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
void propagate_inventory_purchase_layers_not_latest(
				INVENTORY *purchase_inventory,
				LIST *customer_sale_list,
				char *application_name,
				enum inventory_cost_method );

void propagate_inventory_purchase_layers_latest(
				INVENTORY_PURCHASE *inventory_purchase,
				INVENTORY *inventory,
				char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *inventory_name;
	boolean is_latest;
	ENTITY_SELF *entity_self;
	char *database_string = {0};

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s application full_name street_address purchase_date_time inventory_name is_latest_yn\n",
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
	inventory_name = argv[ 5 ];
	is_latest = (*argv[ 6 ] == 'y');

is_latest = 0;

	if ( is_latest )
	{
		INVENTORY *inventory;
		INVENTORY_PURCHASE *inventory_purchase;

		inventory_purchase =
			inventory_purchase_fetch(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				inventory_name );

		inventory = inventory_load_new(
				application_name,
				inventory_name );

		propagate_inventory_purchase_layers_latest(
			inventory_purchase,
			inventory,
			application_name );
	}
	else
	{
		entity_self =
			entity_self_purchase_inventory_load(
				application_name,
				inventory_name );

		propagate_inventory_purchase_layers_not_latest(
			entity_self->purchase_inventory,
			entity_self->customer_sale_list,
			application_name,
			entity_self->inventory_cost_method );
	}

	return 0;

} /* main() */

void propagate_inventory_purchase_layers_latest(
				INVENTORY_PURCHASE *inventory_purchase,
				INVENTORY *inventory,
				char *application_name )
{
	if ( !inventory_purchase )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: inventory_purchase is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !inventory_purchase->arrived_quantity )
	{
		inventory_purchase->arrived_quantity =
			inventory_purchase->ordered_quantity;
	}

	inventory_purchase->quantity_on_hand =
		inventory_purchase->arrived_quantity;

	inventory_purchase->extension =
		inventory_purchase_get_extension(
				inventory_purchase->ordered_quantity,
				inventory_purchase->unit_cost );

	inventory->last_inventory_balance->total_cost_balance =
		inventory_purchase_get_total_cost_balance(
			&inventory->last_inventory_balance->quantity_on_hand,
			&inventory->last_inventory_balance->average_unit_cost,
			inventory->last_inventory_balance->total_cost_balance,
			inventory_purchase->unit_cost,
			inventory_purchase->ordered_quantity );

	inventory_last_inventory_balance_update(
		inventory->last_inventory_balance->quantity_on_hand,
		inventory->last_inventory_balance->average_unit_cost,
		inventory->last_inventory_balance->total_cost_balance,
		inventory->inventory_name,
		application_name );

	inventory_purchase_update(
		application_name,
		inventory->inventory_name,
		inventory_purchase->full_name,
		inventory_purchase->street_address,
		inventory_purchase->purchase_date_time,
		inventory_purchase->arrived_quantity,
		inventory_purchase->database_arrived_quantity,
		inventory_purchase->quantity_on_hand,
		inventory_purchase->database_quantity_on_hand,
		inventory_purchase->extension,
		inventory_purchase->database_extension,
		inventory_purchase->average_unit_cost,
		inventory_purchase->database_average_unit_cost );

} /* propagate_inventory_purchase_layers_latest() */

void propagate_inventory_purchase_layers_not_latest(
				INVENTORY *purchase_inventory,
				LIST *customer_sale_list,
				char *application_name,
				enum inventory_cost_method
					inventory_cost_method )
{
	purchase_inventory->inventory_balance_list =
		inventory_get_average_cost_inventory_balance_list(
			purchase_inventory->inventory_purchase_list,
			purchase_inventory->inventory_sale_list );

	if ( list_length( purchase_inventory->inventory_balance_list ) )
	{
		purchase_inventory->last_inventory_balance =
			list_get_last_pointer(
				purchase_inventory->inventory_balance_list );
	}
	else
	{
		purchase_inventory->last_inventory_balance =
			inventory_balance_new();
	}

	inventory_last_inventory_balance_update(
		purchase_inventory->last_inventory_balance->quantity_on_hand,
		purchase_inventory->last_inventory_balance->average_unit_cost,
		purchase_inventory->last_inventory_balance->total_cost_balance,
		purchase_inventory->inventory_name,
		application_name );

	if ( inventory_cost_method == inventory_fifo )
	{
		inventory_reset_quantity_on_hand(
			purchase_inventory->inventory_purchase_list );

		inventory_set_quantity_on_hand_CGS_fifo(
			purchase_inventory->inventory_sale_list,
			purchase_inventory->inventory_purchase_list );
	}
	else
	if ( inventory_cost_method == inventory_lifo )
	{
		inventory_reset_quantity_on_hand(
			purchase_inventory->inventory_purchase_list );

		inventory_set_quantity_on_hand_CGS_lifo(
			purchase_inventory->inventory_sale_list,
			purchase_inventory->inventory_purchase_list );
	}
	else
	/* ----------------------------------------------------- */
	/* Must be either inventory_average or inventory_not_set */
	/* ----------------------------------------------------- */
	{
		inventory_reset_quantity_on_hand(
			purchase_inventory->inventory_purchase_list );

		inventory_set_quantity_on_hand_fifo(
			purchase_inventory->inventory_sale_list,
			purchase_inventory->inventory_purchase_list );
	}

	/* Update inventory_purchase->quantity_on_hand. */
	/* -------------------------------------------- */
	inventory_purchase_list_update(
		application_name,
		purchase_inventory->inventory_purchase_list );

	inventory_sale_list_cost_of_goods_sold_update(
		application_name,
		purchase_inventory->inventory_sale_list,
		purchase_inventory->inventory_name );

	/* Set customer_sale->cost_of_goods_sold = sum(inventory_sale) */
	/* ----------------------------------------------------------- */
	customer_sale_list_cost_of_goods_sold_set( customer_sale_list );

	/* ---------------------------------------- */
	/* Update JOURNAL_LEDGER.debit_amount from  */
	/* INVENTORY_SALE.cost_of_goods_sold	    */
	/* ---------------------------------------- */
	customer_sale_list_transaction_update(
		application_name,
		customer_sale_list );

} /* propagate_inventory_purchase_layers_not_latest() */

