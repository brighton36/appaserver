/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/customer.c			*/
/* -------------------------------------------------------------------- */
/* This is the AccountancyModel customer_sale ADT.			*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "folder.h"
#include "list.h"
#include "customer.h"
#include "ledger.h"
#include "inventory.h"
#include "entity.h"

CUSTOMER_SALE *customer_sale_calloc( void )
{
	CUSTOMER_SALE *c =
		(CUSTOMER_SALE *)
			calloc( 1, sizeof( CUSTOMER_SALE ) );

	if ( !c )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	c->inventory_account_list = list_new();
	c->cost_account_list = list_new();

	return c;

} /* customer_sale_calloc() */

SERVICE_WORK *customer_service_work_new( char *begin_date_time )
{
	SERVICE_WORK *c =
		(SERVICE_WORK *)
			calloc( 1, sizeof( SERVICE_WORK ) );

	if ( !c )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	c->begin_date_time = begin_date_time;
	return c;

} /* customer_service_work_new() */

CUSTOMER_SALE *customer_sale_new(	char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time )
{
	CUSTOMER_SALE *c = customer_sale_calloc();
	LIST *inventory_list;

	c->full_name = full_name;
	c->street_address = street_address;
	c->sale_date_time = sale_date_time;

	if ( !customer_sale_load(
				&c->sum_extension,
				&c->database_sum_extension,
				&c->sales_tax,
				&c->database_sales_tax,
				&c->invoice_amount,
				&c->database_invoice_amount,
				&c->title_passage_rule,
				&c->completed_date_time,
				&c->database_completed_date_time,
				&c->shipped_date_time,
				&c->database_shipped_date_time,
				&c->arrived_date,
				&c->database_arrived_date,
				&c->total_payment,
				&c->database_total_payment,
				&c->amount_due,
				&c->database_amount_due,
				&c->transaction_date_time,
				&c->database_transaction_date_time,
				&c->shipping_revenue,
				&c->fund_name,
				application_name,
				c->full_name,
				c->street_address,
				c->sale_date_time ) )
	{
		return (CUSTOMER_SALE *)0;
	}

	if ( c->transaction_date_time )
	{
		c->transaction =
			ledger_transaction_with_load_new(
					application_name,
					full_name,
					street_address,
					c->transaction_date_time );
	}

	if ( ledger_folder_exists(
		application_name,
		"inventory" ) )
	{
		c->inventory_sale_list =
			inventory_get_inventory_sale_list(
				application_name,
				c->full_name,
				c->street_address,
				c->sale_date_time );
	}

	if ( ledger_folder_exists(
		application_name,
		"fixed_service_sale" ) )
	{
		c->fixed_service_sale_list =
			customer_fixed_service_sale_get_list(
				application_name,
				c->full_name,
				c->street_address,
				c->sale_date_time );
	}

	if ( ledger_folder_exists(
		application_name,
		"hourly_service_sale" ) )
	{
		c->hourly_service_sale_list =
			customer_hourly_service_sale_get_list(
				application_name,
				c->full_name,
				c->street_address,
				c->sale_date_time );
	}

	if ( ledger_folder_exists(
		application_name,
		"specific_inventory_sale" ) )
	{
		c->specific_inventory_sale_list =
			customer_sale_specific_inventory_get_list(
				application_name,
				c->full_name,
				c->street_address,
				c->sale_date_time );
	}

	c->payment_list =
		customer_payment_get_list(
					application_name,
					c->full_name,
					c->street_address,
					c->sale_date_time );

	c->total_payment =
		customer_get_total_payment(
			c->payment_list );

	inventory_sale_list_set_extension( c->inventory_sale_list );

	c->invoice_amount =
		customer_sale_get_invoice_amount(
			&c->sum_inventory_extension,
			&c->sum_fixed_service_extension,
			&c->sum_hourly_service_extension,
			&c->sum_extension,
			&c->sales_tax,
			c->shipping_revenue,
			c->inventory_sale_list,
			c->specific_inventory_sale_list,
			c->fixed_service_sale_list,
			c->hourly_service_sale_list,
			c->full_name,
			c->street_address,
			application_name );

	c->amount_due =
		CUSTOMER_GET_AMOUNT_DUE(
			c->invoice_amount,
			c->total_payment );

	inventory_list =
		entity_get_inventory_list(
			application_name );

	customer_sale_inventory_cost_account_list_set(
		c->inventory_account_list,
		c->cost_account_list,
		c->inventory_sale_list,
		inventory_list,
		1 /* is_database*/ );

	return c;

} /* customer_sale_new() */

LIST *customer_sale_specific_inventory_get_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 512 ];
	char *transaction_where;
	char *join_where;
	char input_buffer[ 1024 ];
	char inventory_name[ 128 ];
	char serial_number[ 128 ];
	char piece_buffer[ 128 ];
	FILE *input_pipe;
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;
	LIST *specific_inventory_sale_list;

	select =
"specific_inventory_sale.inventory_name,specific_inventory_sale.serial_number,specific_inventory_sale.retail_price,discount_amount,extension,unit_cost";

	folder = "specific_inventory_sale,specific_inventory_purchase";

	transaction_where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					"specific_inventory_sale"
						 /* folder_name */,
					"sale_date_time" );

	join_where =
"specific_inventory_sale.inventory_name = specific_inventory_purchase.inventory_name and specific_inventory_sale.serial_number = specific_inventory_purchase.serial_number";

	sprintf( where, "%s and %s", transaction_where, join_where );

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
	specific_inventory_sale_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( inventory_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( serial_number, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		specific_inventory_sale =
			customer_specific_inventory_sale_new(
				strdup( inventory_name ),
				strdup( serial_number ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		if ( *piece_buffer )
			specific_inventory_sale->retail_price =
				atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		if ( *piece_buffer )
			specific_inventory_sale->discount_amount =
				atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
		if ( *piece_buffer )
			specific_inventory_sale->database_extension =
				atof( piece_buffer );

		specific_inventory_sale->extension =
			CUSTOMER_SALE_GET_EXTENSION(
				specific_inventory_sale->retail_price,
				specific_inventory_sale->discount_amount );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
		if ( *piece_buffer )
			specific_inventory_sale->cost_of_goods_sold =
				atof( piece_buffer );

		list_append_pointer(	specific_inventory_sale_list,
					specific_inventory_sale );
	}

	pclose( input_pipe );
	return specific_inventory_sale_list;

} /* customer_sale_specific_inventory_get_list() */

LIST *customer_fixed_service_sale_get_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *transaction_where;
	char *join_where;
	char where[ 512 ];
	char *select;
	char *folder;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	FIXED_SERVICE *fixed_service;
	LIST *fixed_service_sale_list;

	if ( !ledger_folder_exists(
		application_name,
		"fixed_service_sale" ) )
	{
		return (LIST *)0;
	}

	select =
"fixed_service.service_name,fixed_service_sale.retail_price,discount_amount,extension,work_hours,account";

	folder = "fixed_service_sale,fixed_service";

	transaction_where =
		ledger_get_transaction_where(
			full_name,
			street_address,
			sale_date_time,
			(char *)0 /* folder_name */,
			"sale_date_time" );

	join_where =
	"fixed_service.service_name = fixed_service_sale.service_name";

	transaction_where =
		ledger_get_transaction_where(
			full_name,
			street_address,
			sale_date_time,
			(char *)0 /* folder_name */,
			"sale_date_time" );

	sprintf( where,
		 "%s and %s",
		 transaction_where,
		 join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	fixed_service_sale_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		fixed_service =
			customer_fixed_service_sale_new(
				strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		if ( *piece_buffer )
			fixed_service->retail_price =
				 atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		if ( *piece_buffer )
			fixed_service->discount_amount =
				atof( piece_buffer );

		fixed_service->extension =
			CUSTOMER_SALE_GET_EXTENSION(
				fixed_service->retail_price,
				fixed_service->discount_amount );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		if ( *piece_buffer )
			fixed_service->database_extension =
				atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
		if ( *piece_buffer )
			fixed_service->database_work_hours =
				atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
		if ( *piece_buffer )
			fixed_service->account_name =
				strdup( piece_buffer );

		fixed_service->service_work_list =
			customer_fixed_service_work_get_list(
				&fixed_service->work_hours,
				application_name,
				full_name,
				street_address,
				sale_date_time,
				fixed_service->service_name );

		list_append_pointer( fixed_service_sale_list, fixed_service );
	}

	pclose( input_pipe );
	return fixed_service_sale_list;

} /* customer_fixed_service_sale_get_list() */

LIST *customer_hourly_service_sale_get_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *transaction_where;
	char *join_where;
	char where[ 512 ];
	char *select;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	char service_name[ 256 ];
	char description[ 256 ];
	FILE *input_pipe;
	HOURLY_SERVICE *hourly_service;
	LIST *hourly_service_sale_list;
	char *folder;

	if ( !ledger_folder_exists(
		application_name,
		"hourly_service_sale" ) )
	{
		return (LIST *)0;
	}

	select =
"hourly_service.service_name,description,hourly_service_sale.hourly_rate,discount_amount,extension,work_hours,account";

	folder = "hourly_service_sale,hourly_service";

	transaction_where =
		ledger_get_transaction_where(
			full_name,
			street_address,
			sale_date_time,
			(char *)0 /* folder_name */,
			"sale_date_time" );

	join_where =
	"hourly_service.service_name = hourly_service_sale.service_name";

	sprintf(where,
		"%s and %s",
		transaction_where,
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

	input_pipe = popen( sys_string, "r" );
	hourly_service_sale_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( service_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( description, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		hourly_service =
			customer_hourly_service_sale_new(
				strdup( service_name ),
				strdup( description ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		if ( *piece_buffer )
			hourly_service->hourly_rate =
				 atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		if ( *piece_buffer )
			hourly_service->discount_amount =
				atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
		if ( *piece_buffer )
			hourly_service->database_extension =
				atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
		if ( *piece_buffer )
			hourly_service->database_work_hours =
				atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
		if ( *piece_buffer )
			hourly_service->account_name =
				strdup( piece_buffer );

		hourly_service->service_work_list =
			customer_hourly_service_work_get_list(
				&hourly_service->work_hours,
				application_name,
				full_name,
				street_address,
				sale_date_time,
				hourly_service->service_name,
				hourly_service->description );

		hourly_service->extension =
			CUSTOMER_HOURLY_SERVICE_GET_EXTENSION(
				hourly_service->hourly_rate,
				hourly_service->work_hours,
				hourly_service->discount_amount );

		list_append_pointer( hourly_service_sale_list, hourly_service );
	}

	pclose( input_pipe );

	return hourly_service_sale_list;

} /* customer_hourly_service_sale_get_list() */

SPECIFIC_INVENTORY_SALE *customer_specific_inventory_sale_new(
					char *inventory_name,
					char *serial_number )
{
	SPECIFIC_INVENTORY_SALE *p =
		(SPECIFIC_INVENTORY_SALE *)
			calloc( 1, sizeof( SPECIFIC_INVENTORY_SALE ) );

	p->inventory_name = inventory_name;
	p->serial_number = serial_number;
	return p;

} /* customer_specific_inventory_sale_new() */

FIXED_SERVICE *customer_fixed_service_sale_new( char *service_name )
{
	FIXED_SERVICE *p =
		(FIXED_SERVICE *)
			calloc( 1, sizeof( FIXED_SERVICE ) );

	p->service_name = service_name;
	return p;

} /* customer_fixed_service_sale_new() */

HOURLY_SERVICE *customer_hourly_service_sale_new(
					char *service_name,
					char *description )
{
	HOURLY_SERVICE *p =
		(HOURLY_SERVICE *)
			calloc( 1, sizeof( HOURLY_SERVICE ) );

	p->service_name = service_name;
	p->description = description;
	return p;

} /* customer_hourly_service_sale_new() */

double customer_sale_get_cost_of_goods_sold(
					LIST *inventory_sale_list )
{
	INVENTORY_SALE *inventory_sale;
	double cost_of_goods_sold;

	if ( !list_rewind( inventory_sale_list ) ) return 0.0;

	cost_of_goods_sold = 0.0;

	do {
		inventory_sale = list_get( inventory_sale_list );

		cost_of_goods_sold +=
			inventory_sale->cost_of_goods_sold;

	} while( list_next( inventory_sale_list ) );

	return cost_of_goods_sold;

} /* customer_sale_get_cost_of_goods_sold() */

double customer_sale_get_sum_specific_inventory_extension(
				LIST *specific_inventory_sale_list )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;
	double sum_extension;

	if ( !list_rewind( specific_inventory_sale_list ) ) return 0.0;

	sum_extension = 0.0;

	do {
		specific_inventory_sale =
			list_get( specific_inventory_sale_list );

		sum_extension += specific_inventory_sale->extension;

	} while( list_next( specific_inventory_sale_list ) );

	return sum_extension;

} /* customer_sale_get_sum_specific_inventory_extension() */

double customer_sale_get_sum_inventory_extension(
				LIST *inventory_sale_list )
{
	INVENTORY_SALE *inventory_sale;
	double sum_extension;

	if ( !list_rewind( inventory_sale_list ) ) return 0.0;

	sum_extension = 0.0;

	do {
		inventory_sale = list_get( inventory_sale_list );

		sum_extension += inventory_sale->extension;

	} while( list_next( inventory_sale_list ) );

	return sum_extension;

} /* customer_sale_get_sum_inventory_extension() */

double customer_sale_get_sum_fixed_service_extension(
					LIST *fixed_service_sale_list )
{
	FIXED_SERVICE *s;
	double sum_extension;

	if ( !list_rewind( fixed_service_sale_list ) ) return 0.0;

	sum_extension = 0.0;

	do {
		s = list_get( fixed_service_sale_list );
		sum_extension += s->extension;

	} while( list_next( fixed_service_sale_list ) );

	return sum_extension;

} /* customer_sale_get_sum_fixed_service_extension() */

double customer_sale_get_sum_hourly_service_extension(
					LIST *hourly_service_sale_list )
{
	HOURLY_SERVICE *s;
	double sum_extension;

	if ( !list_rewind( hourly_service_sale_list ) ) return 0.0;

	sum_extension = 0.0;

	do {
		s = list_get( hourly_service_sale_list );
		sum_extension += s->extension;

	} while( list_next( hourly_service_sale_list ) );

	return sum_extension;

} /* customer_sale_get_sum_hourly_service_extension() */

char *customer_sale_get_select( char *application_name )
{
	char select[ 1024 ];
	char *fund_select;
	char *title_passage_rule_select;
	char *shipped_date_time_select;
	char *arrived_date_select;

	if ( ledger_fund_attribute_exists(
			application_name,
			"customer_sale" /* folder_name */ ) )
	{
		fund_select = "fund";
	}
	else
	{
		fund_select = "''";
	}

	if ( ledger_title_passage_rule_attribute_exists(
			application_name,
			"customer_sale" /* folder_name */ ) )
	{
		title_passage_rule_select = "title_passage_rule";
		shipped_date_time_select = "shipped_date_time";
		arrived_date_select = "arrived_date";
	}
	else
	{
		title_passage_rule_select = "''";
		shipped_date_time_select = "''";
		arrived_date_select = "''";
	}

	sprintf( select,
"full_name,street_address,sale_date_time,sum_extension,sales_tax,invoice_amount,%s,completed_date_time,%s,%s,total_payment,amount_due,transaction_date_time,shipping_revenue,%s",
		 title_passage_rule_select,
		 shipped_date_time_select,
		 arrived_date_select,
		 fund_select );

	return strdup( select );

} /* customer_sale_get_select() */

boolean customer_sale_load(
				double *sum_extension,
				double *database_sum_extension,
				double *sales_tax,
				double *database_sales_tax,
				double *invoice_amount,
				double *database_invoice_amount,
				enum title_passage_rule *title_passage_rule,
				char **completed_date_time,
				char **database_completed_date_time,
				char **shipped_date_time,
				char **database_shipped_date_time,
				char **arrived_date,
				char **database_arrived_date,
				double *total_payment,
				double *database_total_payment,
				double *amount_due,
				double *database_amount_due,
				char **transaction_date_time,
				char **database_transaction_date_time,
				double *shipping_revenue,
				char **fund_name,
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char *results;

	select = customer_sale_get_select( application_name );

	where = ledger_get_transaction_where(
			full_name,
			street_address,
			sale_date_time,
			(char *)0 /* folder_name */,
			"sale_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=customer_sale		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	customer_sale_parse(	(char **)0 /* full_name */,
				(char **)0 /* street_address */,
				(char **)0 /* sale_date_time */,
				sum_extension,
				database_sum_extension,
				sales_tax,
				database_sales_tax,
				invoice_amount,
				database_invoice_amount,
				title_passage_rule,
				completed_date_time,
				database_completed_date_time,
				shipped_date_time,
				database_shipped_date_time,
				arrived_date,
				database_arrived_date,
				total_payment,
				database_total_payment,
				amount_due,
				database_amount_due,
				transaction_date_time,
				database_transaction_date_time,
				shipping_revenue,
				fund_name,
				results );

	free( results );

	return 1;

} /* customer_sale_load() */

void customer_sale_parse(	char **full_name,
				char **street_address,
				char **sale_date_time,
				double *sum_extension,
				double *database_sum_extension,
				double *sales_tax,
				double *database_sales_tax,
				double *invoice_amount,
				double *database_invoice_amount,
				enum title_passage_rule *title_passage_rule,
				char **completed_date_time,
				char **database_completed_date_time,
				char **shipped_date_time,
				char **database_shipped_date_time,
				char **arrived_date,
				char **database_arrived_date,
				double *total_payment,
				double *database_total_payment,
				double *amount_due,
				double *database_amount_due,
				char **transaction_date_time,
				char **database_transaction_date_time,
				double *shipping_revenue,
				char **fund_name,
				char *input_buffer )
{
	char piece_buffer[ 128 ];

	if ( full_name )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		if ( *piece_buffer )
			*full_name = strdup( piece_buffer );
	}

	if ( street_address )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		if ( *piece_buffer )
			*street_address = strdup( piece_buffer );
	}

	if ( sale_date_time )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		if ( *piece_buffer )
			*sale_date_time = strdup( piece_buffer );
	}

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *piece_buffer )
		*sum_extension =
		*database_sum_extension = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	if ( *piece_buffer )
		*sales_tax =
		*database_sales_tax = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *piece_buffer )
		*invoice_amount =
		*database_invoice_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *piece_buffer )
		*title_passage_rule =
			entity_get_title_passage_rule(
				piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	if ( *piece_buffer )
		*completed_date_time =
		*database_completed_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	if ( *piece_buffer )
		*shipped_date_time =
		*database_shipped_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
	if ( *piece_buffer )
		*arrived_date =
		*database_arrived_date = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 10 );
	if ( *piece_buffer )
		*total_payment =
		*database_total_payment = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 11 );
	if ( *piece_buffer )
		*amount_due =
		*database_amount_due = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 12 );
	if ( *piece_buffer )
		*transaction_date_time =
		*database_transaction_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 13 );
	if ( *piece_buffer )
		*shipping_revenue = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 14 );
	if ( *piece_buffer )
		*fund_name = strdup( piece_buffer );

} /* customer_sale_parse() */

void customer_sale_update(
				double sum_extension,
				double database_sum_extension,
				double sales_tax,
				double database_sales_tax,
				double invoice_amount,
				double database_invoice_amount,
				char *completed_date_time,
				char *database_completed_date_time,
				char *shipped_date_time,
				char *database_shipped_date_time,
				char *arrived_date,
				char *database_arrived_date,
				double total_payment,
				double database_total_payment,
				double amount_due,
				double database_amount_due,
				char *transaction_date_time,
				char *database_transaction_date_time,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *application_name )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *table_name;
	char *key;

	table_name = get_table_name( application_name, "customer_sale" );
	key = "full_name,street_address,sale_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	output_pipe = popen( sys_string, "w" );

	if ( !dollar_virtually_same(
			sum_extension,
			database_sum_extension ) )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^sum_extension^%.2lf\n",
			full_name,
			street_address,
			sale_date_time,
			sum_extension );
	}

	if ( !dollar_virtually_same(
			sales_tax,
			database_sales_tax ) )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^sales_tax^%.2lf\n",
			full_name,
			street_address,
			sale_date_time,
			sales_tax );
	}

	if ( !dollar_virtually_same(
			invoice_amount,
			database_invoice_amount ) )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^invoice_amount^%.2lf\n",
			full_name,
			street_address,
			sale_date_time,
			invoice_amount );
	}

	if ( timlib_strcmp(
			completed_date_time,
			database_completed_date_time ) != 0 )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^completed_date_time^%s\n",
			full_name,
			street_address,
			sale_date_time,
			(completed_date_time)
				? completed_date_time
				: "" );
	}

	if ( timlib_strcmp(
			shipped_date_time,
			database_shipped_date_time ) != 0 )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^shipped_date_time^%s\n",
			full_name,
			street_address,
			sale_date_time,
			(shipped_date_time)
				? shipped_date_time
				: "" );
	}

	if ( timlib_strcmp(
			arrived_date,
			database_arrived_date ) != 0 )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^arrived_date^%s\n",
			full_name,
			street_address,
			sale_date_time,
			(arrived_date)
				? arrived_date
				: "" );
	}

	if ( !dollar_virtually_same(
			total_payment,
			database_total_payment ) )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^total_payment^%.2lf\n",
			full_name,
			street_address,
			sale_date_time,
			total_payment );
	}

	if ( !dollar_virtually_same(
			amount_due,
			database_amount_due ) )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^amount_due^%.2lf\n",
			full_name,
			street_address,
			sale_date_time,
			amount_due );
	}

	if ( timlib_strcmp(
			transaction_date_time,
			database_transaction_date_time ) != 0 )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^transaction_date_time^%s\n",
			full_name,
			street_address,
			sale_date_time,
			(transaction_date_time)
				? transaction_date_time
				: "" );
	}

	pclose( output_pipe );

} /* customer_sale_update() */

double customer_sale_get_invoice_amount(
				double *sum_inventory_extension,
				double *sum_fixed_service_extension,
				double *sum_hourly_service_extension,
				double *sum_extension,
				double *sales_tax,
				double shipping_revenue,
				LIST *inventory_sale_list,
				LIST *specific_inventory_sale_list,
				LIST *fixed_service_sale_list,
				LIST *hourly_service_sale_list,
				char *full_name,
				char *street_address,
				char *application_name )
{
	double invoice_amount;

	*sum_inventory_extension =
		customer_sale_get_sum_inventory_extension(
			inventory_sale_list );

	*sum_inventory_extension +=
		customer_sale_get_sum_specific_inventory_extension(
			specific_inventory_sale_list );

	*sum_fixed_service_extension =
		customer_sale_get_sum_fixed_service_extension(
			fixed_service_sale_list );

	*sum_hourly_service_extension =
		customer_sale_get_sum_hourly_service_extension(
			hourly_service_sale_list );

	*sum_extension =
			*sum_inventory_extension +
			*sum_fixed_service_extension +
			*sum_hourly_service_extension;

	*sales_tax =
		*sum_inventory_extension *
		customer_sale_get_tax_rate(
			application_name,
			full_name,
			street_address );

	*sales_tax = timlib_round_money( *sales_tax );

	invoice_amount =
		*sum_extension +
		*sales_tax +
		shipping_revenue;

	return invoice_amount;

} /* customer_sale_get_invoice_amount() */

double customer_sale_get_tax_rate(
			char *application_name,
			char *full_name,
			char *street_address )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char *results;
	ENTITY *entity_customer;
	ENTITY_SELF *self;

	self = entity_self_load( application_name );

	entity_customer = entity_new( full_name, street_address );

	if ( !entity_load(	&entity_customer->unit,
				&entity_customer->city,
				&entity_customer->state_code,
				&entity_customer->zip_code,
				&entity_customer->phone_number,
				&entity_customer->email_address,
				&entity_customer->sales_tax_exempt,
				application_name,
				entity_customer->full_name,
				entity_customer->street_address ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load entity = (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 entity_customer->full_name,
			 entity_customer->street_address );
		exit( 1 );
	}

	if ( entity_customer->sales_tax_exempt
	||   timlib_strcmp(	entity_customer->state_code,
				self->entity->state_code ) != 0 )
	{
		return 0.0;
	}

	select = "application_constant_value";
	where = "application_constant = 'sales_tax_rate'";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=application_constants	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0.0;

	return atof( results );

} /* customer_sale_get_tax_rate() */

LIST *customer_payment_get_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	CUSTOMER_PAYMENT *customer_payment;
	LIST *payment_list;

	select =
"payment_date_time,payment_amount,check_number,transaction_date_time";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=customer_payment		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );
	payment_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		customer_payment =
			customer_payment_new(
				strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		if ( *piece_buffer )
			customer_payment->payment_amount = atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		if ( *piece_buffer )
			customer_payment->check_number = atoi( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		if ( *piece_buffer )
		{
			customer_payment->transaction_date_time =
			customer_payment->database_transaction_date_time =
				strdup( piece_buffer );

			customer_payment->transaction =
				ledger_transaction_with_load_new(
					application_name,
					full_name,
					street_address,
					customer_payment->
						transaction_date_time );
		}

		list_append_pointer( payment_list, customer_payment );
	}

	pclose( input_pipe );
	return payment_list;

} /* customer_payment_get_list() */

double customer_get_total_payment(
			LIST *payment_list )
{
	double total_payment;
	CUSTOMER_PAYMENT *customer_payment;

	if ( !list_rewind( payment_list ) ) return 0.0;

	total_payment = 0.0;

	do {
		customer_payment = list_get( payment_list );
		total_payment += customer_payment->payment_amount;

	} while( list_next( payment_list ) );

	return total_payment;

} /* customer_get_total_payment() */

LIST *customer_sale_ledger_cost_of_goods_sold_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				LIST *inventory_account_list,
				LIST *cost_account_list )
{
	LIST *propagate_account_list = {0};
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;
	INVENTORY_COST_ACCOUNT *inventory_cost_account;

	/* Insert debit cost_of_goods_sold accounts */
	/* ---------------------------------------- */
	if ( !list_rewind( cost_account_list ) ) return (LIST *)0;

	do {
		inventory_cost_account =
			list_get_pointer(
				cost_account_list );

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			inventory_cost_account->account_name,
			inventory_cost_account->cost_of_goods_sold,
			1 /* is_debit */ );

		if ( !propagate_account_list )
			propagate_account_list = list_new();

		account =
			ledger_account_new(
				inventory_cost_account->
					account_name );

		prior_ledger = ledger_get_prior_ledger(
					application_name,
					transaction_date_time,
					account->account_name );

		account->journal_ledger_list =
			ledger_get_propagate_journal_ledger_list(
				application_name,
				prior_ledger,
				account->account_name );

		list_append_pointer( propagate_account_list, account );

	} while( list_next( cost_account_list ) );

	/* Insert credit inventory accounts */
	/* -------------------------------- */
	if ( !list_rewind( inventory_account_list ) ) return (LIST *)0;

	do {
		inventory_cost_account =
			list_get_pointer(
				inventory_account_list );

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			inventory_cost_account->account_name,
			inventory_cost_account->cost_of_goods_sold,
			0 /* not is_debit */ );

		if ( !propagate_account_list )
			propagate_account_list = list_new();

		account =
			ledger_account_new(
				inventory_cost_account->
					account_name );

		prior_ledger = ledger_get_prior_ledger(
					application_name,
					transaction_date_time,
					account->account_name );

		account->journal_ledger_list =
			ledger_get_propagate_journal_ledger_list(
				application_name,
				prior_ledger,
				account->account_name );

		list_append_pointer( propagate_account_list, account );

	} while( list_next( inventory_account_list ) );

	return propagate_account_list;

} /* customer_sale_ledger_cost_of_goods_sold_insert() */

#ifdef NOT_DEFINED
LIST *customer_sale_ledger_refresh(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double sum_inventory_extension,
				double sum_fixed_service_extension,
				double sum_hourly_service_extension,
				double sales_tax,
				double shipping_revenue,
				double invoice_amount )
{
	char *sales_revenue_account = {0};
	char *sales_tax_payable_account = {0};
	char *shipping_revenue_account = {0};
	char *receivable_account = {0};
	LIST *propagate_account_list = {0};
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;

	ledger_delete(			application_name,
					LEDGER_FOLDER_NAME,
					full_name,
					street_address,
					transaction_date_time );

	ledger_get_customer_sale_account_names(
		&sales_revenue_account,
		&sales_tax_payable_account,
		&shipping_revenue_account,
		&receivable_account,
		application_name,
		fund_name );

	if ( invoice_amount )
	{
		if ( !receivable_account )
		{
			fprintf( stderr,
			 "Error in %s/%s()/%d: empty receivable_account.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
			exit(1 );
		}

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			receivable_account,
			invoice_amount,
			1 /* is_debit */ );

		propagate_account_list = list_new();

		account = ledger_account_new( receivable_account );

		prior_ledger = ledger_get_prior_ledger(
					application_name,
					transaction_date_time,
					receivable_account );

		account->journal_ledger_list =
			ledger_get_propagate_journal_ledger_list(
				application_name,
				prior_ledger,
				receivable_account );

		list_append_pointer( propagate_account_list, account );
	}

	if ( sales_tax )
	{
		if ( !sales_tax_payable_account )
		{
			fprintf( stderr,
		"Error in %s/%s()/%d: empty sales_tax_payable_account.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
			exit(1 );
		}

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			sales_tax_payable_account,
			sales_tax,
			0 /* not is_debit */ );

		if ( !propagate_account_list )
			propagate_account_list = list_new();

		account = ledger_account_new( sales_tax_payable_account );

		prior_ledger = ledger_get_prior_ledger(
					application_name,
					transaction_date_time,
					sales_tax_payable_account );

		account->journal_ledger_list =
			ledger_get_propagate_journal_ledger_list(
				application_name,
				prior_ledger,
				sales_tax_payable_account );

		list_append_pointer( propagate_account_list, account );
	}

	if ( shipping_revenue )
	{
		if ( !shipping_revenue_account )
		{
			fprintf( stderr,
		"Error in %s/%s()/%d: empty shipping_revenue_account.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
			exit(1 );
		}

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			shipping_revenue_account,
			shipping_revenue,
			0 /* not is_debit */ );

		if ( !propagate_account_list )
			propagate_account_list = list_new();

		account = ledger_account_new( shipping_revenue_account );

		prior_ledger = ledger_get_prior_ledger(
					application_name,
					transaction_date_time,
					shipping_revenue_account );

		account->journal_ledger_list =
			ledger_get_propagate_journal_ledger_list(
				application_name,
				prior_ledger,
				shipping_revenue_account );

		list_append_pointer( propagate_account_list, account );
	}

	if ( sum_inventory_extension )
	{
		if ( !sales_revenue_account )
		{
			fprintf( stderr,
		"Error in %s/%s()/%d: empty sales_revenue_account.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
			exit(1 );
		}

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			sales_revenue_account,
			sum_inventory_extension,
			0 /* not is_debit */ );

		if ( !propagate_account_list )
			propagate_account_list = list_new();

		account = ledger_account_new( sales_revenue_account );

		prior_ledger = ledger_get_prior_ledger(
					application_name,
					transaction_date_time,
					sales_revenue_account );

		account->journal_ledger_list =
			ledger_get_propagate_journal_ledger_list(
				application_name,
				prior_ledger,
				sales_revenue_account );

		list_append_pointer( propagate_account_list, account );
	}

	if ( sum_fixed_service_extension + sum_hourly_service_extension )
	{
		if ( !service_revenue_account )
		{
			fprintf( stderr,
		"Error in %s/%s()/%d: empty service_revenue_account.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
			exit(1 );
		}

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			service_revenue_account,
			sum_fixed_service_extension +
			sum_hourly_service_extension,
			0 /* not is_debit */ );

		if ( !propagate_account_list )
			propagate_account_list = list_new();

		account = ledger_account_new( service_revenue_account );

		prior_ledger = ledger_get_prior_ledger(
					application_name,
					transaction_date_time,
					service_revenue_account );

		account->journal_ledger_list =
			ledger_get_propagate_journal_ledger_list(
				application_name,
				prior_ledger,
				service_revenue_account );

		list_append_pointer( propagate_account_list, account );
	}

	return propagate_account_list;

} /* customer_sale_ledger_refresh() */
#endif

char *customer_get_max_completed_date_time( char *application_name )
{
	char sys_string[ 1024 ];
	char *select = "max( completed_date_time )";
	char *where = "completed_date_time is not null";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=customer_sale		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	return pipe2string( sys_string );

} /* customer_get_max_completed_date_time() */

char *customer_get_completed_sale_date_time(
				char *application_name,
				char *completed_date_time )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	if ( !completed_date_time || !*completed_date_time ) return (char *)0;

	sprintf( where,
		 "completed_date_time = '%s'",
		 completed_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=sale_date_time		"
		 "			folder=customer_sale		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* customer_get_completed_sale_date_time() */
 
char *customer_sale_fetch_transaction_date_time(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time )
{

	char sys_string[ 1024 ];
	char *where;

	where = ledger_get_transaction_where(
			full_name,
			street_address,
			sale_date_time,
			"customer_sale",
			"sale_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=transaction_date_time	"
		 "			folder=customer_sale		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* customer_sale_fetch_transaction_date_time() */

CUSTOMER_SALE *customer_sale_seek(
				LIST *customer_sale_list,
				char *sale_date_time )
{
	CUSTOMER_SALE *customer_sale;

	if ( !list_rewind( customer_sale_list ) )
		return (CUSTOMER_SALE *)0;

	do {
		customer_sale =
			list_get( 
				customer_sale_list );

		if ( strcmp(	customer_sale->sale_date_time,
				sale_date_time ) == 0 )
		{
			return customer_sale;
		}

	} while( list_next( customer_sale_list ) );

	return (CUSTOMER_SALE *)0;

} /* customer_sale_seek() */

HOURLY_SERVICE *customer_hourly_service_sale_seek(
			LIST *hourly_service_sale_list,
			char *service_name,
			char *description )
{
	HOURLY_SERVICE *hourly_service;

	if ( !list_rewind( hourly_service_sale_list ) )
		return (HOURLY_SERVICE *)0;

	do {
		hourly_service = list_get( hourly_service_sale_list );

		if ( strcmp(	hourly_service->service_name,
				service_name ) == 0
		&&   strcmp(	hourly_service->description,
				description ) == 0 )
		{
			return hourly_service;
		}

	} while( list_next( hourly_service_sale_list ) );

	return (HOURLY_SERVICE *)0;

} /* customer_hourly_service_sale_seek() */

FIXED_SERVICE *customer_fixed_service_sale_seek(
			LIST *fixed_service_sale_list,
			char *service_name )
{
	FIXED_SERVICE *fixed_service;

	if ( !list_rewind( fixed_service_sale_list ) )
		return (FIXED_SERVICE *)0;

	do {
		fixed_service = list_get( fixed_service_sale_list );

		if ( strcmp(	fixed_service->service_name,
				service_name ) == 0 )
		{
			return fixed_service;
		}

	} while( list_next( fixed_service_sale_list ) );

	return (FIXED_SERVICE *)0;

} /* customer_fixed_service_sale_seek() */

SERVICE_WORK *customer_service_work_seek(
				LIST *service_work_list,
				char *begin_date_time )
{
	SERVICE_WORK *service_work;

	if ( !list_rewind( service_work_list ) )
		return (SERVICE_WORK *)0;

	do {
		service_work = list_get_pointer( service_work_list );

		if ( strcmp(	service_work->begin_date_time,
				begin_date_time ) == 0 )
		{
			return service_work;
		}

	} while( list_next( service_work_list ) );

	return (SERVICE_WORK *)0;

} /* customer_service_work_seek() */

SPECIFIC_INVENTORY_SALE *customer_specific_inventory_sale_seek(
			LIST *specific_inventory_sale_list,
			char *inventory_name,
			char *serial_number )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;

	if ( !list_rewind( specific_inventory_sale_list ) )
		return (SPECIFIC_INVENTORY_SALE *)0;

	do {
		specific_inventory_sale =
			list_get( specific_inventory_sale_list );

		if ( strcmp(	specific_inventory_sale->inventory_name,
				inventory_name ) == 0
		&&   strcmp(	specific_inventory_sale->serial_number,
				serial_number ) == 0 )
		{
			return specific_inventory_sale;
		}

	} while( list_next( specific_inventory_sale_list ) );

	return (SPECIFIC_INVENTORY_SALE *)0;

} /* customer_specific_inventory_sale_seek() */

char *customer_sale_get_inventory_sale_join_where( void )
{
	char *join_where;

	join_where =	"customer_sale.full_name =			"
			"	inventory_sale.full_name and		"
			"customer_sale.street_address =			"
			"	inventory_sale.street_address and	"
			"customer_sale.sale_date_time =			"
			"	inventory_sale.sale_date_time and	"
			"inventory_sale.inventory_name =		"
			"inventory.inventory_name			";

	return join_where;

} /* customer_sale_get_inventory_sale_join_where() */

LIST *customer_get_inventory_sale_list(
				char *full_name,
				char *street_address,
				char *sale_date_time,
				HASH_TABLE *inventory_sale_hash_table,
				LIST *inventory_sale_name_list )
{
	char *inventory_name;
	INVENTORY_SALE *inventory_sale;
	LIST *inventory_sale_list = {0};
	char *key;

	if ( !list_rewind( inventory_sale_name_list ) )
		return (LIST *)0;

	do {
		inventory_name = list_get( inventory_sale_name_list );

		key = inventory_sale_get_hash_table_key(
				full_name,
				street_address,
				sale_date_time,
				inventory_name );

		if ( ( inventory_sale =
			hash_table_fetch(
				inventory_sale_hash_table,
				key ) ) )
		{
			if ( !inventory_sale_list )
				inventory_sale_list = list_new();

			list_append_pointer(
				inventory_sale_list,
				inventory_sale );
		}

	} while( list_next( inventory_sale_name_list ) );

	return inventory_sale_list;

} /* customer_get_inventory_sale_list() */

LIST *customer_get_inventory_customer_sale_list(
			char *application_name,
			char *inventory_name,
			HASH_TABLE *inventory_sale_hash_table,
			LIST *inventory_sale_name_list,
			HASH_TABLE *transaction_hash_table,
			HASH_TABLE *journal_ledger_hash_table,
			LIST *inventory_list )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *inventory_subquery;
	char *select;
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	CUSTOMER_SALE *customer_sale;
	LIST *customer_sale_list = {0};

	select = customer_sale_get_select( application_name );

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"customer_sale"      /* one2m_folder_name */,
			"inventory_sale"     /* mto1_folder_name */,
			"sale_date_time"     /* foreign_attribute_name */ );

	sprintf(	where,
			"%s and completed_date_time is not null",
			inventory_subquery );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=customer_sale		"
		 "			where=\"%s\"			"
		 "			order=completed_date_time	",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		customer_sale = customer_sale_calloc();

		customer_sale_parse(
				&customer_sale->full_name,
				&customer_sale->street_address,
				&customer_sale->sale_date_time,
				&customer_sale->sum_extension,
				&customer_sale->database_sum_extension,
				&customer_sale->sales_tax,
				&customer_sale->database_sales_tax,
				&customer_sale->invoice_amount,
				&customer_sale->database_invoice_amount,
				&customer_sale->title_passage_rule,
				&customer_sale->completed_date_time,
				&customer_sale->database_completed_date_time,
				&customer_sale->shipped_date_time,
				&customer_sale->database_shipped_date_time,
				&customer_sale->arrived_date,
				&customer_sale->database_arrived_date,
				&customer_sale->total_payment,
				&customer_sale->database_total_payment,
				&customer_sale->amount_due,
				&customer_sale->database_amount_due,
				&customer_sale->transaction_date_time,
				&customer_sale->database_transaction_date_time,
				&customer_sale->shipping_revenue,
				&customer_sale->fund_name,
				input_buffer );

		customer_sale->inventory_sale_list =
			customer_get_inventory_sale_list(
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->sale_date_time,
				inventory_sale_hash_table,
				inventory_sale_name_list );

		customer_sale_inventory_cost_account_list_set(
			customer_sale->inventory_account_list,
			customer_sale->cost_account_list,
			customer_sale->inventory_sale_list,
			inventory_list,
			1 /* is_database */ );

		inventory_sale_list_set_extension(
			customer_sale->inventory_sale_list );

		customer_sale->sum_inventory_extension =
			customer_sale_get_sum_inventory_extension(
				customer_sale->inventory_sale_list );

		customer_sale->fixed_service_sale_list =
			customer_fixed_service_sale_get_list(
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->sale_date_time );

		customer_sale->sum_fixed_service_extension =
			customer_sale_get_sum_fixed_service_extension(
				customer_sale->fixed_service_sale_list );

		customer_sale->hourly_service_sale_list =
			customer_hourly_service_sale_get_list(
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->sale_date_time );

		customer_sale->sum_hourly_service_extension =
			customer_sale_get_sum_hourly_service_extension(
				customer_sale->hourly_service_sale_list );

		customer_sale->payment_list =
			customer_payment_get_list(
					application_name,
					customer_sale->full_name,
					customer_sale->street_address,
					customer_sale->sale_date_time );

		customer_sale->total_payment =
			customer_get_total_payment(
				customer_sale->payment_list );

		customer_sale->invoice_amount =
			customer_sale_get_invoice_amount(
				&customer_sale->sum_inventory_extension,
				&customer_sale->sum_fixed_service_extension,
				&customer_sale->sum_hourly_service_extension,
				&customer_sale->sum_extension,
				&customer_sale->sales_tax,
				customer_sale->shipping_revenue,
				customer_sale->inventory_sale_list,
				customer_sale->specific_inventory_sale_list,
				customer_sale->fixed_service_sale_list,
				customer_sale->hourly_service_sale_list,
				customer_sale->full_name,
				customer_sale->street_address,
				application_name );

		if ( customer_sale->transaction_date_time )
		{
			customer_sale->transaction =
				ledger_sale_hash_table_build_transaction(
					application_name,
					customer_sale->fund_name,
					customer_sale->full_name,
					customer_sale->street_address,
					customer_sale->transaction_date_time,
					transaction_hash_table,
					journal_ledger_hash_table );
		}

		if ( !customer_sale_list )
			customer_sale_list = list_new();

		list_append_pointer( customer_sale_list, customer_sale );

	}

	pclose( input_pipe );
	return customer_sale_list;

} /* customer_get_inventory_customer_sale_list() */

void customer_sale_transaction_delete_with_propagate(
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

	entity_propagate_customer_sale_ledger_accounts(
		application_name,
		fund_name,
		transaction_date_time );

} /* customer_sale_transaction_delete_with_propagate() */

boolean customer_sale_inventory_is_latest(
				char *application_name,
				char *inventory_name,
				char *sale_date_time )
{
	INVENTORY *inventory;

	inventory = inventory_load_new(
			application_name,
			inventory_name );

	inventory->
		last_inventory_balance->
		inventory_purchase =
			inventory_get_last_inventory_purchase(
				application_name,
				inventory_name );

	if ( !inventory->last_inventory_balance->inventory_purchase )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: This inventory item has never been purchased: (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	inventory->
		last_inventory_balance->
		inventory_sale =
			inventory_get_last_inventory_sale(
				application_name,
				inventory_name );

	if ( !inventory->last_inventory_balance->inventory_sale )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot inventory_get_last_inventory_sale(%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	return inventory_is_latest_sale(
		inventory->
			last_inventory_balance->
			inventory_sale->
				sale_date_time,
		inventory->
			last_inventory_balance->
			inventory_sale->
			completed_date_time,
		inventory->
			last_inventory_balance->
			inventory_purchase->
			arrived_date_time,
		sale_date_time );

} /* customer_sale_inventory_is_latest() */

void customer_sale_list_cost_of_goods_sold_transaction_update(
				char *application_name,
				LIST *customer_sale_list )
{
	CUSTOMER_SALE *customer_sale;
	INVENTORY_COST_ACCOUNT *inventory_cost_account;
	FILE *update_pipe;
	char *customer_sale_transaction_date_time = {0};
	LIST *propagate_account_name_list;

	if ( !list_rewind( customer_sale_list ) ) return;

	update_pipe = ledger_open_update_pipe( application_name );

	propagate_account_name_list = list_new();

	do {
		customer_sale =
			list_get_pointer(
				customer_sale_list );

		if ( !customer_sale->transaction ) continue;

		/* Update cost_of_goods_sold debit account */
		/* --------------------------------------- */
		if ( !list_rewind( customer_sale->cost_account_list ) )
			continue;

		do {
			inventory_cost_account =
				list_get_pointer(
					customer_sale->
						cost_account_list );

			if ( !dollar_virtually_same(
				inventory_cost_account->
					cost_of_goods_sold,
				inventory_cost_account->
					database_cost_of_goods_sold ) )
			{
				ledger_journal_ledger_update(
					update_pipe,
					customer_sale->
						transaction->
						full_name,
					customer_sale->
						transaction->
						street_address,
					customer_sale->
						transaction->
						transaction_date_time,
					inventory_cost_account->
						account_name,
					inventory_cost_account->
						cost_of_goods_sold,
					0.0 /* credit_amount */ );

				if ( !customer_sale_transaction_date_time )
				{
					customer_sale_transaction_date_time =
						customer_sale->
							transaction->
							transaction_date_time;
				}

				list_append_unique_string(
					propagate_account_name_list,
					inventory_cost_account->
					      account_name );
			}

		} while( list_next( customer_sale->cost_account_list ) );

		/* Update inventory credit account */
		/* ------------------------------- */
		if ( !list_rewind( customer_sale->inventory_account_list ) )
			continue;

		do {
			inventory_cost_account =
				list_get_pointer(
					customer_sale->
						inventory_account_list );

			if ( !dollar_virtually_same(
				inventory_cost_account->
					cost_of_goods_sold,
				inventory_cost_account->
					database_cost_of_goods_sold ) )
			{
				ledger_journal_ledger_update(
					update_pipe,
					customer_sale->
						transaction->
						full_name,
					customer_sale->
						transaction->
						street_address,
					customer_sale->
						transaction->
						transaction_date_time,
					inventory_cost_account->
						account_name,
					0.0 /* debit_amount */,
					inventory_cost_account->
						cost_of_goods_sold );

				if ( !customer_sale_transaction_date_time )
				{
					customer_sale_transaction_date_time =
						customer_sale->
							transaction->
							transaction_date_time;
				}

				list_append_unique_string(
					propagate_account_name_list,
					inventory_cost_account->
					      account_name );
			}

		} while( list_next( customer_sale->cost_account_list ) );

	} while( list_next( customer_sale_list ) );

	pclose( update_pipe );

	if ( customer_sale_transaction_date_time )
	{
		ledger_propagate_account_name_list(
			application_name,
			customer_sale_transaction_date_time,
			propagate_account_name_list );
	}

} /* customer_sale_list_cost_of_goods_sold_transaction_update() */

void customer_sale_list_cost_of_goods_sold_set(
			LIST *customer_sale_list,
			LIST *inventory_list )
{
	CUSTOMER_SALE *customer_sale;

	if ( !list_rewind( customer_sale_list ) ) return;

	do {
		customer_sale = list_get( customer_sale_list );

		customer_sale_inventory_cost_account_list_set(
			customer_sale->inventory_account_list,
			customer_sale->cost_account_list,
			customer_sale->inventory_sale_list,
			inventory_list,
			0 /* not is_database */ );

	} while( list_next( customer_sale_list ) );

} /* customer_sale_list_cost_of_goods_sold_set() */

CUSTOMER_PAYMENT *customer_payment_new(
				char *payment_date_time )
{
	CUSTOMER_PAYMENT *p =
		(CUSTOMER_PAYMENT *)
			calloc( 1, sizeof( CUSTOMER_PAYMENT ) );

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

} /* customer_payment_new() */

LIST *customer_get_payment_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	CUSTOMER_PAYMENT *customer_payment;
	LIST *payment_list;

	select = "payment_date_time,payment_amount,transaction_date_time";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=customer_payment		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );
	payment_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		customer_payment =
			customer_payment_new(
				strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		if ( *piece_buffer )
			customer_payment->payment_amount = atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		if ( *piece_buffer )
		{
			customer_payment->transaction_date_time =
			customer_payment->database_transaction_date_time =
				strdup( piece_buffer );

			customer_payment->transaction =
				ledger_transaction_with_load_new(
					application_name,
					full_name,
					street_address,
					customer_payment->
						transaction_date_time );
		}

		list_append_pointer( payment_list, customer_payment );
	}

	pclose( input_pipe );
	return payment_list;

} /* customer_get_payment_list() */

CUSTOMER_PAYMENT *customer_payment_seek(
				LIST *customer_payment_list,
				char *payment_date_time )
{
	CUSTOMER_PAYMENT *customer_payment;

	if ( !list_rewind( customer_payment_list ) )
		return (CUSTOMER_PAYMENT *)0;

	do {
		customer_payment = list_get( customer_payment_list );

		if ( timlib_strcmp(	customer_payment->payment_date_time,
					payment_date_time ) == 0 )
		{
			return customer_payment;
		}

	} while( list_next( customer_payment_list ) );

	return (CUSTOMER_PAYMENT *)0;

} /* customer_payment_seek() */

/* Returns propagate_account_list */
/* ------------------------------ */
LIST *customer_payment_journal_ledger_refresh(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double payment_amount )
{
	char *checking_account = {0};
	char *account_receivable_account = {0};
	LIST *propagate_account_list = {0};
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;

	ledger_delete(			application_name,
					LEDGER_FOLDER_NAME,
					full_name,
					street_address,
					transaction_date_time );

	ledger_get_customer_payment_account_names(
		&checking_account,
		&account_receivable_account,
		application_name,
		fund_name );

	if ( payment_amount )
	{
		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			checking_account,
			payment_amount,
			1 /* is_debit */ );

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			account_receivable_account,
			payment_amount,
			0 /* not is_debit */ );
	}

	propagate_account_list = list_new();

	account = ledger_account_new( account_receivable_account );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				account_receivable_account );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			account_receivable_account );

	list_append_pointer( propagate_account_list, account );

	account = ledger_account_new( checking_account );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				checking_account );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			checking_account );

	list_append_pointer( propagate_account_list, account );

	return propagate_account_list;

} /* customer_payment_journal_ledger_refresh() */

void customer_payment_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
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
		customer_payment_get_update_sys_string(
			application_name );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^%s^transaction_date_time^%s\n",
		full_name,
		street_address,
		sale_date_time,
		payment_date_time,
		(transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( output_pipe );

} /* customer_payment_update() */

char *customer_payment_get_update_sys_string(
				char *application_name )
{
	static char sys_string[ 256 ];
	char *table_name;
	char *key;

	table_name = get_table_name( application_name, "customer_payment" );
	key = "full_name,street_address,sale_date_time,payment_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	return sys_string;

} /* customer_payment_get_update_sys_string() */

void customer_specific_inventory_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				char *serial_number,
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

	table_name =
		get_table_name(
			application_name,
			"specific_inventory_sale" );

	key_column_list_string =
"full_name,street_address,sale_date_time,inventory_name,serial_number";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	update_pipe = popen( sys_string, "w" );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^%s^extension^%.2lf\n",
	 	full_name,
	 	street_address,
	 	sale_date_time,
	 	inventory_name,
		serial_number,
	 	extension );

	pclose( update_pipe );

} /* customer_specific_inventory_update() */

double customer_sale_get_specific_inventory_cost_of_goods_sold(
				LIST *specific_inventory_sale_list )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;
	double sum_cost_of_goods_sold;

	if ( !list_rewind( specific_inventory_sale_list ) ) return 0.0;

	sum_cost_of_goods_sold = 0.0;

	do {
		specific_inventory_sale =
			list_get( specific_inventory_sale_list );

		sum_cost_of_goods_sold +=
			specific_inventory_sale->cost_of_goods_sold;

	} while( list_next( specific_inventory_sale_list ) );

	return sum_cost_of_goods_sold;

} /* customer_sale_get_specific_inventory_cost_of_goods_sold() */

/* Need to add SPECIFIC_INVENTORY_PURCHASE.unit_cost */
/* ------------------------------------------------- */
double customer_sale_fetch_cost_of_goods_sold(
					char *full_name,
					char *street_address,
					char *sale_date_time,
					char *application_name )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	double customer_sale_cost_of_goods_sold;

	select = "sum( cost_of_goods_sold )";
	where = ledger_get_transaction_where(
			full_name,
			street_address,
			sale_date_time,
			(char *)0 /* folder_name */,
			"sale_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=inventory_sale	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	customer_sale_cost_of_goods_sold = atof( pipe2string( sys_string ) );

	return customer_sale_cost_of_goods_sold;

} /* customer_sale_fetch_cost_of_goods_sold() */

double customer_sale_get_amount_due(	char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *select;
	char *where;
	char *results_string;

	select = "amount_due";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=customer_sale	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	results_string = pipe2string( sys_string );

	if ( !results_string )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get amount_due.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return atof( results_string );

} /* customer_sale_get_amount_due() */

double customer_sale_get_total_payment(	char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time )
{
	char *table_name;
	char sys_string[ 1024 ];
	char *where;
	char *results_string;

	table_name = get_table_name( application_name, "customer_payment" );

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

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

} /* customer_sale_get_total_payment() */

char *customer_sale_get_memo( char *full_name )
{
	char memo[ 256 ];

	if ( full_name && *full_name )
	{
		sprintf(	memo,
				"%s by %s",
				CUSTOMER_SALE_MEMO,
				full_name );
	}
	else
	{
		strcpy(	memo, CUSTOMER_SALE_MEMO );
	}
	return strdup( memo );

} /* customer_sale_get_memo() */

void customer_sale_inventory_cost_account_list_set(
			LIST *inventory_account_list,
			LIST *cost_account_list,
			LIST *inventory_sale_list,
			LIST *inventory_list,
			boolean is_database )
{
	INVENTORY_SALE *inventory_sale;
	INVENTORY *inventory;
	INVENTORY_COST_ACCOUNT *inventory_cost_account;

	if ( !list_rewind( inventory_sale_list ) ) return;

	do {
		inventory_sale = list_get_pointer( inventory_sale_list );

		if ( ! ( inventory =
				inventory_list_seek(
					inventory_list,
					inventory_sale->inventory_name ) ) )
		{
			fprintf( stderr,
		"Error in %s/%s()/%d: cannot seek inventory_name = (%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
				inventory_sale->inventory_name );
			exit(1 );
		}

		if ( !inventory->inventory_account_name )
		{
			fprintf( stderr,
"Error in %s/%s()/%d: empty inventory account name for inventory_name = (%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
				inventory->inventory_name );
			exit(1 );
		}

		if ( !inventory->cost_of_goods_sold_account_name )
		{
			fprintf( stderr,
"Error in %s/%s()/%d: empty cost of goods sold account name for inventory_name = (%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
				inventory->inventory_name );
			exit(1 );
		}

		inventory_cost_account =
			inventory_get_or_set_cost_account(
				inventory_account_list,
				inventory->inventory_account_name );

		if ( is_database )
		{
			inventory_cost_account->database_cost_of_goods_sold +=
				inventory_sale->cost_of_goods_sold;
		}
		else
		{
			inventory_cost_account->cost_of_goods_sold +=
				inventory_sale->cost_of_goods_sold;
		}

		inventory_cost_account =
			inventory_get_or_set_cost_account(
				cost_account_list,
				inventory->cost_of_goods_sold_account_name );

		if ( is_database )
		{
			inventory_cost_account->database_cost_of_goods_sold +=
				inventory_sale->cost_of_goods_sold;
		}
		else
		{
			inventory_cost_account->cost_of_goods_sold +=
				inventory_sale->cost_of_goods_sold;
		}

	} while( list_next( inventory_sale_list ) );

} /* customer_sale_inventory_cost_account_list_set() */

double customer_get_work_hours(	char *end_date_time,
				char *begin_date_time )
{
	DATE *end_date;
	DATE *begin_date;
	double work_hours;

	end_date = date_yyyy_mm_dd_hms_new( end_date_time );
	begin_date = date_yyyy_mm_dd_hms_new( begin_date_time );

	work_hours =
		(double)date_subtract_minutes(
				end_date /* later_date */,
				begin_date /* earlier_date */ ) /
			60.0;

	return work_hours;

} /* customer_get_work_hours() */

LIST *customer_fixed_service_work_get_list(
				double *work_hours,
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name )
{
	char sys_string[ 1024 ];
	char *ledger_where;
	char where[ 256 ];
	char *select;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	SERVICE_WORK *service_work;
	LIST *service_work_list;

	select =
	"begin_date_time,end_date_time,work_hours";

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	sprintf( where,
		 "%s and service_name = '%s'",
		 ledger_where,
		 service_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=fixed_service_work	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );
	service_work_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );

		service_work =
			customer_service_work_new(
				strdup( piece_buffer )
					/* begin_date_time */ );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		if ( *piece_buffer )
		{
			service_work->end_date_time = strdup( piece_buffer );

			service_work->work_hours =
				customer_get_work_hours(
					service_work->end_date_time,
					service_work->begin_date_time );

			*work_hours += service_work->work_hours;
		}

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );

		if ( *piece_buffer )
		{
			service_work->database_work_hours =
				atof( piece_buffer );
		}

		list_append_pointer( service_work_list, service_work );
	}

	pclose( input_pipe );
	return service_work_list;

} /* customer_fixed_service_work_get_list() */

LIST *customer_hourly_service_work_get_list(
				double *work_hours,
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				char *description )
{
	char sys_string[ 1024 ];
	char *ledger_where;
	char where[ 512 ];
	char buffer[ 256 ];
	char *select;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	SERVICE_WORK *service_work;
	LIST *service_work_list;

	select =
	"begin_date_time,end_date_time,work_hours";

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	sprintf( where,
		 "%s and service_name = '%s' and description = '%s'",
		 ledger_where,
		 service_name,
		 timlib_escape_single_quotes( buffer, description ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=hourly_service_work	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );
	service_work_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );

		service_work =
			customer_service_work_new(
				strdup( piece_buffer )
					/* begin_date_time */ );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		if ( *piece_buffer )
		{
			service_work->end_date_time = strdup( piece_buffer );

			service_work->work_hours =
				customer_get_work_hours(
					service_work->end_date_time,
					service_work->begin_date_time );

			*work_hours += service_work->work_hours;
		}

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );

		if ( *piece_buffer )
		{
			service_work->database_work_hours =
				atof( piece_buffer );
		}

		list_append_pointer( service_work_list, service_work );
	}

	pclose( input_pipe );
	return service_work_list;

} /* customer_hourly_service_work_get_list() */

boolean customer_service_work_open(
				LIST *service_work_list )
{
	SERVICE_WORK *service_work;

	if ( !list_rewind( service_work_list ) ) return 0;

	do {
		service_work = list_get_pointer( service_work_list );

		if ( !service_work->end_date_time ) return 1;

	} while( list_next( service_work_list ) );

	return 0;

} /* customer_service_work_open() */

boolean customer_fixed_service_open(
				LIST *fixed_service_sale_list )
{
	FIXED_SERVICE *fixed_service;

	if ( !list_rewind( fixed_service_sale_list ) ) return 0;

	do {
		fixed_service = list_get_pointer( fixed_service_sale_list );

		if ( customer_service_work_open(
				fixed_service->service_work_list ) )
		{
			return 1;
		}

	} while( list_next( fixed_service_sale_list ) );

	return 0;

} /* customer_fixed_service_open() */

boolean customer_hourly_service_open(
				LIST *hourly_service_sale_list )
{
	HOURLY_SERVICE *hourly_service;

	if ( !list_rewind( hourly_service_sale_list ) ) return 0;

	do {
		hourly_service = list_get_pointer( hourly_service_sale_list );

		if ( customer_service_work_open(
				hourly_service->service_work_list ) )
		{
			return 1;
		}

	} while( list_next( hourly_service_sale_list ) );

	return 0;

} /* customer_hourly_service_open() */

/* Returns fixed_service->work_hours */
/* --------------------------------- */
double customer_fixed_service_work_list_close(
				LIST *service_work_list,
				char *completed_date_time )
{
	SERVICE_WORK *service_work;
	double work_hours = 0.0;

	if ( !list_rewind( service_work_list ) ) return 0.0;

	do {
		service_work = list_get_pointer( service_work_list );

		if ( !service_work->end_date_time )
		{
			service_work->end_date_time = completed_date_time;

			service_work->work_hours =
				customer_get_work_hours(
					service_work->end_date_time,
					service_work->begin_date_time );
		}

		work_hours += service_work->work_hours;

	} while( list_next( service_work_list ) );

	return work_hours;

} /* customer_fixed_service_work_list_close() */

void customer_fixed_service_sale_list_close(
				LIST *fixed_service_sale_list,
				char *completed_date_time )
{
	FIXED_SERVICE *fixed_service;

	if ( !list_rewind( fixed_service_sale_list ) ) return;

	do {
		fixed_service = list_get_pointer( fixed_service_sale_list );

		fixed_service->work_hours =
			customer_fixed_service_work_list_close(
				fixed_service->service_work_list,
				completed_date_time );

	} while( list_next( fixed_service_sale_list ) );

} /* customer_fixed_service_sale_list_close() */

/* Returns hourly_service->work_hours */
/* ---------------------------------- */
double customer_hourly_service_work_list_close(
				LIST *service_work_list,
				char *completed_date_time )
{
	SERVICE_WORK *service_work;
	double work_hours = 0.0;

	if ( !list_rewind( service_work_list ) ) return 0.0;

	do {
		service_work = list_get_pointer( service_work_list );

		if ( !service_work->end_date_time )
		{
			service_work->end_date_time = completed_date_time;

			service_work->work_hours =
				customer_get_work_hours(
					service_work->end_date_time,
					service_work->begin_date_time );
		}

		work_hours += service_work->work_hours;

	} while( list_next( service_work_list ) );

	return work_hours;

} /* customer_hourly_service_work_list_close() */

void customer_hourly_service_sale_list_close(
				LIST *hourly_service_sale_list,
				char *completed_date_time )
{
	HOURLY_SERVICE *hourly_service;

	if ( !list_rewind( hourly_service_sale_list ) ) return;

	do {
		hourly_service = list_get_pointer( hourly_service_sale_list );

		hourly_service->work_hours =
			customer_hourly_service_work_list_close(
				hourly_service->service_work_list,
				completed_date_time );

		hourly_service->extension =
			CUSTOMER_HOURLY_SERVICE_GET_EXTENSION(
				hourly_service->hourly_rate,
				hourly_service->work_hours,
				hourly_service->discount_amount );

	} while( list_next( hourly_service_sale_list ) );

} /* customer_hourly_service_sale_list_close() */

void customer_fixed_service_work_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				char *begin_date_time,
				char *end_date_time,
				double work_hours,
				double database_work_hours )
{
	FILE *update_pipe;
	char *table_name;
	char *key_column_list_string;
	char sys_string[ 1024 ];

	if ( double_virtually_same( work_hours, database_work_hours )
	&&   !end_date_time )
	{
		return;
	}

	table_name =
		get_table_name(
			application_name,
			"fixed_service_work" );

	key_column_list_string =
	"full_name,street_address,sale_date_time,service_name,begin_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	update_pipe = popen( sys_string, "w" );

	if ( !double_virtually_same( work_hours, database_work_hours ) )
	{
		fprintf(update_pipe,
	 		"%s^%s^%s^%s^%s^work_hours^%.4lf\n",
	 		full_name,
	 		street_address,
	 		sale_date_time,
	 		service_name,
			begin_date_time,
	 		work_hours );
	}

	if ( end_date_time )
	{
		fprintf(update_pipe,
	 		"%s^%s^%s^%s^%s^end_date_time^%s\n",
	 		full_name,
	 		street_address,
	 		sale_date_time,
	 		service_name,
			begin_date_time,
	 		end_date_time );
	}

	pclose( update_pipe );

} /* customer_fixed_service_work_update() */

void customer_fixed_service_work_list_update(
				LIST *fixed_service_work_list,
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name )
{
	SERVICE_WORK *service_work;

	if ( !list_rewind( fixed_service_work_list ) ) return;

	do {
		service_work = list_get_pointer( fixed_service_work_list );

		customer_fixed_service_work_update(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			service_name,
			service_work->begin_date_time,
			service_work->end_date_time,
			service_work->work_hours,
			service_work->database_work_hours );

		service_work->database_work_hours =
			service_work->work_hours;

	}  while( list_next( fixed_service_work_list ) );

} /* customer_fixed_service_work_list_update() */

void customer_fixed_service_sale_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				double extension,
				double database_extension,
				double work_hours,
				double database_work_hours )
{
	FILE *update_pipe;
	char *table_name;
	char *key_column_list_string;
	char sys_string[ 1024 ];

	if ( dollar_virtually_same( extension, database_extension )
	&&   double_virtually_same( work_hours, database_work_hours ) )
	{
		return;
	}

	table_name =
		get_table_name(
			application_name,
			"fixed_service_sale" );

	key_column_list_string =
		"full_name,street_address,sale_date_time,service_name";

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
	 	sale_date_time,
	 	service_name,
	 	extension );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^work_hours^%.4lf\n",
	 	full_name,
	 	street_address,
	 	sale_date_time,
	 	service_name,
	 	work_hours );

	pclose( update_pipe );

} /* customer_fixed_service_sale_update() */

void customer_fixed_service_sale_list_update(
				LIST *fixed_service_sale_list,
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time )
{
	FIXED_SERVICE *fixed_service;

	if ( !list_rewind( fixed_service_sale_list ) ) return;

	do {
		fixed_service = list_get_pointer( fixed_service_sale_list );

		customer_fixed_service_sale_update(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			fixed_service->service_name,
			fixed_service->extension,
			fixed_service->database_extension,
			fixed_service->work_hours,
			fixed_service->database_work_hours );

		fixed_service->database_extension =
			fixed_service->extension;

		fixed_service->database_work_hours =
			fixed_service->work_hours;

		customer_fixed_service_work_list_update(
			fixed_service->service_work_list,
			application_name,
			full_name,
			street_address,
			sale_date_time,
			fixed_service->service_name );

	} while( list_next( fixed_service_sale_list ) );

} /* customer_fixed_service_sale_list_update() */

void customer_hourly_service_work_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				char *description,
				char *begin_date_time,
				char *end_date_time,
				double work_hours,
				double database_work_hours )
{
	FILE *update_pipe;
	char *table_name;
	char *key_column_list_string;
	char sys_string[ 1024 ];

	if ( double_virtually_same( work_hours, database_work_hours )
	&&   !end_date_time )
	{
		return;
	}

	table_name =
		get_table_name(
			application_name,
			"hourly_service_work" );

	key_column_list_string =
"full_name,street_address,sale_date_time,service_name,description,begin_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	update_pipe = popen( sys_string, "w" );

	if ( !double_virtually_same( work_hours, database_work_hours ) )
	{
		fprintf(update_pipe,
	 		"%s^%s^%s^%s^%s^%s^work_hours^%.4lf\n",
	 		full_name,
	 		street_address,
	 		sale_date_time,
	 		service_name,
			description,
			begin_date_time,
	 		work_hours );
	}

	if ( end_date_time )
	{
		fprintf(update_pipe,
	 		"%s^%s^%s^%s^%s^%s^end_date_time^%s\n",
	 		full_name,
	 		street_address,
	 		sale_date_time,
	 		service_name,
			description,
			begin_date_time,
	 		end_date_time );
	}

	pclose( update_pipe );

} /* customer_hourly_service_work_update() */

void customer_hourly_service_work_list_update(
				LIST *hourly_service_work_list,
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				char *description )
{
	SERVICE_WORK *service_work;

	if ( !list_rewind( hourly_service_work_list ) ) return;

	do {
		service_work = list_get_pointer( hourly_service_work_list );

		customer_hourly_service_work_update(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			service_name,
			description,
			service_work->begin_date_time,
			service_work->end_date_time,
			service_work->work_hours,
			service_work->database_work_hours );

		service_work->database_work_hours =
			service_work->work_hours;

	}  while( list_next( hourly_service_work_list ) );

} /* customer_hourly_service_work_list_update() */

void customer_hourly_service_sale_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				char *description,
				double extension,
				double database_extension,
				double work_hours,
				double database_work_hours )
{
	FILE *update_pipe;
	char *table_name;
	char *key_column_list_string;
	char sys_string[ 1024 ];

	if ( dollar_virtually_same( extension, database_extension )
	&&   double_virtually_same( work_hours, database_work_hours ) )
	{
		return;
	}

	table_name =
		get_table_name(
			application_name,
			"hourly_service_sale" );

	key_column_list_string =
	"full_name,street_address,sale_date_time,service_name,description";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	update_pipe = popen( sys_string, "w" );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^%s^extension^%.2lf\n",
	 	full_name,
	 	street_address,
	 	sale_date_time,
	 	service_name,
		description,
	 	extension );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^%s^work_hours^%.4lf\n",
	 	full_name,
	 	street_address,
	 	sale_date_time,
	 	service_name,
		description,
	 	work_hours );

	pclose( update_pipe );

} /* customer_hourly_service_sale_update() */

void customer_hourly_service_sale_list_update(
				LIST *hourly_service_sale_list,
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time )
{
	HOURLY_SERVICE *hourly_service;

	if ( !list_rewind( hourly_service_sale_list ) ) return;

	do {
		hourly_service = list_get_pointer( hourly_service_sale_list );

		customer_hourly_service_sale_update(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			hourly_service->service_name,
			hourly_service->description,
			hourly_service->extension,
			hourly_service->database_extension,
			hourly_service->work_hours,
			hourly_service->database_work_hours );

		hourly_service->database_extension =
			hourly_service->extension;

		hourly_service->database_work_hours =
			hourly_service->work_hours;

		customer_hourly_service_work_list_update(
			hourly_service->service_work_list,
			application_name,
			full_name,
			street_address,
			sale_date_time,
			hourly_service->service_name,
			hourly_service->description );

	} while( list_next( hourly_service_sale_list ) );

} /* customer_hourly_service_sale_list_update() */

/* Returns journal_ledger_list */
/* --------------------------- */
LIST *customer_sale_inventory_distinct_account_extract(
					double *sales_revenue_amount,
					LIST *inventory_sale_list )
{
	INVENTORY_SALE *inventory_sale;
	LIST *journal_ledger_list;
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( inventory_sale_list ) ) return (LIST *)0;

	journal_ledger_list = list_new();

	do {
		inventory_sale =
			list_get_pointer(
				inventory_sale_list );

		/* Debit cost of goods sold */
		/* ------------------------ */
		if ( !inventory_sale->cost_of_goods_sold_account_name )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: empty cost_of_goods_sold_account_name for (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 inventory_sale->inventory_name );
			exit( 1 );
		}

		journal_ledger =
			ledger_get_or_set_journal_ledger(
				journal_ledger_list,
				inventory_sale->
					cost_of_goods_sold_account_name );

		journal_ledger->debit_amount +=
			inventory_sale->cost_of_goods_sold;

		/* Credit inventory */
		/* ---------------- */
		if ( !inventory_sale->inventory_account_name )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: empty inventory_account_name for (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 inventory_sale->inventory_name );
			exit( 1 );
		}

		journal_ledger =
			ledger_get_or_set_journal_ledger(
				journal_ledger_list,
				inventory_sale->inventory_account_name );

		journal_ledger->credit_amount +=
			inventory_sale->cost_of_goods_sold;

		*sales_revenue_amount += inventory_sale->extension;

	} while( list_next( inventory_sale_list ) );

	return journal_ledger_list;

} /* customer_sale_inventory_distinct_account_extract() */

/* Returns journal_ledger_list */
/* --------------------------- */
LIST *customer_sale_fixed_service_distinct_account_extract(
					LIST *fixed_service_sale_list )
{
	FIXED_SERVICE *fixed_service;
	LIST *journal_ledger_list;
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( fixed_service_sale_list ) ) return (LIST *)0;

	journal_ledger_list = list_new();

	do {
		fixed_service =
			list_get_pointer(
				fixed_service_sale_list );

		if ( !fixed_service->account_name )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: empty account_name for (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 fixed_service->service_name );
			exit( 1 );
		}

		journal_ledger =
			ledger_get_or_set_journal_ledger(
				journal_ledger_list,
				fixed_service->
					account_name );

		journal_ledger->credit_amount +=
			fixed_service->extension;

	} while( list_next( fixed_service_sale_list ) );

	return journal_ledger_list;

} /* customer_sale_fixed_service_distinct_account_extract() */

/* Returns journal_ledger_list */
/* --------------------------- */
LIST *customer_sale_hourly_service_distinct_account_extract(
					LIST *hourly_service_sale_list )
{
	HOURLY_SERVICE *hourly_service;
	LIST *journal_ledger_list;
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( hourly_service_sale_list ) ) return (LIST *)0;

	journal_ledger_list = list_new();

	do {
		hourly_service =
			list_get_pointer(
				hourly_service_sale_list );

		if ( !hourly_service->account_name )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: empty account_name for (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 hourly_service->service_name );
			exit( 1 );
		}

		journal_ledger =
			ledger_get_or_set_journal_ledger(
				journal_ledger_list,
				hourly_service->
					account_name );

		journal_ledger->credit_amount +=
			hourly_service->extension;

	} while( list_next( hourly_service_sale_list ) );

	return journal_ledger_list;

} /* customer_sale_hourly_service_distinct_account_extract() */

