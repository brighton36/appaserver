/* ---------------------------------------------------------------	*/
/* src_predictive/propagate_customer_sale_accounts.c			*/
/* ---------------------------------------------------------------	*/
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
#include "ledger.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *transaction_date_time;
	char *sales_revenue_account = {0};
	char *service_revenue_account = {0};
	char *sales_tax_payable_account = {0};
	char *shipping_revenue_account = {0};
	char *cost_of_goods_sold_account = {0};
	char *receivable_account;
	char *database_string = {0};

	if ( argc != 4 )
	{
		fprintf( stderr, 
			 "Usage: %s application fund transaction_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	fund_name = argv[ 2 ];
	transaction_date_time = argv[ 3 ];

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

	customer_propagate_customer_sale_ledger_accounts(
				application_name,
				fund_name,
				transaction_date_time );

	return 0;

} /* main() */

