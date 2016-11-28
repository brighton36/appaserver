/* ---------------------------------------------------------------	*/
/* src_accountancymodel/post_change_depreciation.c			*/
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
#include "ledger.h"
#include "purchase.h"
#include "column.h"
#include "depreciation.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_depreciation_entity(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *application_name );

void post_change_depreciation_date_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date );

/*
void post_change_depreciation_asset_serial_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date,
			char *preupdate_asset_name,
			char *preupdate_serial_number );
*/

void post_change_depreciation_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date );

void post_change_depreciation_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date );

void post_change_depreciation_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *preupdate_purchase_date_time,
			char *preupdate_asset_name,
			char *preupdate_serial_number,
			char *preupdate_depreciation_date,
			char *preupdate_units_produced );

int main( int argc, char **argv )
{
	char *database_string = {0};
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *asset_name;
	char *serial_number;
	char *depreciation_date;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_purchase_date_time;
	char *preupdate_asset_name;
	char *preupdate_serial_number;
	char *preupdate_depreciation_date;
	char *preupdate_units_produced;

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( argc != 16 )
	{
		fprintf( stderr,
"Usage: %s application full_name street_address purchase_date_time asset_name serial_number depreciation_date state preupdate_full_name preupdate_street_address preupdate_purchase_date_time preupdate_asset_name preupdate_serial_number preupdate_depreciation_date preupdate_units_produced\n",
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
	depreciation_date = argv[ 7 ];
	state = argv[ 8 ];
	preupdate_full_name = argv[ 9 ];
	preupdate_street_address = argv[ 10 ];
	preupdate_purchase_date_time = argv[ 11 ];
	preupdate_asset_name = argv[ 12 ];
	preupdate_serial_number = argv[ 13 ];
	preupdate_depreciation_date = argv[ 14 ];
	preupdate_units_produced = argv[ 15 ];

	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

	/* ------------------------------------ */
	/* Execute on preupdate because we have	*/
	/* DEPRECIATION.transaction_date_time.  */
	/* ------------------------------------ */

	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_depreciation_delete(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			serial_number,
			depreciation_date );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_depreciation_insert(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			serial_number,
			depreciation_date );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_depreciation_update(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			serial_number,
			depreciation_date,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_purchase_date_time,
			preupdate_asset_name,
			preupdate_serial_number,
			preupdate_depreciation_date,
			preupdate_units_produced );
	}

	return 0;

} /* main() */

void post_change_depreciation_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *preupdate_purchase_date_time,
			char *preupdate_asset_name,
			char *preupdate_serial_number,
			char *preupdate_depreciation_date,
			char *preupdate_units_produced )
{
	enum preupdate_change_state full_name_change_state;
	enum preupdate_change_state street_address_change_state;
	enum preupdate_change_state purchase_date_time_change_state;
	enum preupdate_change_state asset_name_change_state;
	enum preupdate_change_state serial_number_change_state;
	enum preupdate_change_state depreciation_date_change_state;
	enum preupdate_change_state units_produced_change_state;

	full_name_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_full_name,
			full_name /* postupdate_data */,
			"preupdate_full_name" );

	street_address_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_street_address,
			street_address /* postupdate_data */,
			"preupdate_street_address" );

	purchase_date_time_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_purchase_date_time,
			purchase_date_time /* postupdate_data */,
			"preupdate_purchase_date_time" );

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

	depreciation_date_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_depreciation_date,
			depreciation_date /* postupdate_data */,
			"preupdate_depreciation_date" );

	units_produced_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_units_produced,
			(char *)0 /* postupdate_data */,
			"preupdate_units_produced" );

	if ( full_name_change_state == from_something_to_something_else
	||   street_address_change_state == from_something_to_something_else )
	{
		post_change_depreciation_entity(
			full_name,
			street_address,
			purchase_date_time,
			preupdate_full_name,
			preupdate_street_address,
			application_name );
	}

	if ( asset_name_change_state == from_something_to_something_else
	||   serial_number_change_state == from_something_to_something_else
	||   purchase_date_time_change_state ==
		from_something_to_something_else )
	{
		/* Do nothing. */
	}

	if (	depreciation_date_change_state ==
		from_something_to_something_else )
	{
		post_change_depreciation_date_update(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			serial_number,
			depreciation_date );
	}

} /* post_change_depreciation_update() */

void post_change_depreciation_date_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date )
{
	PURCHASE_ORDER *purchase_order;
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	DEPRECIATION *depreciation;

	if ( ! ( purchase_order =
			purchase_order_new(
				application_name,
				full_name,
				street_address,
				purchase_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !purchase_order->arrived_date_time )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: purchase_order has empty arrived_date_time.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( purchase_fixed_asset =
			purchase_fixed_asset_list_seek(
				purchase_order->fixed_asset_purchase_list,
				asset_name,
				serial_number ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot seek fixed_asset = (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 asset_name,
			 serial_number );
		exit( 1 );
	}

	if ( !purchase_fixed_asset->depreciation_list )
	{
		purchase_fixed_asset->depreciation_list =
			depreciation_fetch_list(
				application_name,
				purchase_fixed_asset->full_name,
				purchase_fixed_asset->street_address,
				purchase_fixed_asset->purchase_date_time,
				purchase_fixed_asset->asset_name,
				purchase_fixed_asset->serial_number );
	}

	if ( ! ( depreciation =
			depreciation_list_seek(
				purchase_fixed_asset->depreciation_list,
				depreciation_date ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot seek depreciation = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 depreciation_date );
		exit( 1 );
	}

	depreciation->transaction_date_time =
		ledger_get_transaction_date_time(
			depreciation_date );

	depreciation_update(
		application_name,
		depreciation->full_name,
		depreciation->street_address,
		depreciation->purchase_date_time,
		depreciation->asset_name,
		depreciation->serial_number,
		depreciation->depreciation_date,
		depreciation->depreciation_amount,
		depreciation->database_depreciation_amount,
		depreciation->transaction_date_time,
		depreciation->database_transaction_date_time );

	ledger_transaction_generic_update(
		application_name,
		depreciation->full_name,
		depreciation->street_address,
		depreciation->transaction->transaction_date_time,
		"transaction_date_time" /* attribute_name */,
		depreciation->transaction_date_time /* data */ );

	ledger_journal_generic_update(
		application_name,
		depreciation->full_name,
		depreciation->street_address,
		depreciation->transaction->transaction_date_time,
		"transaction_date_time" /* attribute_name */,
		depreciation->transaction_date_time /* data */ );

	depreciation->transaction->transaction_date_time =
		depreciation->transaction_date_time;

	purchase_fixed_asset_depreciation_propagate(
		purchase_fixed_asset,
		purchase_order->arrived_date_time,
		application_name,
		purchase_order->fund_name );

} /* post_change_depreciation_date_update() */

void post_change_depreciation_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date )
{
	PURCHASE_ORDER *purchase_order;
	DEPRECIATION *depreciation;
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	char *arrived_date_time;

	if ( ! ( arrived_date_time =
			purchase_order_fetch_arrived_date_time(
				application_name,
				full_name,
				street_address,
				purchase_date_time ) )
	||      !*arrived_date_time )
	{
		return;
	}

	if ( ! ( purchase_order =
			purchase_order_new(
				application_name,
				full_name,
				street_address,
				purchase_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( purchase_fixed_asset =
			purchase_fixed_asset_fetch(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				asset_name,
				serial_number ) ) )
	{
		fprintf( stderr,
		"ERROR In %s/%s()/%d: cannot fetch purchase_fixed_asset.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( depreciation =
			depreciation_list_seek(
				purchase_fixed_asset->depreciation_list,
				depreciation_date ) ) )
	{
		fprintf( stderr,
		"ERROR In %s/%s()/%d: cannot seek depreciation = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 depreciation_date );
		exit( 1 );
	}

	if ( !depreciation->transaction )
	{
		fprintf( stderr,
			 "ERROR In %s/%s()/%d: empty transaction.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Delete the TRANSACTION */
	/* ---------------------- */
	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			depreciation->transaction->full_name,
			depreciation->transaction->street_address,
			depreciation->
				transaction->
				transaction_date_time );

	/* Delete the TRANSACTION */
	/* ---------------------- */
	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			depreciation->transaction->full_name,
			depreciation->transaction->street_address,
			depreciation->
				transaction->
				transaction_date_time );

	list_delete_current( purchase_fixed_asset->depreciation_list );

	purchase_fixed_asset_depreciation_propagate(
		purchase_fixed_asset,
		arrived_date_time,
		application_name,
		purchase_order->fund_name );

} /* post_change_depreciation_delete() */

void post_change_depreciation_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date )
{
	PURCHASE_ORDER *purchase_order;
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	DEPRECIATION *depreciation;
	LIST *propagate_account_list;
	char *prior_depreciation_date;
	char arrived_date_string[ 16 ];

	if ( ! ( purchase_order =
			purchase_order_new(
				application_name,
				full_name,
				street_address,
				purchase_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !purchase_order->arrived_date_time )
	{
		depreciation_delete(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			serial_number,
			depreciation_date );
		return;
	}

	if ( ! ( purchase_fixed_asset =
			purchase_fixed_asset_list_seek(
				purchase_order->fixed_asset_purchase_list,
				asset_name,
				serial_number ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot seek fixed_asset = (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 asset_name,
			 serial_number );
		exit( 1 );
	}

	if ( !list_length( purchase_fixed_asset->depreciation_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty deprecation_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	column( arrived_date_string, 0, purchase_order->arrived_date_time );

	if ( list_length( purchase_fixed_asset->depreciation_list ) == 1 )
	{
		prior_depreciation_date = arrived_date_string;
	}
	else
	{
		depreciation =
			depreciation_list_seek( 
				purchase_fixed_asset->
					depreciation_list,
				depreciation_date );

		if ( !list_at_tail( 
				purchase_fixed_asset->
					depreciation_list ) )
		{
			/* If inserting a middle depreciation. */
			/* ----------------------------------- */
			purchase_fixed_asset_depreciation_propagate(
				purchase_fixed_asset,
				purchase_order->arrived_date_time,
				application_name,
				purchase_order->fund_name );

			return;
		}

		/* Get the next to the last one. */
		/* ----------------------------- */
		list_previous(
			purchase_fixed_asset->
					depreciation_list );

		depreciation =
			list_get(
				purchase_fixed_asset->
					depreciation_list );

		prior_depreciation_date =
			depreciation->depreciation_date;
	}

	depreciation =
		list_get_last_pointer( 
			purchase_fixed_asset->
				depreciation_list );

	depreciation->depreciation_amount =
		depreciation_get_amount(
			purchase_fixed_asset->depreciation_method,
			purchase_fixed_asset->extension,
			purchase_fixed_asset->estimated_residual_value,
			purchase_fixed_asset->estimated_useful_life_years,
			purchase_fixed_asset->estimated_useful_life_units,
			purchase_fixed_asset->declining_balance_n,
			prior_depreciation_date,
			depreciation->depreciation_date,
			purchase_fixed_asset->accumulated_depreciation,
			arrived_date_string,
			depreciation->units_produced );

	if ( depreciation->transaction_date_time )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: not expecting transaction_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 depreciation->transaction_date_time );
	}

	depreciation->transaction_date_time =
		ledger_get_transaction_date_time(
			depreciation->depreciation_date );
	
	depreciation->transaction =
		ledger_transaction_new(
			depreciation->full_name,
			depreciation->street_address,
			depreciation->transaction_date_time,
			DEPRECIATION_MEMO );
	
	ledger_transaction_insert(
		application_name,
		depreciation->transaction->full_name,
		depreciation->transaction->street_address,
		depreciation->transaction->transaction_date_time,
		depreciation->depreciation_amount /* transaction_amount */,
		depreciation->transaction->memo,
		0 /* check_number */ );

	if ( ( propagate_account_list =
		depreciation_journal_ledger_refresh(
			application_name,
			purchase_order->fund_name,
			depreciation->transaction->full_name,
			depreciation->transaction->street_address,
			depreciation->
				transaction->
				transaction_date_time,
			depreciation->depreciation_amount ) ) )
	{
		ledger_account_list_balance_update(
			propagate_account_list,
			application_name,
			depreciation->
				transaction->
				transaction_date_time );
	}

	depreciation_update(
		application_name,
		depreciation->full_name,
		depreciation->street_address,
		depreciation->purchase_date_time,
		depreciation->asset_name,
		depreciation->serial_number,
		depreciation->depreciation_date,
		depreciation->depreciation_amount,
		depreciation->database_depreciation_amount,
		depreciation->transaction_date_time,
		depreciation->database_transaction_date_time );

	purchase_fixed_asset->accumulated_depreciation +=
		depreciation->depreciation_amount;

	purchase_fixed_asset_update(
		application_name,
		purchase_fixed_asset->full_name,
		purchase_fixed_asset->street_address,
		purchase_fixed_asset->purchase_date_time,
		purchase_fixed_asset->asset_name,
		purchase_fixed_asset->serial_number,
		purchase_fixed_asset->accumulated_depreciation,
		purchase_fixed_asset->database_accumulated_depreciation );

} /* post_change_depreciation_insert() */

void post_change_depreciation_entity(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *application_name )
{
	PURCHASE_ORDER *purchase_order;
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	DEPRECIATION *depreciation;

	if ( ! ( purchase_order =
			purchase_order_new(
				application_name,
				full_name,
				street_address,
				purchase_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( purchase_order->fixed_asset_purchase_list ) )
		return;

	do {
		purchase_fixed_asset =
			list_get_pointer(
				purchase_order->
					fixed_asset_purchase_list );

		if ( !purchase_fixed_asset->depreciation_list )
		{
			purchase_fixed_asset->depreciation_list =
				depreciation_fetch_list(
					application_name,
					purchase_fixed_asset->full_name,
					purchase_fixed_asset->street_address,
					purchase_fixed_asset->
						purchase_date_time,
					purchase_fixed_asset->asset_name,
					purchase_fixed_asset->serial_number );
		}


		if ( !list_rewind(
			purchase_fixed_asset->
				depreciation_list ) )
		{
			continue;
		}

		do {
			depreciation =
				list_get_pointer(
					purchase_fixed_asset->
						depreciation_list );

			if ( !depreciation->transaction )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: empty transaction for depreciation_date = (%s).\n",
			 		__FILE__,
			 		__FUNCTION__,
			 		__LINE__,
			 		depreciation->depreciation_date );
				exit( 1 );
			}


			ledger_entity_update(
				application_name,
				full_name,
				street_address,
				depreciation->transaction_date_time,
				preupdate_full_name,
				preupdate_street_address );

		} while( list_next( purchase_fixed_asset->depreciation_list ) );

	} while( list_next( purchase_order->fixed_asset_purchase_list ) );

} /* post_change_depreciation_entity() */

