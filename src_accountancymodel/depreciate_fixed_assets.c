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
#include "document.h"
#include "appaserver_library.h"
#include "application.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "depreciation.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void depreciate_fixed_assets_execute(	char *application_name,
					char *fund_name,
					char *depreciation_date );

void depreciate_fixed_assets_display(	char *application_name,
					char *fund_name,
					char *depreciation_date,
					char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};
	char *process_name;
	char *fund_name;
	char *depreciation_date;
	boolean execute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;

	if ( argc != 6 )
	{
		fprintf(stderr,
	"Usage: %s application process fund depreciation_date execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	fund_name = argv[ 3 ];
	depreciation_date = argv[ 4 ];
	execute = (*argv[ 5 ]) == 'y';

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

	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( *depreciation_date
	&&   strcmp( depreciation_date, "depreciation_date" ) == 0 )
	{
		depreciation_date = pipe2string( "now.sh ymd" );
	}

	document = document_new( process_name /* title */, application_name );
	document->output_content_type = 1;
	
	document_output_heading(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->
				appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );
	
	document_output_body(	document->application_name,
				document->onload_control_string );

	if ( execute )
	{
		depreciate_fixed_assets_execute(
			application_name,
			fund_name,
			depreciation_date );
	}
	else
	{
		depreciate_fixed_assets_display(
			application_name,
			fund_name,
			depreciation_date,
			process_name );
	}

	document_close();

	return 0;

} /* main() */

void depreciate_fixed_assets_execute(	char *application_name,
					char *fund_name,
					char *depreciation_date )
{
	FIXED_ASSET_DEPRECIATION *fixed_asset_depreciation;

	fixed_asset_depreciation =
		depreciation_fixed_asset_depreciation_new(
			application_name,
			fund_name,
			depreciation_date );

	depreciation_fixed_asset_set_depreciation(
		fixed_asset_depreciation->entity_list,
		depreciation_date );

	depreciation_fixed_asset_execute(
		fixed_asset_depreciation->entity_list,
		application_name,
		fund_name,
		depreciation_date );

} /* depreciate_fixed_assets_execute() */

void depreciate_fixed_assets_display(	char *application_name,
					char *fund_name,
					char *depreciation_date,
					char *process_name )
{
	FIXED_ASSET_DEPRECIATION *fixed_asset_depreciation;

	fixed_asset_depreciation =
		depreciation_fixed_asset_depreciation_new(
			application_name,
			fund_name,
			depreciation_date );

	depreciation_fixed_asset_set_depreciation(
		fixed_asset_depreciation->entity_list,
		depreciation_date );

/*
	depreciation_fixed_asset_depreciation_tree_display(
		fixed_asset_depreciation->entity_list );
*/

	depreciation_fixed_asset_depreciation_table_display(
		process_name,
		fixed_asset_depreciation->entity_list );

} /* depreciate_fixed_assets_display() */

