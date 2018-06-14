/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/depreciate_fixed_assets.c		*/
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
#include "entity.h"
#include "appaserver_parameter_file.h"
#include "depreciation.h"

/* Constants */
/* --------- */
#define FIXED_ASSET_DEPRECIATION_FOLDER		"fixed_asset_depreciation"
#define PRIOR_FIXED_ASSET_DEPRECIATION_FOLDER	"prior_fixed_asset_depreciation"

/* Prototypes */
/* ---------- */
void depreciate_fixed_assets(		char *application_name,
					char *process_name,
					boolean undo,
					boolean execute );

/*
void depreciate_purchased_fixed_assets_display(
					char *application_name,
					char *process_name,
					char *depreciation_date,
					char *prior_depreciation_date );

void depreciate_purchased_fixed_assets(
					char *application_name,
					boolean undo,
					boolean execute );

void depreciate_purchased_fixed_assets_undo(
					char *application_name,
					char *depreciation_date );

boolean depreciate_purchased_fixed_assets_execute(
					char *application_name,
					char *depreciation_date,
					char *prior_depreciation_date );
*/

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	boolean execute;
	boolean undo;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
	"Usage: %s ignored process undo_yn execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 2 ];
	undo = (*argv[ 3 ]) == 'y';
	execute = (*argv[ 4 ]) == 'y';

	appaserver_parameter_file = appaserver_parameter_file_new();

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

	depreciate_fixed_assets(
		application_name,
		process_name,
		undo,
		execute );

	document_close();

	return 0;

} /* main() */

void depreciate_fixed_assets(	char *application_name,
				char *process_name,
				boolean undo,
				boolean execute )
{
	char buffer[ 128 ];
	char *depreciation_date;
	char *prior_depreciation_date;
	ENTITY_SELF *entity_self;

	depreciation_date =
	prior_depreciation_date =
		depreciation_fetch_max_depreciation_date(
			application_name,
			FIXED_ASSET_DEPRECIATION_FOLDER );

	if ( ! ( entity_self =
			entity_self_load(
				application_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load entity self.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Input */
	/* ----- */
	if ( undo )
	{
		if ( !depreciation_date || !*depreciation_date )
		{
			printf(
		"<h3>Error: no depreciated purchased fixed assets to undo.\n" );
			return;
		}
	}
	else
	{
		depreciation_date = pipe2string( "now.sh ymd" );

		if ( depreciation_date_exists(
			application_name,
			FIXED_ASSET_DEPRECIATION_FOLDER,
			depreciation_date ) )
		{
			printf(
"<h3>Error: depreciation date for purchased fixed assets exists for today.</h3>\n" );

			return;
		}
	}

	/* Process */
	/* ------- */
	if ( execute )
	{
		printf( "<h1>%s</h1>\n",
			format_initial_capital(
				buffer,
				process_name ) );

		if ( undo )
		{
/*
			depreciate_purchased_fixed_assets_undo(
				application_name,
				depreciation_date );

			printf(
"<h3>Depreciation of purchased fixed assets posted on %s is now deleted.</h3>\n",
				depreciation_date );
*/
			printf(
"<h3>Undo is not ready yet.</h3>\n" );
		}
		else
		{
			entity_self->depreciation_fund_list =
				depreciation_fetch_fund_list(
					application_name,
					depreciation_date,
					prior_depreciation_date,
					1 /* with_load */ );

			if ( !depreciation_fixed_assets_execute(
				application_name,
				entity_self->depreciation_fund_list ) )	
			{
				printf(
		"<h3>Error: no fixed asset to depreciate.</h3>\n" );
			}
			else
			{
				printf(
				"<h3>Depreciation now posted on %s.</h3>\n",
				depreciation_date );
			}
		}
	}
	else
	/* Must be display */
	/* --------------- */
	{
		if ( undo )
		{
			printf(
	"<h3>Will delete the depreciation taken place on %s.</h3>\n",
				depreciation_date );
		}
		else
		{
			depreciation_depreciation_fund_list_table_display(
				process_name,
				entity_self->depreciation_fund_list );
		}
	}

} /* depreciate_fixed_assets() */

#ifdef NOT_DEFINED
boolean depreciate_purchased_fixed_assets_execute(
					char *application_name,
					char *depreciation_date,
					char *prior_depreciation_date )
{
	ENTITY_SELF *entity_self;

	if ( ! ( entity_self =
			entity_self_load(
				application_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load entity self.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	entity_self->depreciation_fund_list =
		depreciation_fetch_fund_list(
			application_name );

	entity_self->fixed_asset_purchased_list =
		fixed_asset_depreciation_purchase_fetch_list(
			application_name );

	if ( !list_length( entity_self->fixed_asset_purchased_list )
		return 0;

	depreciation_fixed_asset_list_set_depreciation(
		entity_self->fixed_asset_purchased_list,
		&entity_self->purchased_fixed_asset_depreciation_amount,
		depreciation_date,
		prior_depreciation_date );

	depreciation_fixed_asset_set_transaction(
		fixed_asset_depreciation->entity_list );

	depreciation_fixed_asset_execute(
		fixed_asset_depreciation->entity_list,
		application_name,
		fund_name,
		depreciation_date );

	return 1;

} /* depreciate_purchased_fixed_assets_execute() */

void depreciate_fixed_assets_undo(	char *application_name,
					char *depreciation_date )
{
	char transaction_date_time[ 64 ];
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *depreciation_expense_account = {0};
	char *accumulated_depreciation_account = {0};
	char *propagate_transaction_date_time = {0};
	FILE *input_pipe;
	FILE *output_pipe;

	sprintf(where,
		"depreciation_date = '%s'",
		depreciation_date );

	output_pipe = popen( "sql.e", "w" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=transaction_date_time	"
		 "			folder=depreciation		"
		 "			where=\"%s\"			"
		 "			order=select			",
		 application_name,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( transaction_date_time, input_pipe ) )
	{
		sprintf(where,
			"transaction_date_time = '%s'",
			transaction_date_time );

		fprintf( output_pipe,
		 	 "delete from journal_ledger where %s;\n",
		 	 where );

		fprintf( output_pipe,
		 	 "delete from transaction where %s;\n",
		 	 where );

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
				strdup( transaction_date_time );
		}
	}

	pclose( input_pipe );

	fprintf( output_pipe,
	 	 "delete from depreciation where %s;\n",
	 	 where );

	pclose( output_pipe );

	/* Error with an exit if failure. */
	/* ------------------------------ */
	ledger_get_depreciation_account_names(
		&depreciation_expense_account,
		&accumulated_depreciation_account,
		application_name,
		fund_name );

	sprintf( sys_string,
		 "ledger_propagate %s \"%s\" '' \"%s\" \"%s\"",
		 application_name,
		 propagate_transaction_date_time,
		 depreciation_expense_account,
		 accumulated_depreciation_account );

	system( sys_string );

	sprintf( sys_string,
		 "finance_accumulated_depreciation_reset.sh %s",
		 application_name );

	system( sys_string );

} /* depreciate_fixed_assets_undo() */
#endif

