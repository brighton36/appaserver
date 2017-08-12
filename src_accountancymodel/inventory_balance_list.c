/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/inventory_balance_list.c	*/
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
#define OUTPUT_PROCESS		"delimiter2padded_columns.e '^'"

/* Prototypes */
/* ---------- */
void inventory_balance_list(	char *application_name,
				INVENTORY *inventory );

int main( int argc, char **argv )
{
	char *application_name;
	char *inventory_name;
	ENTITY_SELF *entity_self;
	char *database_string = {0};

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s application inventory_name\n",
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
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	inventory_name = argv[ 2 ];

	entity_self =
		entity_self_sale_inventory_load(
			application_name,
			inventory_name );

	inventory_balance_list(
			application_name,
			entity_self->sale_inventory
				/* inventory */ );

	return 0;

} /* main() */

void inventory_balance_list(	char *application_name,
				INVENTORY *inventory )
{
	FILE *output_pipe;

	output_pipe = popen( OUTPUT_PROCESS, "w" );

	inventory->inventory_balance_list =
		inventory_get_average_cost_inventory_balance_list(
			inventory->inventory_purchase_list,
			inventory->inventory_sale_list );

	printf( "\nInventory Average Balance List:\n" );
	
	inventory_balance_list_table_display(
		output_pipe,
		inventory->inventory_balance_list );

	pclose( output_pipe );

	output_pipe = popen( OUTPUT_PROCESS, "w" );

	printf( "\nINVENTORY:\n" );
	
	inventory_folder_table_display(
		output_pipe,
		application_name,
		inventory->inventory_name );

	pclose( output_pipe );

} /* inventory_balance_list_display() */

