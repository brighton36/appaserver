/* ---------------------------------------------------------------	*/
/* src_predictive/inventory_cost_method_propagate.c			*/
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
#include "inventory.h"
#include "entity.h"
#include "ledger.h"
#include "purchase.h"
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void inventory_cost_method_propagate(
			char *application_name );

/*
void inventory_cost_method_propagate_inventory(
			char *application_name,
			char *inventory_name );
*/

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( argc != 2 )
	{
		fprintf( stderr,
			 "Usage: %s application\n",
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

	inventory_cost_method_propagate( application_name );

	return 0;

} /* main() */

void inventory_cost_method_propagate(
			char *application_name )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char inventory_name[ 128 ];

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=inventory_name	"
		 "			folder=inventory	",
		 application_name );

	input_pipe = popen( sys_string, "r" );

	while( get_line( inventory_name, input_pipe ) )
	{
		sprintf( sys_string,
		"propagate_inventory_sale_layers %s '' '' '' \"%s\" '' n",
			 application_name,
			 inventory_name );

		system( sys_string );
	}

	pclose( input_pipe );

} /* inventory_cost_method_propagate() */

