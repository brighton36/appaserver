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
void depreciate_fixed_assets_undo(	char *application_name,
					char *fund_name,
					char *depreciation_date );

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
	char buffer[ 128 ];
	boolean undo;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;

	if ( argc != 6 )
	{
		fprintf(stderr,
	"Usage: %s application process fund undo_yn execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	fund_name = argv[ 3 ];
	undo = (*argv[ 4 ]) == 'y';
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
		printf( "<h1>%s</h1>\n",
			format_initial_capital(
				buffer,
				process_name ) );

		if ( undo )
		{
			depreciation_date =
				depreciation_fetch_max_depreciation_date(
					application_name,
					fund_name );

			depreciate_fixed_assets_undo(
				application_name,
				fund_name,
				depreciation_date );

			printf(
			"<h3>Depreciation posted on %s is now deleted.</h3>\n",
				depreciation_date );
		}
		else
		{
			depreciation_date = pipe2string( "now.sh ymd" );

			if ( depreciation_date_exists(
				application_name,
				fund_name,
				depreciation_date ) )
			{
				printf(
		"<h3>Error: depreciation date exists for today.</h3>\n" );
			}
			else
			{
				depreciate_fixed_assets_execute(
					application_name,
					fund_name,
					depreciation_date );

				printf(
				"<h3>Depreciation now posted on %s.</h3>\n",
				depreciation_date );
			}
		}
	}
	else
	{
		if ( undo )
		{
			depreciation_date =
				depreciation_fetch_max_depreciation_date(
					application_name,
					fund_name );

			printf( "<h1>%s</h1>\n",
				format_initial_capital(
					buffer,
					process_name ) );

			if ( !depreciation_date
			||   !*depreciation_date )
			{
				printf(
			"<h3>No depreciations are found.</h3>\n" );
			}
			else
			{
				printf(
			"<h3>Will undo depreciation posted on %s.</h3>\n",
					depreciation_date );
			}
		}
		else
		{
			depreciation_date = pipe2string( "now.sh ymd" );

			if ( depreciation_date_exists(
				application_name,
				fund_name,
				depreciation_date ) )
			{
				printf( "<h1>%s</h1>\n",
					format_initial_capital(
						buffer,
						process_name ) );

				printf(
		"<h3>Error: depreciation date exists for today.</h3>\n" );
			}
			else
			{
				depreciate_fixed_assets_display(
					application_name,
					fund_name,
					depreciation_date,
					process_name );
			}
		}
	}

	document_close();

	return 0;

} /* main() */

void depreciate_fixed_assets_execute(	char *application_name,
					char *fund_name,
					char *depreciation_date )
{
	DEPRECIATE_FIXED_ASSET *fixed_asset_depreciation;

	fixed_asset_depreciation =
		depreciation_fixed_asset_depreciation_new(
			application_name,
			fund_name,
			depreciation_date );

	depreciation_fixed_asset_set_depreciation(
		fixed_asset_depreciation->entity_list,
		depreciation_date );

	depreciation_fixed_asset_set_transaction(
		fixed_asset_depreciation->entity_list );

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
	DEPRECIATE_FIXED_ASSET *fixed_asset_depreciation;

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

void depreciate_fixed_assets_undo(	char *application_name,
					char *fund_name,
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
		 "accumulated_depreciation_reset.sh %s",
		 application_name );

	system( sys_string );

} /* depreciate_fixed_assets_undo() */

