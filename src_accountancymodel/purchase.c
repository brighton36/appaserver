/* -------------------------------------------------------------------- */
/* src_accountancymodel/purchase.c					*/
/* -------------------------------------------------------------------- */
/* This is the AccountancyModel purchase ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "date.h"
#include "folder.h"
#include "list.h"
#include "ledger.h"
#include "purchase.h"
#include "depreciation.h"
#include "inventory.h"
#include "entity.h"

PURCHASE_ORDER *purchase_order_calloc( void )
{
	PURCHASE_ORDER *p =
		(PURCHASE_ORDER *)
			calloc( 1, sizeof( PURCHASE_ORDER ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;

} /* purchase_order_calloc() */

PURCHASE_ORDER *purchase_order_new(	char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time )
{
	PURCHASE_ORDER *p;

	p = purchase_order_calloc();
	p->full_name = full_name;
	p->street_address = street_address;
	p->purchase_date_time = purchase_date_time;

	if ( !purchase_order_load(
				&p->sum_extension,
				&p->database_sum_extension,
				&p->sales_tax,
				&p->freight_in,
				&p->purchase_amount,
				&p->database_purchase_amount,
				&p->amount_due,
				&p->database_amount_due,
				&p->title_passage_rule,
				&p->shipped_date,
				&p->database_shipped_date,
				&p->arrived_date_time,
				&p->database_arrived_date_time,
				&p->transaction_date_time,
				&p->database_transaction_date_time,
				&p->fund_name,
				application_name,
				p->full_name,
				p->street_address,
				p->purchase_date_time ) )
	{
		return (PURCHASE_ORDER *)0;
	}

	p->inventory_purchase_list =
		inventory_get_inventory_purchase_list(
				application_name,
				p->full_name,
				p->street_address,
				p->purchase_date_time );

	p->specific_inventory_purchase_list =
		purchase_specific_inventory_get_list(
				application_name,
				p->full_name,
				p->street_address,
				p->purchase_date_time );

	p->supply_purchase_list =
		purchase_supply_get_list(
				application_name,
				p->full_name,
				p->street_address,
				p->purchase_date_time );

	p->service_purchase_list =
		purchase_service_get_list(
				application_name,
				p->full_name,
				p->street_address,
				p->purchase_date_time );

	p->fixed_asset_purchase_list =
		purchase_fixed_asset_get_list(
				application_name,
				p->full_name,
				p->street_address,
				p->purchase_date_time );

	if ( p->transaction_date_time )
	{
		p->transaction =
			ledger_transaction_with_load_new(
					application_name,
					p->full_name,
					p->street_address,
					p->transaction_date_time );
	}

	p->purchase_amount =
		purchase_order_get_purchase_amount(
			&p->sum_inventory_extension,
			&p->sum_specific_inventory_unit_cost,
			&p->sum_supply_extension,
			&p->sum_service_extension,
			&p->sum_fixed_asset_extension,
			&p->sum_extension,
			p->inventory_purchase_list,
			p->specific_inventory_purchase_list,
			p->supply_purchase_list,
			p->service_purchase_list,
			p->fixed_asset_purchase_list,
			p->sales_tax,
			p->freight_in );

	if ( ( p->vendor_payment_list =
		purchase_get_vendor_payment_list(
			application_name,
			p->full_name,
			p->street_address,
			p->purchase_date_time ) ) )
	{
		p->sum_payment_amount =
			purchase_get_sum_payment_amount(
				p->vendor_payment_list );
	}

	p->amount_due =
		PURCHASE_GET_AMOUNT_DUE(
			p->purchase_amount,
			p->sum_payment_amount );

	return p;

} /* purchase_order_new() */

double purchase_order_get_purchase_amount(
				double *sum_inventory_extension,
				double *sum_specific_inventory_unit_cost,
				double *sum_supply_extension,
				double *sum_service_extension,
				double *sum_fixed_asset_extension,
				double *sum_extension,
				LIST *inventory_purchase_list,
				LIST *specific_inventory_purchase_list,
				LIST *supply_purchase_list,
				LIST *service_purchase_list,
				LIST *fixed_asset_purchase_list,
				double sales_tax,
				double freight_in )
{
	*sum_inventory_extension = 0.0;
	*sum_inventory_extension =
		inventory_purchase_list_set_extension(
			inventory_purchase_list );

	*sum_specific_inventory_unit_cost = 0.0;
	*sum_specific_inventory_unit_cost =
		purchase_get_sum_specific_inventory_unit_cost(
			specific_inventory_purchase_list );

	*sum_supply_extension = 0.0;
	*sum_supply_extension =
		purchase_get_sum_supply_extension(
			supply_purchase_list );

	*sum_service_extension = 0.0;
	*sum_service_extension =
		purchase_get_sum_service_extension(
			service_purchase_list );

	*sum_fixed_asset_extension = 0.0;
	*sum_fixed_asset_extension =
		purchase_get_sum_fixed_asset_extension(
			fixed_asset_purchase_list );

	*sum_extension =	*sum_inventory_extension +
				*sum_specific_inventory_unit_cost +
				*sum_supply_extension +
				*sum_service_extension +
				*sum_fixed_asset_extension;

	return *sum_extension + sales_tax + freight_in;

} /* purchase_order_get_purchase_amount() */

char *purchase_order_get_select( void )
{
	char *select =
"full_name,street_address,purchase_date_time,sum_extension,sales_tax,freight_in,purchase_amount,amount_due,title_passage_rule,shipped_date,arrived_date_time,transaction_date_time,fund";

	return select;

} /* purchase_order_get_select() */

boolean purchase_order_load(	double *sum_extension,
				double *database_sum_extension,
				double *sales_tax,
				double *freight_in,
				double *purchase_amount,
				double *database_purchase_amount,
				double *amount_due,
				double *database_amount_due,
				enum title_passage_rule *title_passage_rule,
				char **shipped_date,
				char **database_shipped_date,
				char **arrived_date_time,
				char **database_arrived_date_time,
				char **transaction_date_time,
				char **database_transaction_date_time,
				char **fund_name,
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char *results;

	select = purchase_order_get_select();

	where = ledger_get_transaction_where(
			full_name,
			street_address,
			purchase_date_time,
			(char *)0 /* folder_name */,
			"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=purchase_order		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	purchase_order_parse(
			(char **)0 /* full_name */,
			(char **)0 /* street_address */,
			&purchase_date_time
				/* Won't change it, but needed */
				/* to set arrived_date_time    */,
			sum_extension,
			database_sum_extension,
			sales_tax,
			freight_in,
			purchase_amount,
			database_purchase_amount,
			amount_due,
			database_amount_due,
			title_passage_rule,
			shipped_date,
			database_shipped_date,
			arrived_date_time,
			database_arrived_date_time,
			transaction_date_time,
			database_transaction_date_time,
			fund_name,
			results );

	free( results );

	return 1;

} /* purchase_order_load() */

void purchase_order_parse(	char **full_name,
				char **street_address,
				char **purchase_date_time,
				double *sum_extension,
				double *database_sum_extension,
				double *sales_tax,
				double *freight_in,
				double *purchase_amount,
				double *database_purchase_amount,
				double *amount_due,
				double *database_amount_due,
				enum title_passage_rule *title_passage_rule,
				char **shipped_date,
				char **database_shipped_date,
				char **arrived_date_time,
				char **database_arrived_date_time,
				char **transaction_date_time,
				char **database_transaction_date_time,
				char **fund_name,
				char *results )
{
	char piece_buffer[ 1024 ];

	if ( full_name )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
		if ( *piece_buffer )
			*full_name = strdup( piece_buffer );
	}

	if ( street_address )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
		if ( *piece_buffer )
			*street_address = strdup( piece_buffer );
	}

	if ( purchase_date_time )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
		if ( *piece_buffer )
			*purchase_date_time = strdup( piece_buffer );
	}

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 3 );
	if ( *piece_buffer )
		*sum_extension =
		*database_sum_extension = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 4 );
	if ( *piece_buffer )
		*sales_tax = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 5 );
	if ( *piece_buffer )
		*freight_in = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 6 );
	if ( *piece_buffer )
		*purchase_amount =
		*database_purchase_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 7 );
	if ( *piece_buffer )
		*amount_due =
		*database_amount_due = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 8 );
	if ( *piece_buffer )
		*title_passage_rule =
			entity_get_title_passage_rule(
				piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 9 );
	if ( *piece_buffer )
		*shipped_date =
		*database_shipped_date = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 10 );
	if ( *piece_buffer )
		*arrived_date_time =
		*database_arrived_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 11 );
	if ( *piece_buffer )
		*transaction_date_time =
		*database_transaction_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 12 );
	if ( *piece_buffer )
		*fund_name = strdup( piece_buffer );

} /* purchase_order_parse() */

PURCHASE_SPECIFIC_INVENTORY *purchase_specific_inventory_new(
				char *inventory_name )
{
	PURCHASE_SPECIFIC_INVENTORY *p =
		(PURCHASE_SPECIFIC_INVENTORY *)
			calloc( 1, sizeof( PURCHASE_SPECIFIC_INVENTORY ) );

	p->inventory_name = inventory_name;
	return p;

} /* purchase_specific_inventory_new() */

PURCHASE_SUPPLY *purchase_supply_new( char *supply_name )
{
	PURCHASE_SUPPLY *p =
		(PURCHASE_SUPPLY *)
			calloc( 1, sizeof( PURCHASE_SUPPLY ) );

	p->supply_name = supply_name;
	return p;

} /* purchase_supply_new() */

PURCHASE_SERVICE *purchase_service_new( char *account_name )
{
	PURCHASE_SERVICE *p =
		(PURCHASE_SERVICE *)
			calloc( 1, sizeof( PURCHASE_SERVICE ) );

	p->account_name = account_name;
	return p;

} /* purchase_service_new() */

PURCHASE_FIXED_ASSET *purchase_fixed_asset_new( void )
{
	PURCHASE_FIXED_ASSET *p =
		(PURCHASE_FIXED_ASSET *)
			calloc( 1, sizeof( PURCHASE_FIXED_ASSET ) );

	if ( !p )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return p;

} /* purchase_fixed_asset_new() */

LIST *purchase_supply_get_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	PURCHASE_SUPPLY *purchase_supply;
	LIST *supply_purchase_list;

	select = "supply_name,quantity,unit_cost,extension";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=supply_purchase		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );
	supply_purchase_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		purchase_supply =
			purchase_supply_new(
				strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		purchase_supply->quantity = atoi( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		purchase_supply->unit_cost = atof( piece_buffer );

		purchase_supply->extension =
			purchase_supply_get_extension(
				purchase_supply->unit_cost,
				purchase_supply->quantity );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		purchase_supply->database_extension = atof( piece_buffer );

		list_append_pointer( supply_purchase_list, purchase_supply );
	}

	pclose( input_pipe );
	return supply_purchase_list;

} /* purchase_supply_get_list() */

LIST *purchase_specific_inventory_get_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char *where;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	PURCHASE_SPECIFIC_INVENTORY *purchase_specific_inventory;
	LIST *specific_inventory_purchase_list;

	select = "inventory_name,serial_number,unit_cost";

	folder = "specific_inventory_purchase";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	specific_inventory_purchase_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		purchase_specific_inventory =
			purchase_specific_inventory_new(
				strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		purchase_specific_inventory->serial_number =
			strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		purchase_specific_inventory->unit_cost = atof( piece_buffer );

		list_append_pointer(
			specific_inventory_purchase_list,
			purchase_specific_inventory );
	}

	pclose( input_pipe );
	return specific_inventory_purchase_list;

} /* purchase_specific_inventory_get_list() */

double purchase_supply_get_extension(
				double unit_cost,
				int quantity )
{
	return unit_cost * (double)quantity;

}

LIST *purchase_service_get_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	PURCHASE_SERVICE *purchase_service;
	LIST *service_purchase_list;

	select = "account,extension";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=service_purchase		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );
	service_purchase_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		purchase_service =
			purchase_service_new(
				strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		purchase_service->extension =
		purchase_service->database_extension = atof( piece_buffer );

		list_append_pointer( service_purchase_list, purchase_service );
	}

	pclose( input_pipe );
	return service_purchase_list;

} /* purchase_service_get_list() */

char *purchase_fixed_asset_get_select( void )
{
	char *select =
"full_name,street_address,purchase_date_time,fixed_asset_purchase.asset_name,account,serial_number,extension,estimated_useful_life_years,estimated_useful_life_units,estimated_residual_value,declining_balance_n,depreciation_method,accumulated_depreciation";
	return select;
}

PURCHASE_FIXED_ASSET *purchase_fixed_asset_parse( char *input_buffer )
{
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	char piece_buffer[ 256 ];

	purchase_fixed_asset = purchase_fixed_asset_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	purchase_fixed_asset->full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	purchase_fixed_asset->street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	purchase_fixed_asset->purchase_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	purchase_fixed_asset->asset_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	if ( *piece_buffer )
		purchase_fixed_asset->account_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	purchase_fixed_asset->serial_number = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *piece_buffer )
		purchase_fixed_asset->extension = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	if ( *piece_buffer )
		purchase_fixed_asset->estimated_useful_life_years =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	if ( *piece_buffer )
		purchase_fixed_asset->estimated_useful_life_units =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
	if ( *piece_buffer )
		purchase_fixed_asset->estimated_residual_value =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 10 );
	if ( *piece_buffer )
		purchase_fixed_asset->declining_balance_n =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 11 );
	if ( *piece_buffer )
		purchase_fixed_asset->depreciation_method =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 12 );
	if ( *piece_buffer )
		purchase_fixed_asset->accumulated_depreciation =
		purchase_fixed_asset->database_accumulated_depreciation =
			atof( piece_buffer );

	return purchase_fixed_asset;

} /* purchase_fixed_asset_parse() */

PURCHASE_FIXED_ASSET *purchase_fixed_asset_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time,
					char *asset_name,
					char *serial_number )
{
	char sys_string[ 2048 ];
	char *ledger_where;
	char buffer[ 256 ];
	char where[ 1024 ];
	char *join_where;
	char *select;
	char *folder;
	char *results;
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;

	select = purchase_fixed_asset_get_select();

	folder = "fixed_asset_purchase,fixed_asset";

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	join_where = "fixed_asset_purchase.asset_name = fixed_asset.asset_name";

	sprintf( where,
"%s and fixed_asset_purchase.asset_name = '%s' and serial_number = '%s' and %s",
		 ledger_where,
		 escape_character(	buffer,
					asset_name,
					'\'' ),
		 serial_number,
		 join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		return (PURCHASE_FIXED_ASSET *)0;
	}

	purchase_fixed_asset = purchase_fixed_asset_parse( results );

	purchase_fixed_asset->depreciation_list =
		depreciation_fetch_list(
			application_name,
			purchase_fixed_asset->full_name,
			purchase_fixed_asset->street_address,
			purchase_fixed_asset->purchase_date_time,
			purchase_fixed_asset->asset_name,
			purchase_fixed_asset->serial_number );

	return purchase_fixed_asset;

} /* purchase_fixed_asset_fetch() */

LIST *purchase_fixed_asset_get_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *ledger_where;
	char *join_where;
	char *select;
	char *folder;
	char input_buffer[ 2048 ];
	FILE *input_pipe;
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	LIST *fixed_asset_purchase_list;

	select = purchase_fixed_asset_get_select();

	folder = "fixed_asset_purchase,fixed_asset";

	join_where = "fixed_asset_purchase.asset_name = fixed_asset.asset_name";

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( where, "%s and %s", ledger_where, join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	fixed_asset_purchase_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		purchase_fixed_asset =
			purchase_fixed_asset_parse(
				input_buffer );

		purchase_fixed_asset->depreciation_list =
			depreciation_fetch_list(
				application_name,
				purchase_fixed_asset->full_name,
				purchase_fixed_asset->street_address,
				purchase_fixed_asset->purchase_date_time,
				purchase_fixed_asset->asset_name,
				purchase_fixed_asset->serial_number );

		list_append_pointer(	fixed_asset_purchase_list,
					purchase_fixed_asset );
	}

	pclose( input_pipe );
	return fixed_asset_purchase_list;

} /* purchase_fixed_asset_get_list() */

double purchase_get_sum_specific_inventory_unit_cost(
			LIST *specific_inventory_purchase_list )
{
	PURCHASE_SPECIFIC_INVENTORY *purchase_specific_inventory;
	double sum_unit_cost;

	if ( !list_rewind( specific_inventory_purchase_list ) ) return 0.0;

	sum_unit_cost = 0.0;

	do {
		purchase_specific_inventory =
			list_get( specific_inventory_purchase_list );

		sum_unit_cost += purchase_specific_inventory->unit_cost;

	} while( list_next( specific_inventory_purchase_list ) );

	return sum_unit_cost;
} /* purchase_get_sum_specific_inventory_unit_cost() */

double purchase_get_sum_supply_extension(
			LIST *supply_purchase_list )
{
	PURCHASE_SUPPLY *purchase_supply;
	double sum_extension;

	if ( !list_rewind( supply_purchase_list ) ) return 0.0;

	sum_extension = 0.0;

	do {
		purchase_supply = list_get( supply_purchase_list );
		sum_extension += purchase_supply->extension;

	} while( list_next( supply_purchase_list ) );

	return sum_extension;
} /* purchase_get_sum_supply_extension() */

double purchase_get_sum_service_extension(
			LIST *service_purchase_list )
{
	PURCHASE_SERVICE *purchase_service;
	double sum_extension;

	if ( !list_rewind( service_purchase_list ) ) return 0.0;

	sum_extension = 0.0;

	do {
		purchase_service = list_get( service_purchase_list );
		sum_extension += purchase_service->extension;

	} while( list_next( service_purchase_list ) );

	return sum_extension;
} /* purchase_get_sum_service_extension() */

double purchase_get_sum_fixed_asset_extension(
			LIST *fixed_asset_purchase_list )
{
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	double sum_extension;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return 0.0;

	sum_extension = 0.0;

	do {
		purchase_fixed_asset = list_get( fixed_asset_purchase_list );
		sum_extension += purchase_fixed_asset->extension;

	} while( list_next( fixed_asset_purchase_list ) );

	return sum_extension;
} /* purchase_get_sum_fixed_asset_extension() */

char *purchase_fixed_asset_get_update_sys_string(
				char *application_name )
{
	static char sys_string[ 256 ];
	char *table_name;
	char *key;

	table_name = get_table_name( application_name, "fixed_asset_purchase" );
	key =
"full_name,street_address,purchase_date_time,asset_name,serial_number";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	return sys_string;

} /* purchase_fixed_asset_get_update_sys_string() */

char *purchase_order_get_update_sys_string(
				char *application_name )
{
	static char sys_string[ 256 ];
	char *table_name;
	char *key;

	table_name = get_table_name( application_name, "purchase_order" );
	key = "full_name,street_address,purchase_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	return sys_string;

} /* purchase_order_get_update_sys_string() */

char *purchase_vendor_payment_get_update_sys_string(
				char *application_name )
{
	static char sys_string[ 256 ];
	char *table_name;
	char *key;

	table_name = get_table_name( application_name, "vendor_payment" );
	key = "full_name,street_address,purchase_date_time,payment_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	return sys_string;

} /* purchase_vendor_payment_get_update_sys_string() */

void purchase_order_update(	char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				double sum_extension,
				double database_sum_extension,
				double purchase_amount,
				double database_purchase_amount,
				double amount_due,
				double database_amount_due,
				char *transaction_date_time,
				char *database_transaction_date_time,
				char *arrived_date_time,
				char *database_arrived_date_time,
				char *shipped_date,
				char *database_shipped_date )
{
	char *sys_string;
	FILE *output_pipe;

	sys_string = purchase_order_get_update_sys_string( application_name );
	output_pipe = popen( sys_string, "w" );

	if ( !double_virtually_same(
			sum_extension,
			database_sum_extension ) )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^sum_extension^%.2lf\n",
			full_name,
			street_address,
			purchase_date_time,
			sum_extension );
	}

	if ( !dollar_virtually_same(
			purchase_amount,
			database_purchase_amount ) )
	{
		fprintf(output_pipe,
	 		"%s^%s^%s^purchase_amount^%.2lf\n",
			full_name,
			street_address,
			purchase_date_time,
			purchase_amount );
	}

	if ( !dollar_virtually_same(
			amount_due,
			database_amount_due ) )
	{
		fprintf(output_pipe,
	 		"%s^%s^%s^amount_due^%.2lf\n",
			full_name,
			street_address,
			purchase_date_time,
			amount_due );
	}

	if ( timlib_strcmp(	transaction_date_time,
				database_transaction_date_time ) != 0 )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^transaction_date_time^%s\n",
			full_name,
			street_address,
			purchase_date_time,
			(transaction_date_time)
				? transaction_date_time
				: "" );
	}

	if ( timlib_strcmp(	arrived_date_time,
				database_arrived_date_time ) != 0 )
	{
		fprintf(output_pipe,
	 		"%s^%s^%s^arrived_date_time^%s\n",
	 		full_name,
	 		street_address,
	 		purchase_date_time,
	 		(arrived_date_time)
	 			? arrived_date_time
				: "" );
	}

	if ( timlib_strcmp(	shipped_date,
				database_shipped_date ) != 0 )
	{
		fprintf(output_pipe,
	 		"%s^%s^%s^shipped_date^%s\n",
	 		full_name,
	 		street_address,
	 		purchase_date_time,
	 		(shipped_date)
				? shipped_date
				: "" );
	}

	pclose( output_pipe );

} /* purchase_order_update() */

void purchase_order_arrived_date_time_update(
				PURCHASE_ORDER *purchase_order,
				char *application_name )
{
	char *sys_string;
	FILE *output_pipe;

	if ( !purchase_order ) return;

	sys_string = purchase_order_get_update_sys_string( application_name );
	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^arrived_date_time^%s\n",
		purchase_order->full_name,
		purchase_order->street_address,
		purchase_order->purchase_date_time,
		(purchase_order->arrived_date_time)
			? purchase_order->arrived_date_time
			: "" );

	pclose( output_pipe );

} /* purchase_order_arrived_date_time_update() */

void purchase_fixed_asset_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			double accumulated_depreciation,
			double database_accumulated_depreciation )
{
	char *sys_string;
	FILE *output_pipe;

	if ( double_virtually_same(
			accumulated_depreciation,
			database_accumulated_depreciation ) )
	{
		return;
	}

	sys_string =
		purchase_fixed_asset_get_update_sys_string(
			application_name );

	output_pipe = popen( sys_string, "w" );

	fprintf(output_pipe,
		"%s^%s^%s^%s^%s^accumulated_depreciation^%.2lf\n",
		full_name,
		street_address,
		purchase_date_time,
		asset_name,
		serial_number,
		accumulated_depreciation );

	pclose( output_pipe );

} /* purchase_fixed_asset_update() */

void purchase_vendor_payment_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time,
			char *transaction_date_time,
			char *database_transaction_date_time )
{
	char *sys_string;
	FILE *output_pipe;

	if ( timlib_strcmp(	transaction_date_time,
				database_transaction_date_time ) == 0 )
	{
		return;
	}

	sys_string =
		purchase_vendor_payment_get_update_sys_string(
			application_name );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^%s^transaction_date_time^%s\n",
		full_name,
		street_address,
		purchase_date_time,
		payment_date_time,
		(transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( output_pipe );

} /* purchase_vendor_payment_update() */

void purchase_order_shipped_date_update(
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *shipped_date,
				char *application_name )
{
	char *sys_string;
	FILE *output_pipe;

	sys_string = purchase_order_get_update_sys_string( application_name );
	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^shipped_date^%s\n",
		full_name,
		street_address,
		purchase_date_time,
		(shipped_date)
			? shipped_date
			: "" );

	pclose( output_pipe );

} /* purchase_order_shipped_date_update() */

void purchase_order_transaction_date_time_update(
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *transaction_date_time,
				char *application_name )
{
	char *sys_string;
	FILE *output_pipe;

	sys_string = purchase_order_get_update_sys_string( application_name );
	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^transaction_date_time^%s\n",
		full_name,
		street_address,
		purchase_date_time,
		(transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( output_pipe );

} /* purchase_order_transaction_date_time_update() */

/* Returns propagate_account_list */
/* ------------------------------ */
LIST *purchase_order_journal_ledger_refresh(
					char *application_name,
					char *fund_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double sum_inventory_extension,
					double sum_specific_inventory_unit_cost,
					double sum_supply_extension,
					double sum_service_extension,
					double sum_fixed_asset_extension,
					double sales_tax,
					double freight_in,
					double purchase_amount,
					LIST *inventory_purchase_list,
					LIST *supply_purchase_list,
					LIST *service_purchase_list,
					LIST *fixed_asset_purchase_list )
{
	char *sales_tax_expense_account = {0};
	char *freight_in_expense_account = {0};
	char *account_payable_account = {0};
	char *cash_account = {0};
	char *uncleared_checks_account = {0};
	LIST *propagate_account_list;
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;

	propagate_account_list = list_new();

	ledger_delete(			application_name,
					LEDGER_FOLDER_NAME,
					full_name,
					street_address,
					transaction_date_time );

	ledger_get_purchase_order_account_names(
		&sales_tax_expense_account,
		&freight_in_expense_account,
		&account_payable_account,
		&cash_account,
		&uncleared_checks_account,
		application_name,
		fund_name );

	if ( sum_specific_inventory_unit_cost )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: sum_specific_inventory_unit_cost is not yet implemented.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( sum_inventory_extension )
	{
		list_append_list(
			propagate_account_list,
			purchase_inventory_journal_ledger_refresh(
				application_name,
				full_name,
				street_address,
				transaction_date_time,
				inventory_purchase_list ) );
	}

	if ( sum_supply_extension )
	{
		list_append_list(
			propagate_account_list,
			purchase_supply_journal_ledger_refresh(
				application_name,
				full_name,
				street_address,
				transaction_date_time,
				sum_supply_extension,
				supply_purchase_list ) );
	}

	if ( sum_service_extension )
	{
		list_append_list(
			propagate_account_list,
			purchase_service_journal_ledger_refresh(
				application_name,
				full_name,
				street_address,
				transaction_date_time,
				service_purchase_list ) );
	}

	if ( sum_fixed_asset_extension )
	{
		list_append_list(
			propagate_account_list,
			purchase_fixed_asset_journal_ledger_refresh(
				application_name,
				full_name,
				street_address,
				transaction_date_time,
				sum_fixed_asset_extension,
				fixed_asset_purchase_list ) );
	}

	if ( sales_tax_expense_account && sales_tax )
	{
		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			sales_tax_expense_account,
			sales_tax,
			1 /* is_debit */ );

		account = ledger_account_new( sales_tax_expense_account );

		prior_ledger = ledger_get_prior_ledger(
					application_name,
					transaction_date_time,
					sales_tax_expense_account );

		account->journal_ledger_list =
			ledger_get_propagate_journal_ledger_list(
				application_name,
				prior_ledger,
				sales_tax_expense_account );

		list_append_pointer( propagate_account_list, account );
	}

	if ( freight_in_expense_account && freight_in )
	{
		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			freight_in_expense_account,
			freight_in,
			1 /* is_debit */ );

		account = ledger_account_new( freight_in_expense_account );

		prior_ledger = ledger_get_prior_ledger(
					application_name,
					transaction_date_time,
					freight_in_expense_account );

		account->journal_ledger_list =
			ledger_get_propagate_journal_ledger_list(
				application_name,
				prior_ledger,
				freight_in_expense_account );

		list_append_pointer( propagate_account_list, account );
	}

	if ( account_payable_account && purchase_amount )
	{
		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			account_payable_account,
			purchase_amount,
			0 /* not is_debit */ );

		account = ledger_account_new( account_payable_account );

		prior_ledger = ledger_get_prior_ledger(
					application_name,
					transaction_date_time,
					account_payable_account );

		account->journal_ledger_list =
			ledger_get_propagate_journal_ledger_list(
				application_name,
				prior_ledger,
				account_payable_account );

		list_append_pointer( propagate_account_list, account );
	}

	return propagate_account_list;

} /* purchase_order_journal_ledger_refresh() */

/* Returns propagate_account_list */
/* ------------------------------ */
LIST *purchase_vendor_payment_journal_ledger_refresh(
					char *application_name,
					char *fund_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double payment_amount,
					int check_number )
{
	char *checking_account = {0};
	char *uncleared_checks_account = {0};
	char *account_payable_account = {0};
	LIST *propagate_account_list = {0};
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;
	char *credit_account_name;

	ledger_delete(			application_name,
					LEDGER_FOLDER_NAME,
					full_name,
					street_address,
					transaction_date_time );

	ledger_get_vendor_payment_account_names(
		&checking_account,
		&uncleared_checks_account,
		&account_payable_account,
		application_name,
		fund_name );

	if ( !account_payable_account || !checking_account || !payment_amount )
	{
		return (LIST *)0;
	}

	ledger_journal_ledger_insert(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		account_payable_account,
		payment_amount,
		1 /* is_debit */ );

	if ( uncleared_checks_account && check_number )
		credit_account_name = uncleared_checks_account;
	else
		credit_account_name = checking_account;

	ledger_journal_ledger_insert(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		credit_account_name,
		payment_amount,
		0 /* not is_debit */ );

	propagate_account_list = list_new();

	account = ledger_account_new( account_payable_account );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				account_payable_account );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			account_payable_account );

	list_append_pointer( propagate_account_list, account );

	account = ledger_account_new( credit_account_name );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				credit_account_name );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			credit_account_name );

	list_append_pointer( propagate_account_list, account );

	return propagate_account_list;

} /* purchase_vendor_payment_journal_ledger_refresh() */

char *purchase_get_arrived_purchase_date_time(
				char *application_name,
				char *arrived_date_time )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	if ( !arrived_date_time || !*arrived_date_time ) return (char *)0;

	sprintf( where,
		 "arrived_date_time = '%s'",
		 arrived_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=purchase_date_time	"
		 "			folder=purchase_order		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* purchase_get_arrived_purchase_date_time() */
 
char *purchase_order_fetch_arrived_date_time(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time )
{

	char sys_string[ 1024 ];
	char *where;

	where = ledger_get_transaction_where(
			full_name,
			street_address,
			purchase_date_time,
			"purchase_order",
			"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=arrived_date_time	"
		 "			folder=purchase_order		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* purchase_order_fetch_arrived_date_time() */

char *purchase_order_fetch_transaction_date_time(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time )
{

	char sys_string[ 1024 ];
	char *where;

	where = ledger_get_transaction_where(
			full_name,
			street_address,
			purchase_date_time,
			"purchase_order",
			"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=transaction_date_time	"
		 "			folder=purchase_order		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* purchase_order_fetch_transaction_date_time() */

PURCHASE_ORDER *purchase_order_seek(
				LIST *purchase_order_list,
				char *purchase_date_time )
{
	PURCHASE_ORDER *purchase_order;

	if ( !list_rewind( purchase_order_list ) )
		return (PURCHASE_ORDER *)0;

	do {
		purchase_order =
			list_get( 
				purchase_order_list );

		if ( strcmp(	purchase_order->purchase_date_time,
				purchase_date_time ) == 0 )
		{
			return purchase_order;
		}

	} while( list_next( purchase_order_list ) );

	return (PURCHASE_ORDER *)0;

} /* purchase_order_seek() */

char *purchase_order_get_inventory_purchase_join_where( void )
{
	char *join_where;

	join_where =	"purchase_order.full_name =			"
			"	inventory_purchase.full_name and	"
			"purchase_order.street_address =		"
			"	inventory_purchase.street_address and	"
			"purchase_order.purchase_date_time =		"
			"	inventory_purchase.purchase_date_time	";

	return join_where;

} /* purchase_order_get_inventory_purchase_join_where() */

LIST *purchase_get_inventory_purchase_list(
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				HASH_TABLE *inventory_purchase_hash_table,
				LIST *inventory_purchase_name_list )
{
	char *inventory_name;
	INVENTORY_PURCHASE *inventory_purchase;
	LIST *inventory_purchase_list = {0};
	char *key;

	if ( !list_rewind( inventory_purchase_name_list ) )
		return (LIST *)0;

	do {
		inventory_name = list_get( inventory_purchase_name_list );

		key = inventory_purchase_get_hash_table_key(
				full_name,
				street_address,
				purchase_date_time,
				inventory_name );

		if ( ( inventory_purchase =
			hash_table_fetch(
				inventory_purchase_hash_table,
				key ) ) )
		{
			if ( !inventory_purchase_list )
				inventory_purchase_list = list_new();

			list_append_pointer(
				inventory_purchase_list,
				inventory_purchase );
		}

	} while( list_next( inventory_purchase_name_list ) );

	return inventory_purchase_list;

} /* purchase_get_inventory_purchase_list() */

LIST *purchase_get_inventory_purchase_order_list(
			char *application_name,
			char *inventory_name,
			HASH_TABLE *inventory_purchase_hash_table,
			LIST *inventory_purchase_name_list,
			HASH_TABLE *transaction_hash_table,
			HASH_TABLE *journal_ledger_hash_table )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *inventory_subquery;
	char *select;
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	PURCHASE_ORDER *purchase_order;
	LIST *purchase_order_list = {0};

	select = purchase_order_get_select();

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"purchase_order"     /* one2m_folder_name */,
			"inventory_purchase" /* mto1_folder_name */,
			"purchase_date_time" /* foreign_attribute_name */ );

	sprintf(	where,
			"%s and arrived_date_time is not null",
			inventory_subquery );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=purchase_order		"
		 "			where=\"%s\"			"
		 "			order=arrived_date_time		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		purchase_order = purchase_order_calloc();

		purchase_order_parse(
			&purchase_order->full_name,
			&purchase_order->street_address,
			&purchase_order->purchase_date_time,
			&purchase_order->sum_extension,
			&purchase_order->database_sum_extension,
			&purchase_order->sales_tax,
			&purchase_order->freight_in,
			&purchase_order->purchase_amount,
			&purchase_order->database_purchase_amount,
			&purchase_order->amount_due,
			&purchase_order->database_amount_due,
			&purchase_order->title_passage_rule,
			&purchase_order->shipped_date,
			&purchase_order->database_shipped_date,
			&purchase_order->arrived_date_time,
			&purchase_order->database_arrived_date_time,
			&purchase_order->transaction_date_time,
			&purchase_order->database_transaction_date_time,
			&purchase_order->fund_name,
			input_buffer );

		purchase_order->inventory_purchase_list =
			purchase_get_inventory_purchase_list(
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->purchase_date_time,
				inventory_purchase_hash_table,
				inventory_purchase_name_list );

		purchase_order->sum_inventory_extension =
			inventory_purchase_list_set_extension(
				purchase_order->inventory_purchase_list );

		inventory_purchase_list_set_capitalized_unit_cost(
			purchase_order->inventory_purchase_list,
			purchase_order->sum_inventory_extension,
			purchase_order->sales_tax,
			purchase_order->freight_in );

/*
		purchase_order->supply_purchase_list =
			purchase_supply_get_list(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->purchase_date_time );

		purchase_order->service_purchase_list =
			purchase_service_get_list(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->purchase_date_time );

		purchase_order->fixed_asset_purchase_list =
			purchase_fixed_asset_get_list(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->purchase_date_time );
*/

		purchase_order->purchase_amount =
			purchase_order_get_purchase_amount(
				&purchase_order->sum_inventory_extension,
				&purchase_order->
					sum_specific_inventory_unit_cost,
				&purchase_order->sum_supply_extension,
				&purchase_order->sum_service_extension,
				&purchase_order->sum_fixed_asset_extension,
				&purchase_order->sum_extension,
				purchase_order->inventory_purchase_list,
				purchase_order->
					specific_inventory_purchase_list,
				purchase_order->supply_purchase_list,
				purchase_order->service_purchase_list,
				purchase_order->fixed_asset_purchase_list,
				purchase_order->sales_tax,
				purchase_order->freight_in );

		if ( purchase_order->transaction_date_time )
		{
			purchase_order->transaction =
				ledger_purchase_build_transaction(
					application_name,
					purchase_order->fund_name,
					purchase_order->full_name,
					purchase_order->street_address,
					purchase_order->transaction_date_time,
					transaction_hash_table,
					journal_ledger_hash_table );
		}

		if ( !purchase_order_list )
			purchase_order_list = list_new();

		list_append_pointer( purchase_order_list, purchase_order );
	}

	pclose( input_pipe );
	return purchase_order_list;

} /* purchase_get_inventory_purchase_order_list() */

boolean purchase_inventory_is_latest(
				char *application_name,
				char *inventory_name,
				char *purchase_date_time )
{
	INVENTORY *inventory;

	inventory = inventory_load_new( application_name, inventory_name );

	inventory->
		last_inventory_balance->
		inventory_purchase =
			inventory_get_last_inventory_purchase(
				application_name,
				inventory_name );

	/* If deleted the last inventory purchase. */
	/* --------------------------------------- */
	if ( !inventory->last_inventory_balance->inventory_purchase )
		return 0;

	inventory->
		last_inventory_balance->
		inventory_sale =
			inventory_get_last_inventory_sale(
				application_name,
				inventory_name );

	return inventory_is_latest_purchase(
			inventory->
				last_inventory_balance->
				inventory_purchase->
				purchase_date_time,
			inventory->
				last_inventory_balance->
				inventory_sale,
			inventory->
				last_inventory_balance->
				inventory_purchase->
				arrived_date_time,
			purchase_date_time );

} /* purchase_inventory_is_latest() */

void purchase_order_transaction_delete_with_propagate(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	entity_propagate_purchase_order_ledger_accounts(
		application_name,
		fund_name,
		transaction_date_time );

} /* purchase_order_transaction_delete_with_propagate() */

double purchase_get_sum_payment_amount(
				LIST *vendor_payment_list )
{
	VENDOR_PAYMENT *vendor_payment;
	double sum_payment_amount;

	if ( !list_rewind( vendor_payment_list ) ) return 0.0;

	sum_payment_amount = 0.0;

	do {
		vendor_payment = list_get( vendor_payment_list );

		sum_payment_amount += vendor_payment->payment_amount;

	} while( list_next( vendor_payment_list ) );

	return sum_payment_amount;

} /* purchase_get_sum_payment_amount() */

VENDOR_PAYMENT *purchase_vendor_payment_new(
					char *payment_date_time )
{
	VENDOR_PAYMENT *p =
		(VENDOR_PAYMENT *)
			calloc( 1, sizeof( VENDOR_PAYMENT ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	p->payment_date_time = payment_date_time;
	return p;

} /* purchase_vendor_payment_new() */


VENDOR_PAYMENT *purchase_vendor_payment_seek(
				LIST *vendor_payment_list,
				char *payment_date_time )
{
	VENDOR_PAYMENT *vendor_payment;

	if ( !list_rewind( vendor_payment_list ) ) return (VENDOR_PAYMENT *)0;

	do {
		vendor_payment = list_get( vendor_payment_list );

		if ( timlib_strcmp(	vendor_payment->payment_date_time,
					payment_date_time ) == 0 )
		{
			return vendor_payment;
		}

	} while( list_next( vendor_payment_list ) );

	return (VENDOR_PAYMENT *)0;

} /* purchase_vendor_payment_seek() */

LIST *purchase_get_vendor_payment_list(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	VENDOR_PAYMENT *vendor_payment;
	LIST *payment_list;

	select =
	"payment_date_time,payment_amount,check_number,transaction_date_time";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=vendor_payment		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );
	payment_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		vendor_payment =
			purchase_vendor_payment_new(
				strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		if ( *piece_buffer )
			vendor_payment->payment_amount = atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		if ( *piece_buffer )
			vendor_payment->check_number = atoi( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		if ( *piece_buffer )
		{
			vendor_payment->transaction_date_time =
			vendor_payment->database_transaction_date_time =
				strdup( piece_buffer );

			vendor_payment->transaction =
				ledger_transaction_with_load_new(
					application_name,
					full_name,
					street_address,
					vendor_payment->transaction_date_time );
		}

		list_append_pointer( payment_list, vendor_payment );
	}

	pclose( input_pipe );
	return payment_list;

} /* purchase_get_vendor_payment_list() */

PURCHASE_SERVICE *purchase_service_list_seek(
			LIST *service_purchase_list,
			char *account )
{
	PURCHASE_SERVICE *purchase_service;

	if ( !list_rewind( service_purchase_list ) )
		return (PURCHASE_SERVICE *)0;

	do {
		purchase_service = list_get( service_purchase_list );

		if ( strcmp(	purchase_service->account_name,
				account ) == 0 )
		{
			return purchase_service;
		}

	} while( list_next( service_purchase_list ) );

	return (PURCHASE_SERVICE *)0;

} /* purchase_service_list_seek() */

PURCHASE_FIXED_ASSET *purchase_fixed_asset_list_seek(
				LIST *fixed_asset_purchase_list,
				char *asset_name,
				char *serial_number )
{
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;

	if ( !list_rewind( fixed_asset_purchase_list ) )
		return (PURCHASE_FIXED_ASSET *)0;

	do {
		purchase_fixed_asset = list_get( fixed_asset_purchase_list );

		if ( strcmp(	purchase_fixed_asset->asset_name,
				asset_name ) == 0
		&&   strcmp(	purchase_fixed_asset->serial_number,
				serial_number ) == 0 )
		{
			return purchase_fixed_asset;
		}

	} while( list_next( fixed_asset_purchase_list ) );

	return (PURCHASE_FIXED_ASSET *)0;

} /* purchase_fixed_asset_list_seek() */

PURCHASE_SUPPLY *purchase_supply_list_seek(
			LIST *supply_purchase_list,
			char *supply_name )
{
	PURCHASE_SUPPLY *purchase_supply;

	if ( !list_rewind( supply_purchase_list ) )
		return (PURCHASE_SUPPLY *)0;

	do {
		purchase_supply = list_get( supply_purchase_list );

		if ( strcmp(	purchase_supply->supply_name,
				supply_name ) == 0 )
		{
			return purchase_supply;
		}

	} while( list_next( supply_purchase_list ) );

	return (PURCHASE_SUPPLY *)0;

} /* purchase_supply_list_seek() */

void purchase_supply_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *supply_name,
				double extension,
				double database_extension )
{
	FILE *update_pipe;
	char *table_name;
	char *key_column_list_string;
	char sys_string[ 1024 ];

	if ( dollar_virtually_same( extension, database_extension ) )
	{
		return;
	}

	table_name = get_table_name( application_name, "supply_purchase" );

	key_column_list_string =
		"full_name,street_address,purchase_date_time,supply_name";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	update_pipe = popen( sys_string, "w" );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^extension^%.2lf\n",
	 	full_name,
	 	street_address,
	 	purchase_date_time,
	 	supply_name,
	 	extension );

	pclose( update_pipe );

} /* purchase_supply_update() */

LIST *purchase_inventory_journal_ledger_refresh(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					LIST *inventory_purchase_list )
{
	LIST *propagate_account_list;
	INVENTORY_PURCHASE *inventory_purchase;
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;

	propagate_account_list = list_new();

	if ( list_rewind( inventory_purchase_list ) )
	{
		do {
			inventory_purchase =
				list_get_pointer(
					inventory_purchase_list );

			ledger_journal_ledger_insert(
				application_name,
				full_name,
				street_address,
				transaction_date_time,
				inventory_purchase->
					inventory_account_name,
				inventory_purchase->extension,
				1 /* is_debit */ );

			account =
				ledger_account_new(
					inventory_purchase->
						inventory_account_name );

			prior_ledger =
				ledger_get_prior_ledger(
					application_name,
					transaction_date_time,
					account->account_name );

			account->journal_ledger_list =
				ledger_get_propagate_journal_ledger_list(
					application_name,
					prior_ledger,
					account->account_name );

			list_append_pointer( propagate_account_list, account );

		} while( list_next( inventory_purchase_list ) );
	}

	return propagate_account_list;

} /* purchase_inventory_journal_ledger_refresh() */

LIST *purchase_supply_journal_ledger_refresh(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double sum_supply_extension,
					LIST *supply_purchase_list )
{
	LIST *propagate_account_list;
	PURCHASE_SUPPLY *purchase_supply;
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;
	char *specific_supply_expense_account;

	propagate_account_list = list_new();

	if ( list_rewind( supply_purchase_list ) )
	{
		do {
			purchase_supply = list_get( supply_purchase_list );

			if ( ! ( specific_supply_expense_account =
				ledger_get_supply_expense_account(
					application_name,
					purchase_supply->supply_name ) )
			||    !*specific_supply_expense_account )
			{
				fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get supply_expense_account.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			ledger_journal_ledger_insert(
				application_name,
				full_name,
				street_address,
				transaction_date_time,
				specific_supply_expense_account,
				purchase_supply->extension,
				1 /* is_debit */ );

			account =
				ledger_account_new(
					specific_supply_expense_account );

			prior_ledger =
				ledger_get_prior_ledger(
					application_name,
					transaction_date_time,
					specific_supply_expense_account );

			account->journal_ledger_list =
				ledger_get_propagate_journal_ledger_list(
					application_name,
					prior_ledger,
					specific_supply_expense_account );

			list_append_pointer( propagate_account_list, account );

			sum_supply_extension -= purchase_supply->extension;

		} while( list_next( supply_purchase_list ) );
	}

	return propagate_account_list;

} /* purchase_supply_journal_ledger_refresh() */

LIST *purchase_service_journal_ledger_refresh(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					LIST *service_purchase_list )
{
	LIST *propagate_account_list;
	PURCHASE_SERVICE *purchase_service;
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;

	if ( !list_rewind( service_purchase_list ) )
		return (LIST *)0;

	propagate_account_list = list_new();

	do {
		purchase_service = list_get( service_purchase_list );

		ledger_journal_ledger_insert(
				application_name,
				full_name,
				street_address,
				transaction_date_time,
				purchase_service->account_name,
				purchase_service->extension,
				1 /* is_debit */ );

		account = ledger_account_new( purchase_service->account_name );

		prior_ledger =
			ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				purchase_service->account_name );

		account->journal_ledger_list =
			ledger_get_propagate_journal_ledger_list(
				application_name,
				prior_ledger,
				purchase_service->account_name );

		list_append_pointer( propagate_account_list, account );

	} while( list_next( service_purchase_list ) );

	return propagate_account_list;

} /* purchase_service_journal_ledger_refresh() */

LIST *purchase_fixed_asset_journal_ledger_refresh(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double sum_fixed_asset_extension,
					LIST *fixed_asset_purchase_list )
{
	LIST *propagate_account_list;
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;
	PURCHASE_FIXED_ASSET_ACCOUNT *purchase_fixed_asset_account;
	LIST *fixed_asset_account_list;

	if ( timlib_dollar_virtually_same( sum_fixed_asset_extension, 0.0 ) )
		return (LIST *)0;

	if ( !list_length( fixed_asset_purchase_list ) )
		return (LIST *)0;

	fixed_asset_account_list =
		purchase_get_fixed_asset_account_list(
					application_name,
					fixed_asset_purchase_list );

	if ( !list_rewind( fixed_asset_account_list ) ) return (LIST *)0;

	propagate_account_list = list_new();

	do {
		purchase_fixed_asset_account =
			list_get( fixed_asset_account_list );

		if ( !purchase_fixed_asset_account->account_name )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: empty fixed_asset_account->account.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			purchase_fixed_asset_account->account_name,
			purchase_fixed_asset_account->debit_amount,
			1 /* is_debit */ );

		account =
			ledger_account_new(
				purchase_fixed_asset_account->account_name );

		prior_ledger =
			ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				purchase_fixed_asset_account->account_name );

		account->journal_ledger_list =
			ledger_get_propagate_journal_ledger_list(
				application_name,
				prior_ledger,
				purchase_fixed_asset_account->account_name );

		list_append_pointer( propagate_account_list, account );

	} while( list_next( fixed_asset_purchase_list ) );

	return propagate_account_list;

} /* purchase_fixed_asset_journal_ledger_refresh() */

double purchase_order_get_amount_due(	char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time )
{
	char *table_name;
	char sys_string[ 1024 ];
	char *select;
	char *where;
	char *results_string;

	select = "amount_due";

	table_name = get_table_name( application_name, "purchase_order" );

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( sys_string,
		 "echo \"	select %s			 "
		 "		from %s				 "
		 "		where %s;\"			|"
		 "sql.e						 ",
		 select,
		 table_name,
		 where );

	results_string = pipe2string( sys_string );

	if ( !results_string )
		return 0.0;
	else
		return atof( results_string );

} /* purchase_order_get_amount_due() */

double purchase_order_get_total_payment(	char *application_name,
						char *full_name,
						char *street_address,
						char *purchase_date_time )
{
	char *table_name;
	char sys_string[ 1024 ];
	char *where;
	char *results_string;

	table_name = get_table_name( application_name, "vendor_payment" );

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( sys_string,
		 "echo \"	select sum( payment_amount )	 "
		 "		from %s				 "
		 "		where %s;\"			|"
		 "sql.e						 ",
		 table_name,
		 where );

	results_string = pipe2string( sys_string );

	if ( !results_string )
		return 0.0;
	else
		return atof( results_string );

} /* purchase_order_get_total_payment() */

void purchase_fixed_asset_list_depreciation_delete(
				LIST *purchase_fixed_asset_list,
				char *application_name,
				char *fund_name )
{
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;

	if ( !list_rewind( purchase_fixed_asset_list ) ) return;

	do {
		purchase_fixed_asset = list_get( purchase_fixed_asset_list );

		purchase_fixed_asset_depreciation_delete(
			purchase_fixed_asset,
			application_name,
			fund_name );

	} while( list_next( purchase_fixed_asset_list ) );

} /* purchase_fixed_asset_list_depreciation_delete() */

void purchase_fixed_asset_list_depreciation_method_update(
			LIST *purchase_fixed_asset_list,
			char *arrived_date_time,
			char *application_name,
			char *fund_name )
{
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;

	if ( !list_rewind( purchase_fixed_asset_list ) ) return;

	do {
		purchase_fixed_asset = list_get( purchase_fixed_asset_list );

		purchase_fixed_asset_depreciation_propagate(
			purchase_fixed_asset,
			arrived_date_time,
			application_name,
			fund_name );

	} while( list_next( purchase_fixed_asset_list ) );

} /* purchase_fixed_asset_list_depreciation_method_update() */

void purchase_fixed_asset_depreciation_delete(
				PURCHASE_FIXED_ASSET *purchase_fixed_asset,
				char *application_name,
				char *fund_name )
{
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

	if ( !list_length( purchase_fixed_asset->depreciation_list ) ) return;

	depreciation_list_delete(
		purchase_fixed_asset->depreciation_list,
		application_name,
		fund_name );

	purchase_fixed_asset_update(
			application_name,
			purchase_fixed_asset->full_name,
			purchase_fixed_asset->street_address,
			purchase_fixed_asset->purchase_date_time,
			purchase_fixed_asset->asset_name,
			purchase_fixed_asset->serial_number,
			0.0 /* accumulated_depreciation */,
			purchase_fixed_asset->
				database_accumulated_depreciation );

} /* purchase_fixed_asset_depreciation_delete() */

void purchase_fixed_asset_depreciation_propagate(
			PURCHASE_FIXED_ASSET *purchase_fixed_asset,
			char *arrived_date_time,
			char *application_name,
			char *fund_name )
{
	char arrived_date[ 16 ];

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

	if ( !list_length( purchase_fixed_asset->depreciation_list ) ) return;

	purchase_fixed_asset->accumulated_depreciation =
		depreciation_list_set(
			purchase_fixed_asset->depreciation_list,
			purchase_fixed_asset->depreciation_method,
			purchase_fixed_asset->extension,
			purchase_fixed_asset->estimated_residual_value,
			purchase_fixed_asset->estimated_useful_life_years,
			purchase_fixed_asset->estimated_useful_life_units,
			purchase_fixed_asset->declining_balance_n,
			column( arrived_date,
				0, 
				arrived_date_time ) );

	depreciation_list_update(
		purchase_fixed_asset->depreciation_list,
		application_name,
		fund_name );

	purchase_fixed_asset_update(
			application_name,
			purchase_fixed_asset->full_name,
			purchase_fixed_asset->street_address,
			purchase_fixed_asset->purchase_date_time,
			purchase_fixed_asset->asset_name,
			purchase_fixed_asset->serial_number,
			purchase_fixed_asset->accumulated_depreciation,
			purchase_fixed_asset->
				database_accumulated_depreciation );

} /* purchase_fixed_asset_depreciation_propagate() */

PURCHASE_FIXED_ASSET_ACCOUNT *purchase_fixed_asset_account_new(
					char *account_name )
{
	PURCHASE_FIXED_ASSET_ACCOUNT *p =
		(PURCHASE_FIXED_ASSET_ACCOUNT *)
			calloc( 1, sizeof( PURCHASE_FIXED_ASSET_ACCOUNT ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	p->account_name = account_name;
	return p;

} /* purchase_fixed_asset_account_new() */

LIST *purchase_get_fixed_asset_account_list(
					char *application_name,
					LIST *fixed_asset_purchase_list )
{
	LIST *fixed_asset_account_name_list;
	LIST *fixed_asset_account_list = {0};
	PURCHASE_FIXED_ASSET_ACCOUNT *purchase_fixed_asset_account;
	char *fixed_asset_account_name;
	double debit_amount;

	if ( !list_length( fixed_asset_purchase_list ) ) return (LIST *)0;

	fixed_asset_account_name_list =
		purchase_get_fixed_asset_account_name_list(
			application_name );

	if ( !list_rewind( fixed_asset_account_name_list ) ) return (LIST *)0;

	do {
		fixed_asset_account_name =
			list_get( fixed_asset_account_name_list );

		debit_amount =
			purchase_get_fixed_asset_account_debit_amount(
				fixed_asset_purchase_list,
				fixed_asset_account_name );

		if ( debit_amount )
		{
			purchase_fixed_asset_account =
				purchase_fixed_asset_account_new(
					fixed_asset_account_name );

			purchase_fixed_asset_account->debit_amount =
				debit_amount;

			if ( !fixed_asset_account_list )
				fixed_asset_account_list = list_new();

			list_append_pointer(	fixed_asset_account_list,
						purchase_fixed_asset_account );
		}

	} while( list_next( fixed_asset_account_name_list ) );

	return fixed_asset_account_list;

} /* purchase_get_fixed_asset_account_list() */

LIST *purchase_get_fixed_asset_account_name_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	char *where;

	where = "subclassification = 'fixed_asset'";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=account		"
		 "			folder=account		"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	return pipe2list( sys_string );

} /* purchase_get_fixed_asset_account_name_list() */

double purchase_get_fixed_asset_account_debit_amount(
				LIST *fixed_asset_purchase_list,
				char *fixed_asset_account_name )
{
	double debit_amount;
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return 0.0;

	debit_amount = 0.0;

	do {
		purchase_fixed_asset =
			list_get( fixed_asset_purchase_list );

		if ( !purchase_fixed_asset->account_name )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty account for fixed_asset = %s.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				purchase_fixed_asset->asset_name );
			exit( 1 );
		}

		if ( timlib_strcmp(	purchase_fixed_asset->
						account_name,
					fixed_asset_account_name ) == 0 )
		{
			debit_amount += purchase_fixed_asset->extension;
		}

	} while( list_next( fixed_asset_purchase_list ) );

	return debit_amount;

} /* purchase_get_fixed_asset_account_debit_amount() */

