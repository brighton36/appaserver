/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/depreciate_fixed_assets.c	*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "boolean.h"
#include "list.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "depreciation.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void depreciate_fixed_assets_execute( char *application_name );

void depreciate_fixed_assets_display( char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};
	boolean execute;

	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: %s application execute_yn\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	execute = (*argv[ 2 ]) == 'y';

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

	if ( execute )
		depreciate_fixed_assets_execute( application_name );
	else
		depreciate_fixed_assets_display( application_name );

	return 0;

} /* main() */

void depreciate_fixed_assets_execute( char *application_name )
{

} /* depreciate_fixed_assets_execute() */

void depreciate_fixed_assets_display( char *application_name )
{
	FIXED_ASSET_DEPRECIATION *fixed_asset_depreciation;

	fixed_asset_depreciation =
		depreciation_fixed_asset_depreciation_new(
			application_name );

} /* depreciate_fixed_assets_display() */


