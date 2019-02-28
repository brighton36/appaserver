/* src_autorepair/autorepair.c				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "piece.h"
#include "ledger.h"
#include "autorepair.h"

double autorepair_get_total_payment(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time )
{
	return ledger_get_total_payment(
				application_name,
				full_name,
				street_address,
				sale_date_time );
#ifdef NOT_DEFINED
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

#endif
} /* autorepair_get_total_payment() */

double autorepair_get_sales_tax(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time )
{
	return autorepair_get_sales_tax(
				application_name,
				full_name,
				street_address,
				sale_date_time );
#ifdef NOT_DEFINED
	char sys_string[ 1024 ];
	char *where;
	char *results;
	char *select = "sales_tax";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=customer_sale	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0.0;

	return atof( results );
#endif

} /* autorepair_get_sales_tax() */

AUTOREPAIR_CUSTOMER_SALE *autorepair_customer_sale_new(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *completed_date_time,
				char *vehicle_make,
				char *vehicle_model,
				char *vehicle_trim,
				int vehicle_year )
{
	AUTOREPAIR_CUSTOMER_SALE *customer_sale;

	if ( ! ( customer_sale =
			calloc( 1, sizeof( AUTOREPAIR_CUSTOMER_SALE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	customer_sale->full_name = full_name;
	customer_sale->street_address = street_address;
	customer_sale->sale_date_time = sale_date_time;
	customer_sale->completed_date_time = completed_date_time;
	customer_sale->vehicle_make = vehicle_make;
	customer_sale->vehicle_model = vehicle_model;
	customer_sale->vehicle_trim = vehicle_trim;
	customer_sale->vehicle_year = vehicle_year;

	if ( !autorepair_customer_sale_load(
			&customer_sale->odometer_miles,
			&customer_sale->symptom,
			&customer_sale->mechanic_full_name,
			application_name,
			customer_sale->full_name,
			customer_sale->street_address,
			customer_sale->sale_date_time ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot load customer sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return customer_sale;

} /* autorepair_customer_sale_new() */

boolean autorepair_customer_sale_load(
			int *odometer_miles,
			char **symptom,
			char **mechanic_full_name,
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char *results;
	char piece_buffer[ 2048 ];

	select = "odometer_miles,symptom,mechanic_full_name";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=customer_sale	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		results,
		0 );
	*odometer_miles = atoi( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		results,
		0 );
	*odometer_miles = atoi( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		results,
		1 );
	*symptom = strdup( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		results,
		2 );
	*mechanic_full_name = strdup( piece_buffer );

	free( results );
	return 1;

} /* autorepair_customer_sale_load() */

