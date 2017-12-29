/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_change_self.c			*/
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

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};
	char *preupdate_inventory_cost_method_string;
	ENTITY_SELF *self;
	enum inventory_cost_method preupdate_inventory_cost_method;
	char sys_string[ 128 ];

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( argc != 3 )
	{
		fprintf( stderr,
"Usage: %s application preupdate_inventory_cost_method\n",
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

	preupdate_inventory_cost_method_string = argv[ 2 ];

	if ( strcmp(	preupdate_inventory_cost_method_string,
			"preupdate_inventory_cost_method" ) == 0 )
	{
		exit( 0 );
	}

	preupdate_inventory_cost_method =
		entity_get_inventory_cost_method(
			preupdate_inventory_cost_method_string );

	if ( ! ( self = entity_self_load( application_name ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot fetch from SELF.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ( self->inventory_cost_method != inventory_not_set )
	&&   (	self->inventory_cost_method !=
		preupdate_inventory_cost_method ) )
	{
		sprintf( sys_string,
			 "inventory_cost_method_propagate %s",
		 	 timlib_get_parameter_application_name(
		 		application_name,
				database_string ) );
		system( sys_string );
	}

	return 0;

} /* main() */

