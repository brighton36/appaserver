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
void depreciate_fixed_assets_execute(	char *application_name,
					char *depreciation_date );

void depreciate_fixed_assets_display(	char *application_name,
					char *depreciation_date );

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};
	boolean execute;
	char *depreciation_date;

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
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	depreciation_date = pipe2string( "now.sh ymd" );

	if ( execute )
	{
		depreciate_fixed_assets_execute(
			application_name,
			depreciation_date );
	}
	else
	{
		depreciate_fixed_assets_display(
			application_name,
			depreciation_date );
	}

	return 0;

} /* main() */

void depreciate_fixed_assets_execute(	char *application_name,
					char *depreciation_date )
{
	FIXED_ASSET_DEPRECIATION *fixed_asset_depreciation;

	fixed_asset_depreciation =
		depreciation_fixed_asset_depreciation_new(
			application_name );

} /* depreciate_fixed_assets_execute() */

void depreciate_fixed_assets_display(	char *application_name,
					char *depreciation_date )
{
	FIXED_ASSET_DEPRECIATION *fixed_asset_depreciation;

	fixed_asset_depreciation =
		depreciation_fixed_asset_depreciation_new(
			application_name );

	depreciation_fixed_asset_set_depreciation(
		fixed_asset_depreciation->entity_list,
		depreciation_date );

} /* depreciate_fixed_assets_display() */

