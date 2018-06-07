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
#include "tax_recovery.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void tax_recover_fixed_assets_undo(	char *application_name,
					int recovery_year );

boolean tax_recover_fixed_assets_execute(
					char *application_name,
					char *fund_name,
					int recovery_year );

boolean tax_recover_fixed_assets_display(
					char *application_name,
					char *fund_name,
					int recovery_year );

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
		tax_recovery_fetch_max_recovery_year(
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
		if ( recovery_year == now_year )
		{
			printf(
			"<h3>Error: tax recovery posted already.\n" );
			return 0;
		}

		if ( recovery_year )
			recovery_year++;
		else
			recovery_year = now_year;
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
					recovery_year ) )
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

boolean tax_recover_fixed_assets_execute(char *application_name,
					char *fund_name,
					int recovery_year )
{
	return 1;

} /* tax_recover_fixed_assets_execute() */

boolean tax_recover_fixed_assets_display(
					char *application_name,
					char *fund_name,
					int recovery_year )
{
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 512 ];
	TAX_RECOVERY *tax_recovery;
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

	input_pipe =
		tax_recovery_get_input_pipe(
			application_name,
			fund_name,
			recovery_year );

	while( get_line( input_buffer, input_pipe ) )
	{
		tax_recovery =
			tax_recovery_input_buffer_parse(
				input_buffer );

		/* Need to write the table lookup function. */
		/* ---------------------------------------- */
		if ( tax_recovery->tax_recovery_period_years < 27.5 ) continue;

		tax_recovery->recovery_amount =
			tax_recovery_calculate_recovery_amount(
				&tax_recovery->recovery_percent,
				tax_recovery->tax_cost_basis,
				tax_recovery->tax_service_placement_date,
				tax_recovery->disposal_date
					/* sale_date_string */,
				tax_recovery->tax_recovery_period_years,
				tax_recovery->recovery_year
					/* current_year */ );

		fprintf(	output_pipe,
				"%s^%s^%.2lf^%.2lf^%.3lf\n",
				tax_recovery->asset_name,
				tax_recovery->serial_number,
				tax_recovery->tax_cost_basis,
				tax_recovery->recovery_amount,
				tax_recovery->recovery_percent );
	}

	pclose( input_pipe );
	pclose( output_pipe );

	return 1;

} /* tax_recover_fixed_assets_display() */

