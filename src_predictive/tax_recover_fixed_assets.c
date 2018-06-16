/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/tax_recover_fixed_assets.c		*/
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

/* Include tax_recovery.h in fixed_asset.h */
/* --------------------------------------- */
#include "fixed_asset.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void tax_recover_fixed_assets(		char *application_name,
					boolean undo,
					boolean execute );

void tax_recover_fixed_assets_undo(	char *application_name,
					int recovery_year,
					char *folder_name );

void tax_recover_fixed_assets_insert(	LIST *fixed_asset_purchased_list,
					LIST *fixed_asset_prior_list );

void tax_recover_fixed_asset_list_insert(
					FILE *output_pipe,
					LIST *fixed_asset_list );

void tax_recover_fixed_assets_display(	LIST *fixed_asset_purchased_list,
					LIST *fixed_asset_prior_list );

void tax_recover_fixed_asset_list_display(
					FILE *output_pipe,
					LIST *fixed_asset_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	boolean execute;
	char buffer[ 128 ];
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

	printf( "<h1>%s</h1>\n",
		format_initial_capital(
			buffer,
			process_name ) );

	tax_recover_fixed_assets(
		application_name,
		undo,
		execute );

	document_close();

	return 0;

} /* main() */

void tax_recover_fixed_assets(	char *application_name,
				boolean undo,
				boolean execute )
{
	int recovery_year;
	int now_year;
	LIST *fixed_asset_purchased_list = {0};
	LIST *fixed_asset_prior_list = {0};

	/* Input */
	/* ----- */
	now_year = atoi( date_get_now_yyyy_mm_dd( date_get_utc_offset() ) );

	recovery_year =
		tax_recovery_fetch_max_recovery_year(
			application_name,
			"tax_fixed_asset_recovery"
				/* folder_name */ );

	if ( !recovery_year )
	{
		recovery_year =
			tax_recovery_fetch_max_recovery_year(
				application_name,
				"tax_prior_fixed_asset_recovery"
					/* folder_name */ );
	}

	if ( undo )
	{
		if ( !recovery_year )
		{
			printf(
			"<h3>Error: no tax recoveries posted.\n" );
			return;
		}
	}
	else
	{
		if ( recovery_year == now_year )
		{
			printf(
			"<h3>Error: tax recovery posted already.\n" );
			return;
		}

		if ( recovery_year )
			recovery_year++;
		else
			recovery_year = now_year;

		fixed_asset_purchased_list =
			fixed_asset_fetch_tax_list(
				application_name,
				recovery_year,
				"fixed_asset_purchase"
					/* folder_name */ );

		fixed_asset_prior_list =
			fixed_asset_fetch_tax_list(
				application_name,
				recovery_year,
				"fixed_asset_purchase"
					/* folder_name */ );

		if ( !list_length( fixed_asset_purchased_list )
		&&   !list_length( fixed_asset_prior_list ) )
		{
			printf(
			"<h3>Error: no fixed assets to tax recover.\n" );
			return;
		}
	}

	/* Process */
	/* ------- */
	if ( execute )
	{
		if ( undo )
		{
			char sys_string[ 128 ];

			tax_recover_fixed_assets_undo(
				application_name,
				recovery_year,
				"tax_fixed_asset_recovery"
					/* folder_name */ );

			tax_recover_fixed_assets_undo(
				application_name,
				recovery_year,
				"tax_prior_fixed_asset_recovery"
					/* folder_name */ );

			sprintf( sys_string,
		 		"tax_accumulated_depreciation_reset.sh %s",
		 		application_name );

			system( sys_string );

			printf(
		"<h3>Tax Fixed Asset Recovery for %d is now deleted.</h3>\n",
				recovery_year );
		}
		else
		{
			tax_recovery_fixed_asset_list_set(
				fixed_asset_purchased_list,
				recovery_year );

			tax_recovery_fixed_asset_list_set(
				fixed_asset_prior_list,
				recovery_year );

			tax_recover_fixed_assets_insert(
				fixed_asset_purchased_list,
				fixed_asset_prior_list );

			printf( "<h3>Tax Recovery now posted for %d.</h3>\n",
				recovery_year );
		}
	}
	else
	/* ------- */
	/* Display */
	/* ------- */
	{
		if ( undo )
		{
			printf(
			"<h3>Will undo tax recovery posted for %d.</h3>\n",
				recovery_year );
		}
		else
		{
			tax_recovery_fixed_asset_list_set(
				fixed_asset_purchased_list,
				recovery_year );

			tax_recovery_fixed_asset_list_set(
				fixed_asset_prior_list,
				recovery_year );

			tax_recover_fixed_assets_display(
				fixed_asset_purchased_list,
				fixed_asset_prior_list );
		}
	} /* if Display */

} /* tax_recover_fixed_assets() */

void tax_recover_fixed_assets_undo(	char *application_name,
					int recovery_year,
					char *folder_name )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	sprintf( where, "recovery_year = %d", recovery_year );

	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | sql.e",
		 folder_name,
		 where );

	system( sys_string );

} /* tax_recover_fixed_assets_undo() */

void tax_recover_fixed_assets_insert(	LIST *fixed_asset_purchase_list,
					LIST *fixed_asset_prior_list )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *field;

	field =
"asset_name,serial_number,recovery_year,recovery_amount,recovery_percent";

	if ( list_length( fixed_asset_purchase_list ) )
	{
		sprintf( sys_string,
		 	"insert_statement.e table=%s field=%s del='^'	|"
		 	"sql.e						 ",
		 	"tax_fixed_asset_recovery",
		 	field );

		output_pipe = popen( sys_string, "w" );

		tax_recover_fixed_asset_list_insert(
			output_pipe,
			fixed_asset_purchase_list );

		pclose( output_pipe );
	}

	if ( list_length( fixed_asset_prior_list ) )
	{
		sprintf( sys_string,
		 	"insert_statement.e table=%s field=%s del='^'	|"
		 	"sql.e						 ",
		 	"tax_prior_fixed_asset_recovery",
		 	field );

		output_pipe = popen( sys_string, "w" );

		tax_recover_fixed_asset_list_insert(
			output_pipe,
			fixed_asset_prior_list );

		pclose( output_pipe );
	}

} /* tax_recover_fixed_assets_insert() */

void tax_recover_fixed_asset_list_insert(
					FILE *output_pipe,
					LIST *fixed_asset_list )
{
	FIXED_ASSET *fixed_asset;
	TAX_RECOVERY *tax_recovery;

	if ( list_rewind( fixed_asset_list ) )
	{
		do {
			fixed_asset =
				list_get_pointer(
					fixed_asset_list );

			tax_recovery = fixed_asset->tax_recovery;

			if ( !tax_recovery )
			{
				fprintf( stderr,
				"ERROR in %s/%s()/%d: empty tax_recovery.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				pclose( output_pipe );
				exit( 1 );
			}

			fprintf(	output_pipe,
					"%s^%s^%d^%.2lf^%.3lf\n",
					tax_recovery->asset_name,
					tax_recovery->serial_number,
					tax_recovery->recovery_year,
					tax_recovery->recovery_amount,
					tax_recovery->recovery_percent );

		} while( list_next( fixed_asset_list ) );
	}

} /* tax_recover_fixed_asset_list_insert() */

void tax_recover_fixed_assets_display(	LIST *fixed_asset_purchased_list,
					LIST *fixed_asset_prior_list )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *heading;
	char *justify;

	heading =
	"asset_name,serial_number,cost_basis,recovery_amount,recovery_percent";

	justify = "left,left,right,right,right";

	sprintf( sys_string,
		 "html_table.e '' '%s' '^' '%s'",
		 heading,
		 justify );

	output_pipe = popen( sys_string, "w" );

	if ( list_length( fixed_asset_purchased_list ) )
	{
		tax_recover_fixed_asset_list_display(
			output_pipe,
			fixed_asset_purchased_list );
	}

	if ( list_length( fixed_asset_prior_list ) )
	{
		tax_recover_fixed_asset_list_display(
			output_pipe,
			fixed_asset_prior_list );
	}

	pclose( output_pipe );

} /* tax_recover_fixed_assets_display() */

void tax_recover_fixed_asset_list_display(
					FILE *output_pipe,
					LIST *fixed_asset_list )
{
	FIXED_ASSET *fixed_asset;
	TAX_RECOVERY *tax_recovery;

	if ( list_rewind( fixed_asset_list ) )
	{
		do {
			fixed_asset =
				list_get_pointer(
					fixed_asset_list );

			tax_recovery = fixed_asset->tax_recovery;

			if ( !tax_recovery )
			{
				fprintf( stderr,
				"ERROR in %s/%s()/%d: empty tax_recovery.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				pclose( output_pipe );
				exit( 1 );
			}

			fprintf(	output_pipe,
					"%s^%s^%.2lf^%.2lf^%.3lf\n",
					tax_recovery->asset_name,
					tax_recovery->serial_number,
					tax_recovery->tax_cost_basis,
					tax_recovery->recovery_amount,
					tax_recovery->recovery_percent );

		} while( list_next( fixed_asset_list ) );
	}

} /* tax_recover_fixed_assets_display() */

