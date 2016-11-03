/* ---------------------------------------------------------------	*/
/* src_accountancymodel/propagate_purchase_order_accounts.c		*/
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
	char *inventory_account = {0};
	char *sales_tax_expense_account = {0};
	char *freight_in_expense_account = {0};
	char *account_payable_account = {0};
	char *cash_account = {0};
	char *uncleared_checks_account = {0};

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

	ledger_get_purchase_order_account_names(
		&inventory_account,
		&sales_tax_expense_account,
		&freight_in_expense_account,
		&account_payable_account,
		&cash_account,
		&uncleared_checks_account,
		application_name,
		fund_name );

	if ( !inventory_account )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot get purchase order account names.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	ledger_propagate(	application_name,
				transaction_date_time,
				inventory_account );

	ledger_propagate(	application_name,
				transaction_date_time,
				sales_tax_expense_account );

	ledger_propagate(	application_name,
				transaction_date_time,
				freight_in_expense_account );

	ledger_propagate(	application_name,
				transaction_date_time,
				account_payable_account );

	ledger_propagate(	application_name,
				transaction_date_time,
				cash_account );

	ledger_propagate(	application_name,
				transaction_date_time,
				uncleared_checks_account );

	return 0;

} /* main() */

