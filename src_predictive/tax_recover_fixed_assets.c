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
#include "tax.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean tax_recover_recovery_year_exists(
					char *application_name,
					char *fund_name,
					int recovery_year );

int tax_recover_fetch_max_recovery_year(
					int *now_year,
					char *application_name,
					char *fund_name );

void tax_recover_fixed_assets_undo(	char *application_name,
					int recovery_year );

boolean tax_recover_fixed_assets_execute(
					char *application_name,
					char *fund_name,
					int recovery_year );

boolean tax_recover_fixed_assets_display(
					char *application_name,
					char *fund_name,
					int recovery_year,
					char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *fund_name;
	int recovery_year;
	boolean execute;
	char buffer[ 128 ];
	boolean undo;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;
	int now_year = 0;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 6 )
	{
		fprintf(stderr,
	"Usage: %s ignored process fund undo_yn execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 2 ];
	fund_name = argv[ 3 ];
	undo = (*argv[ 4 ]) == 'y';
	execute = (*argv[ 5 ]) == 'y';

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

	/* Input */
	/* ----- */
	recovery_year =
		tax_recover_fetch_max_recovery_year(
			&now_year,
			application_name,
			fund_name );

	if ( undo )
	{
		if ( !recovery_year )
		{
			printf(
			"<h3>Error: no tax recoveries posted.\n" );
			return 0;
		}
	}
	else
	{
		if ( recovery_year )
			recovery_year++;
		else
			recovery_year = now_year;

		if ( tax_recover_recovery_year_exists(
			application_name,
			fund_name,
			now_year ) )
		{
			printf(
		"<h3>Error: recovery exists for this year.</h3>\n" );
			return 0;
		}
	}

	/* Process */
	/* ------- */
	if ( execute )
	{
		if ( undo )
		{
			tax_recover_fixed_assets_undo(
				application_name,
				recovery_year );

			printf(
		"<h3>Tax Fixed Asset Recovery for %d is now deleted.</h3>\n",
				recovery_year );
		}
		else
		{
			if ( !tax_recover_fixed_assets_execute(
				application_name,
				fund_name,
				recovery_year ) )
			{
				printf(
		"<h3>Error: no fixed asset purchases to depreciate.</h3>\n" );
			}
			else
			{
				printf(
			"<h3>Tax Recovery now posted for %d.</h3>\n",
					recovery_year );
			}
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
			if ( !tax_recover_fixed_assets_display(
					application_name,
					fund_name,
					recovery_year,
					process_name ) )
			{
				printf(
		"<h3>Error: no fixed asset purchases to depreciate.</h3>\n" );
			}
		}
	} /* if Display */

	document_close();

	return 0;

} /* main() */

void tax_recover_fixed_assets_undo(	char *application_name,
					int recovery_year )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *folder_name;

	folder_name = "tax_fixed_asset_recovery";

	sprintf( where, "recovery_year = %d", recovery_year );

	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | sql.e",
		 folder_name,
		 where );

	system( sys_string );

	sprintf( sys_string,
		 "tax_accumulated_depreciation_reset.sh %s",
		 application_name );

	system( sys_string );

} /* tax_recover_fixed_assets_undo() */

int tax_recover_fetch_max_recovery_year(
			int *now_year,
			char *application_name,
			char *fund_name )
{
	char sys_string[ 1024 ];
	char *fund_where;
	char where[ 512 ];
	char *select;
	char *folder;
	int max_recovery_year;

	*now_year = atoi( date_get_now_yyyy_mm_dd( date_get_utc_offset() ) );

	select = "max(recovery_year)";

	folder = "purchase_order,tax_fixed_asset_recovery";

	fund_where =
		ledger_get_fund_where(
			application_name,
			fund_name );

	sprintf( where,
"tax_fixed_asset_recovery.full_name = purchase_order.full_name and tax_fixed_asset_recovery.street_address = purchase_order.street_address and tax_fixed_asset_recovery.purchase_date_time = purchase_order.purchase_date_time and %s",
		 fund_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			",
		 application_name,
		 select,
		 folder );

	max_recovery_year = atoi( pipe2string( sys_string ) );

	if ( max_recovery_year >= *now_year )
		return 0;
	else
		return max_recovery_year;

} /* tax_recover_fetch_max_recovery_year() */

boolean tax_recover_recovery_year_exists(
			char *application_name,
			char *fund_name,
			int recovery_year )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *fund_where;
	char *folder;
	char *results;

	folder = "purchase_order,tax_fixed_asset_recovery";

	fund_where =
		ledger_get_fund_where(
			application_name,
			fund_name );

	sprintf( where,
"tax_fixed_asset_recovery.full_name = purchase_order.full_name and tax_fixed_asset_recovery.street_address = purchase_order.street_address and tax_fixed_asset_recovery.purchase_date_time = purchase_order.purchase_date_time and %s and recovery_year = %d",
		 fund_where,
		 recovery_year );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=count			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( results )
		return atoi( results );
	else
		return 0;

} /* tax_recover_recovery_year_exists() */

boolean tax_recover_fixed_assets_execute(char *application_name,
					char *fund_name,
					int recovery_year )
{
	return 1;

} /* tax_recover_fixed_assets_execute() */

boolean tax_recover_fixed_assets_display(
					char *application_name,
					char *fund_name,
					int recovery_year,
					char *process_name )
{
	return 1;

} /* tax_recover_fixed_assets_display() */

