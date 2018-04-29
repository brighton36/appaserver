/* ---------------------------------------------------------------	*/
/* src_autorepair/post_change_inventory_order.c				*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "inventory.h"
#include "appaserver_library.h"
#include "appaserver_error.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
char *build_part_order_additional_where(
				char *order_date_string,
				char *order_time_string,
				char *customer_name,
				char *vehicle_make,
				char *vehicle_model,
				char *vehicle_trim,
				int vehicle_year );

void post_change_inventory_reset_quantity_remaining(
				char *application_name,
				char *inventory_name,
				char *part_order_additional_where );

LIST *get_part_order_quantity_list(
				char *application_name,
				char *inventory_name,
				char *additional_where );

void update_part_order(
				char *application_name,
				char *order_date_string,
				char *order_time_string,
				char *customer_name,
				char *vehicle_make,
				char *vehicle_model,
				char *vehicle_trim,
				int vehicle_year,
				char *inventory_name,
				double extension,
				double cost_of_goods_sold );

void post_change_inventory_sale_insert(
				char *application_name,
				char *order_date_string,
				char *order_time_string,
				char *customer_name,
				char *vehicle_make,
				char *vehicle_model,
				char *vehicle_trim,
				int vehicle_year,
				char *inventory_name,
				int quantity,
				double retail_price,
				double discount_amount );

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};
	char *order_date_time;
	char *customer_name;
	char *vehicle_make;
	char *vehicle_model;
	char *vehicle_trim;
	int vehicle_year;
	char *inventory_name;
	int quantity;
	double retail_price;
	double discount_amount;
	char *state;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 14 )
	{
		fprintf( stderr, 
"Usage: %s ignored date time customer make model trim year inventory quantity retail discount state\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	order_date_string = argv[ 2 ];
	order_time_string = argv[ 3 ];
	customer_name = argv[ 4 ];
	vehicle_make = argv[ 5 ];
	vehicle_model = argv[ 6 ];
	vehicle_trim = argv[ 7 ];
	vehicle_year = atoi( argv[ 8 ] );
	inventory_name = argv[ 9 ];
	quantity = atoi( argv[ 10 ] );
	retail_price = atof( argv[ 11 ] );
	discount_amount = atof( argv[ 12 ] );
	state = argv[ 13 ];

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_inventory_sale_insert(
				application_name,
				order_date_string,
				order_time_string,
				customer_name,
				vehicle_make,
				vehicle_model,
				vehicle_trim,
				vehicle_year,
				inventory_name,
				quantity,
				retail_price,
				discount_amount );
	}
	if ( strcmp( state, "update" ) == 0 )
	{
		char *part_order_additional_where;

		part_order_additional_where =
			build_part_order_additional_where(
				order_date_string,
				order_time_string,
				customer_name,
				vehicle_make,
				vehicle_model,
				vehicle_trim,
				vehicle_year );

		post_change_inventory_reset_quantity_remaining(
				application_name,
				inventory_name,
				part_order_additional_where );

		post_change_inventory_sale_insert(
				application_name,
				order_date_string,
				order_time_string,
				customer_name,
				vehicle_make,
				vehicle_model,
				vehicle_trim,
				vehicle_year,
				inventory_name,
				quantity,
				retail_price,
				discount_amount );
	}
	else
	{
		post_change_inventory_reset_quantity_remaining(
				application_name,
				inventory_name,
				(char *)0 /* part_order_additional_where */ );
	}

	return 0;
} /* main() */

void post_change_inventory_sale_insert(
				char *application_name,
				char *order_date_string,
				char *order_time_string,
				char *customer_name,
				char *vehicle_make,
				char *vehicle_model,
				char *vehicle_trim,
				int vehicle_year,
				char *inventory_name,
				int quantity,
				double retail_price,
				double discount_amount )
{
	LIST *purchase_list;
	double extension;
	double cost_of_goods_sold;

	extension = ( (double)quantity * retail_price ) - discount_amount;

	purchase_list =
		inventory_get_quantity_remaining_purchase_list(
			application_name,
			inventory_name );

	cost_of_goods_sold =
		inventory_get_cost_of_goods_sold(
			purchase_list,
			quantity,
			inventory_fifo );

	if ( cost_of_goods_sold )
	{
		inventory_purchase_update_quantity_remaining_if_changed(
			application_name,
			inventory_name,
			purchase_list );
	}

	update_part_order(	application_name,
				order_date_string,
				order_time_string,
				customer_name,
				vehicle_make,
				vehicle_model,
				vehicle_trim,
				vehicle_year,
				inventory_name,
				extension,
				cost_of_goods_sold );

} /* post_change_inventory_sale_insert() */

void update_part_order(
				char *application_name,
				char *order_date_string,
				char *order_time_string,
				char *customer_name,
				char *vehicle_make,
				char *vehicle_model,
				char *vehicle_trim,
				int vehicle_year,
				char *inventory_name,
				double extension,
				double cost_of_goods_sold )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_name;
	char *key_column_list;

	table_name = get_table_name( application_name, "part_order" );
	key_column_list =
"order_date,order_time,customer_name,vehicle_make,vehicle_model,vehicle_trim,vehicle_year,inventory_name";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key_column_list );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s^%s^%s^%s^%s^%s^%d^%s^extension^%.2lf\n",
		 order_date_string,
		 order_time_string,
		 customer_name,
		 vehicle_make,
		 vehicle_model,
		 vehicle_trim,
		 vehicle_year,
		 inventory_name,
		 extension );

	fprintf( output_pipe,
		 "%s^%s^%s^%s^%s^%s^%d^%s^cost_of_goods_sold^%.2lf\n",
		 order_date_string,
		 order_time_string,
		 customer_name,
		 vehicle_make,
		 vehicle_model,
		 vehicle_trim,
		 vehicle_year,
		 inventory_name,
		 cost_of_goods_sold );

	pclose( output_pipe );

} /* update_part_order() */

void post_change_inventory_reset_quantity_remaining(
				char *application_name,
				char *inventory_name,
				char *part_order_additional_where )
{
	LIST *purchase_list;
	LIST *part_order_quantity_list;
	char *quantity_string;
	double total_cost_of_goods_sold = 0.0;

	purchase_list = inventory_get_purchase_list(
				application_name,
				inventory_name,
				(char *)0 /* additional_where */ );

	inventory_reset_quantity_remaining( purchase_list );

	part_order_quantity_list =
		get_part_order_quantity_list(
			application_name,
			inventory_name,
			part_order_additional_where );

	if ( list_rewind( part_order_quantity_list ) )
	{
		do {
			quantity_string =
				list_get_pointer(
					part_order_quantity_list );

			/* Reduce the quantity_remaining */
			/* ----------------------------- */
			total_cost_of_goods_sold +=
				inventory_get_cost_of_goods_sold(
					purchase_list,
					atoi( quantity_string ),
					inventory_fifo );

		} while( list_next( part_order_quantity_list ) );
	}

/*
	if ( total_cost_of_goods_sold )
	{
		inventory_purchase_update_quantity_remaining_if_changed(
			application_name,
			inventory_name,
			purchase_list );
	}
*/
	inventory_purchase_update_quantity_remaining_if_changed(
		application_name,
		inventory_name,
		purchase_list );

} /* post_change_inventory_reset_quantity_remaining() */

LIST *get_part_order_quantity_list(
			char *application_name,
			char *inventory_name,
			char *additional_where )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *order;

	if ( additional_where && *additional_where )
	{
		sprintf(	where,
				"inventory_name = '%s' and %s",
				inventory_name,
				additional_where );
	}
	else
	{
		sprintf( where, "inventory_name = '%s'", inventory_name );
	}

	order = "order_date,order_time";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=quantity		"
		 "			folder=part_order	"
		 "			where=\"%s\"		"
		 "			order=%s		",
		 application_name,
		 where,
		 order );

	return pipe2list( sys_string );

} /* get_part_order_quantity_list() */

char *build_part_order_additional_where(
				char *order_date_string,
				char *order_time_string,
				char *customer_name,
				char *vehicle_make,
				char *vehicle_model,
				char *vehicle_trim,
				int vehicle_year )
{
	static char additional_where[ 512 ];

	sprintf( additional_where,
		 "not ( order_date = '%s' and		"
		 "order_time = '%s' and			"
		 "customer_name = '%s' and		"
		 "vehicle_make = '%s' and		"
		 "vehicle_model = '%s' and		"
		 "vehicle_trim = '%s' and		"
		 "vehicle_year = %d )			",
		 order_date_string,
		 order_time_string,
		 customer_name,
		 vehicle_make,
		 vehicle_model,
		 vehicle_trim,
		 vehicle_year );

	return additional_where;

} /* build_part_order_additional_where() */

