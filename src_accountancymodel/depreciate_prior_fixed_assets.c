/* --------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/depreciate_prior_fixed_assets.c */
/* --------------------------------------------------------------------- */
/* 									 */
/* Freely available software: see Appaserver.org			 */
/* --------------------------------------------------------------------- */

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
#include "entity.h"
#include "depreciation.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void depreciate_prior_fixed_assets_process_display(
					char *application_name,
					char *process_name,
					boolean undo );

void depreciate_prior_fixed_assets_process_execute(
					char *application_name,
					char *fund_name,
					char *process_name,
					boolean undo );

void depreciate_prior_fixed_assets_undo(char *application_name,
					char *fund_name,
					char *depreciation_date );

boolean depreciate_prior_fixed_assets_execute(
					char *application_name,
					char *fund_name,
					char *prior_depreciation_date,
					char *depreciation_date );

boolean depreciate_prior_fixed_assets_display(
					char *application_name,
					char *prior_depreciation_date,
					char *depreciation_date,
					char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};
	char *process_name;
	char *fund_name;
	boolean execute;
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
		depreciate_prior_fixed_assets_process_execute(
			application_name,
			fund_name,
			process_name,
			undo );
	}
	else
	{
		depreciate_prior_fixed_assets_process_display(
			application_name,
			process_name,
			undo );
	}

	document_close();

	return 0;

} /* main() */

void depreciate_prior_fixed_assets_process_execute(
					char *application_name,
					char *fund_name,
					char *process_name,
					boolean undo )
{
	char buffer[ 128 ];
	char *depreciation_date;
	char *prior_depreciation_date;

	printf( "<h1>%s</h1>\n",
		format_initial_capital(
			buffer,
			process_name ) );

	if ( undo )
	{
		depreciation_date =
			depreciation_prior_fetch_max_depreciation_date(
				application_name );

		depreciate_prior_fixed_assets_undo(
			application_name,
			fund_name,
			depreciation_date );

		printf(
		"<h3>Depreciation posted on %s is now deleted.</h3>\n",
			depreciation_date );

		return;
	}

	depreciation_date = pipe2string( "now.sh ymd" );

	if ( depreciation_date_prior_exists(
		application_name,
		depreciation_date ) )
	{
		printf(
	"<h3>Error: depreciation date exists for today.</h3>\n" );

		return;
	}

	prior_depreciation_date =
		depreciation_prior_fetch_max_depreciation_date(
			application_name );

	if ( !depreciate_prior_fixed_assets_execute(
		application_name,
		fund_name,
		prior_depreciation_date,
		depreciation_date ) )
	{
		printf(
		"<h3>Error: no prior fixed assets in database.</h3>\n" );
	}
	else
	{
		printf( "<h3>Depreciation now posted on %s.</h3>\n",
			depreciation_date );
	}

} /* depreciate_prior_fixed_assets_process_execute() */

void depreciate_prior_fixed_assets_process_display(
					char *application_name,
					char *process_name,
					boolean undo )
{
	char buffer[ 128 ];
	char *depreciation_date;
	char *prior_depreciation_date;

	if ( undo )
	{
		depreciation_date =
			depreciation_prior_fetch_max_depreciation_date(
				application_name );

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

		return;
	}

	depreciation_date = pipe2string( "now.sh ymd" );

	if ( depreciation_date_prior_exists(
		application_name,
		depreciation_date ) )
	{
		printf( "<h1>%s</h1>\n",
			format_initial_capital(
				buffer,
				process_name ) );

		printf(
	"<h3>Error: depreciation date exists for today.</h3>\n" );

		return;
	}

	prior_depreciation_date =
		depreciation_prior_fetch_max_depreciation_date(
			application_name );

	if ( !depreciate_prior_fixed_assets_display(
		application_name,
		prior_depreciation_date,
		depreciation_date,
		process_name ) )
	{
		printf(
	"<h3>Error: no prior fixed assets in database.</h3>\n" );
	}

} /* depreciate_prior_fixed_assets_process_display() */

boolean depreciate_prior_fixed_assets_execute(
					char *application_name,
					char *fund_name,
					char *prior_depreciation_date,
					char *depreciation_date )
{
	FILE *output_pipe;
	FILE *debit_account_pipe = {0};
	FILE *credit_account_pipe = {0};
	char *depreciation_expense_account = {0};
	char *accumulated_depreciation_account = {0};
	ENTITY_SELF *entity_self;
	char *transaction_date_time;
	char sys_string[ 1024 ];
	DEPRECIATE_PRIOR_FIXED_ASSET_DEPRECIATION *
		depreciate_prior_fixed_asset_depreciation;

	if ( ! ( entity_self = entity_self_load( application_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load entity self.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	transaction_date_time =
		ledger_get_transaction_date_time(
			(char *)0 /* transaction_date */ );

	depreciate_prior_fixed_asset_depreciation =
		depreciate_prior_fixed_asset_depreciation_new(
			application_name,
			entity_self->entity->full_name,
			entity_self->entity->street_address );

	if ( !list_length( 
		depreciate_prior_fixed_asset_depreciation->
			depreciate_prior_fixed_asset_list ) )
	{
		return 0;
	}

	depreciation_prior_fixed_asset_set_depreciation(
		&depreciate_prior_fixed_asset_depreciation->
			depreciation_amount,
		depreciate_prior_fixed_asset_depreciation->
			depreciate_prior_fixed_asset_list,
		prior_depreciation_date,
		depreciation_date );

	depreciation_prior_fixed_asset_insert_depreciation(
			depreciate_prior_fixed_asset_depreciation->
				self_full_name,
			depreciate_prior_fixed_asset_depreciation->
				self_street_address,
			depreciate_prior_fixed_asset_depreciation->
				depreciate_prior_fixed_asset_list,
			depreciation_date,
			transaction_date_time );

	output_pipe =
		ledger_transaction_insert_open_stream(
			application_name );

	ledger_transaction_insert_stream(
		output_pipe,
		depreciate_prior_fixed_asset_depreciation->
			self_full_name,
		depreciate_prior_fixed_asset_depreciation->
			self_street_address,
		transaction_date_time,
		depreciate_prior_fixed_asset_depreciation->
			depreciation_amount
				/* transaction_amount */,
		DEPRECIATION_MEMO,
		0 /* check_number */,
		1 /* lock_transaction */ );

	pclose( output_pipe );

	/* Error with an exit if failure. */
	/* ------------------------------ */
	ledger_get_depreciation_account_names(
		&depreciation_expense_account,
		&accumulated_depreciation_account,
		application_name,
		fund_name );

	ledger_journal_insert_open_stream(
		&debit_account_pipe,
		&credit_account_pipe,
		application_name );

	ledger_journal_insert_stream(
		debit_account_pipe,
		credit_account_pipe,
		entity_self->entity->full_name,
		entity_self->entity->street_address,
		transaction_date_time,
		depreciate_prior_fixed_asset_depreciation->
			depreciation_amount,
		depreciation_expense_account
			/* debit_account_name */,
		accumulated_depreciation_account
			/* credit_account_name */ );

	pclose( debit_account_pipe );
	pclose( credit_account_pipe );

	sprintf( sys_string,
		 "ledger_propagate %s \"%s\" '' \"%s\" \"%s\"",
		 application_name,
		 transaction_date_time,
		 depreciation_expense_account,
		 accumulated_depreciation_account );

	system( sys_string );

	sprintf( sys_string,
		 "accumulated_depreciation_reset.sh %s",
		 application_name );

	system( sys_string );

	return 1;

} /* depreciate_prior_fixed_assets_execute() */

boolean depreciate_prior_fixed_assets_display(
					char *application_name,
					char *prior_depreciation_date,
					char *depreciation_date,
					char *process_name )
{
	ENTITY_SELF *entity_self;
	DEPRECIATE_PRIOR_FIXED_ASSET_DEPRECIATION *
		depreciate_prior_fixed_asset_depreciation;

	if ( ! ( entity_self = entity_self_load( application_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load entity self.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	depreciate_prior_fixed_asset_depreciation =
		depreciate_prior_fixed_asset_depreciation_new(
			application_name,
			entity_self->entity->full_name,
			entity_self->entity->street_address );

	if ( !list_length( 
		depreciate_prior_fixed_asset_depreciation->
			depreciate_prior_fixed_asset_list ) )
	{
		return 0;
	}

	depreciation_prior_fixed_asset_set_depreciation(
		&depreciate_prior_fixed_asset_depreciation->
			depreciation_amount,
		depreciate_prior_fixed_asset_depreciation->
			depreciate_prior_fixed_asset_list,
		prior_depreciation_date,
		depreciation_date );

	depreciation_prior_fixed_asset_table_display(
		process_name,
		depreciate_prior_fixed_asset_depreciation->
			depreciate_prior_fixed_asset_list );

	return 1;

} /* depreciate_prior_fixed_assets_display() */

void depreciate_prior_fixed_assets_undo(char *application_name,
					char *fund_name,
					char *depreciation_date )
{
	char *transaction_date_time;
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *depreciation_expense_account = {0};
	char *accumulated_depreciation_account = {0};
	FILE *output_pipe;
	char *folder_name;

	folder_name = "prior_fixed_asset_depreciation";

	sprintf(where,
		"depreciation_date = '%s'",
		depreciation_date );

	/* They all have the same transaction_date_time */
	/* -------------------------------------------- */
	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=transaction_date_time	 "
		 "			folder=%s			 "
		 "			where=\"%s\"			 "
		 "			order=select			|"
		 "head -1						 ",
		 application_name,
		 folder_name,
		 where );

	if ( ! ( transaction_date_time = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get transaction_date_time.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	output_pipe = popen( "sql.e", "w" );

	sprintf(where,
		"transaction_date_time = '%s'",
		transaction_date_time );

	fprintf( output_pipe,
	 	 "delete from journal_ledger where %s;\n",
	 	 where );

	fprintf( output_pipe,
	 	 "delete from transaction where %s;\n",
	 	 where );

	fprintf( output_pipe,
	 	 "delete from prior_fixed_asset_depreciation where %s;\n",
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
		 transaction_date_time,
		 depreciation_expense_account,
		 accumulated_depreciation_account );

	system( sys_string );

	sprintf( sys_string,
		 "accumulated_depreciation_reset.sh %s",
		 application_name );

	system( sys_string );

} /* depreciate_prior_fixed_assets_undo() */

