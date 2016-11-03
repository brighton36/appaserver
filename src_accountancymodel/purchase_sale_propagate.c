/* ---------------------------------------------------------------	*/
/* src_accountancymodel/purchase_sale_propagate.c			*/
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
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "purchase.h"
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_purchase_order(	char *application_name,
					PURCHASE_ORDER *purchase_order );
void post_change_customer_sale(		char *application_name,
					CUSTOMER_SALE *customer_sale );
void purchase_sale_propagate( char *application_name );
LIST *get_purchase_order_list( char *application_name );
LIST *get_customer_sale_list( char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s application\n",
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

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	purchase_sale_propagate( application_name );

	return 0;
} /* main() */

void purchase_sale_propagate( char *application_name )
{
	LIST *purchase_order_list;
	LIST *customer_sale_list;
	PURCHASE_ORDER *purchase_order;
	CUSTOMER_SALE *customer_sale;

	purchase_order_list = get_purchase_order_list( application_name );
	customer_sale_list = get_customer_sale_list( application_name );

	list_rewind( purchase_order_list );
	list_rewind( customer_sale_list );

	while( 1 )
	{
		if ( !list_past_end( purchase_order_list ) )
			purchase_order = list_get( purchase_order_list );
		else
			purchase_order = (PURCHASE_ORDER *)0;

		if ( !list_past_end( customer_sale_list ) )
			customer_sale = list_get( customer_sale_list );
		else
			customer_sale = (CUSTOMER_SALE *)0;

		/* All done. */
		/* --------- */
		if ( !purchase_order && !customer_sale ) break;

		if ( !customer_sale )
		{
			post_change_purchase_order(
				application_name, purchase_order );
			list_next( purchase_order_list );
		}
		else
		if ( !purchase_order )
		{
			post_change_customer_sale(
				application_name, customer_sale );
			list_next( customer_sale_list );
		}
		else
		if ( timlib_strcmp(
				purchase_order->purchase_date_time,
		     		customer_sale->sale_date_time ) <= 0 )
		{
			post_change_purchase_order(
				application_name, purchase_order );
			list_next( purchase_order_list );
		}
		else
		{
			post_change_customer_sale(
				application_name, customer_sale );
			list_next( customer_sale_list );
		}
	}

} /* purchase_sale_propagate() */

void post_change_purchase_order( char *application_name,
				 PURCHASE_ORDER *purchase_order )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "post_change_purchase_order %s \"%s\" \"%s\" %s",
		 application_name,
		 purchase_order->full_name,
		 purchase_order->street_address,
		 purchase_order->purchase_date_time );
}

void post_change_customer_sale( char *application_name,
				CUSTOMER_SALE *customer_sale )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "post_change_customer_sale %s \"%s\" \"%s\" %s",
		 application_name,
		 customer_sale->full_name,
		 customer_sale->street_address,
		 customer_sale->sale_date_time );
}

LIST *get_purchase_order_list( char *application_name )
{
	PURCHASE_ORDER *purchase_order;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char purchase_date_time[ 128 ];
	char *select;
	LIST *purchase_order_list = list_new();

	select = "full_name,street_address,purchase_date_time";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=purchase_order		"
		 "			order=purchase_date_time	",
		 application_name,
		 select );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece( full_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( street_address, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		piece(	purchase_date_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		purchase_order =
			purchase_order_new(
				application_name,
				strdup( full_name ),
				strdup( street_address ),
				strdup( purchase_date_time ) );

		list_append_pointer( purchase_order_list, purchase_order );
	}
	return purchase_order_list;
}

LIST *get_customer_sale_list( char *application_name )
{
	CUSTOMER_SALE *customer_sale;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char sale_date_time[ 128 ];
	char *select;
	LIST *customer_sale_list = list_new();

	select = "full_name,street_address,sale_date_time";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=customer_sale		"
		 "			order=sale_date_time		",
		 application_name,
		 select );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece( full_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( street_address, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		piece(	sale_date_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		customer_sale =
			customer_sale_new(
				application_name,
				strdup( full_name ),
				strdup( street_address ),
				strdup( sale_date_time ) );

		list_append_pointer( customer_sale_list, customer_sale );
	}
	return customer_sale_list;
}

