/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/inventory_balance_detail.c	*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "inventory.h"
#include "entity.h"
#include "ledger.h"
#include "document.h"
#include "purchase.h"
#include "customer.h"

/* Constants */
/* --------- */
#define STDOUT_PROCESS		"delimiter2padded_columns.e '^'"
#define TABLE_PROCESS		"html_table.e '' '%s' '^'"
#define HEADING			"Arrived/Completed^Operation^FullName^Quantity^CostPer^OnHand^Avg. Cost^Balance"
#define FOLDER_TABLE_HEADING	"OnHand^Avg^Balance"

/* Prototypes */
/* ---------- */
void inventory_balance_detail_stdout(	char *application_name,
					INVENTORY *inventory );

void inventory_balance_detail_table(	char *application_name,
					INVENTORY *inventory,
					char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *inventory_name;
	char *process_name;
	char *output_medium;
	ENTITY_SELF *entity_self;
	char *database_string = {0};
	DOCUMENT *document;

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( argc != 5 )
	{
		fprintf( stderr,
		 "Usage: %s application process inventory_name output_medium\n",
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
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	process_name = argv[ 2 ];
	inventory_name = argv[ 3 ];
	output_medium = argv[ 4 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

	entity_self =
		entity_self_sale_inventory_load(
			application_name,
			inventory_name );

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

		appaserver_parameter_file = appaserver_parameter_file_new();

		document = document_new((char *)0 /* title */,
					application_name );

		document->output_content_type = 1;

		document_output_heading(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

		document_output_body(
				document->application_name,
				document->onload_control_string );
	}

	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		inventory_balance_detail_stdout(
			application_name,
			entity_self->sale_inventory
				/* inventory */ );
	}
	else
	if ( strcmp( output_medium, "table" ) == 0 )
	{
		inventory_balance_detail_table(
			application_name,
			entity_self->sale_inventory
				/* inventory */,
			process_name );
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
		document_close();

	return 0;

} /* main() */

void inventory_balance_detail_stdout(	char *application_name,
					INVENTORY *inventory )
{
	FILE *output_pipe;

	output_pipe = popen( STDOUT_PROCESS, "w" );

	inventory->inventory_balance_list =
		inventory_get_average_cost_inventory_balance_list(
			inventory->inventory_purchase_list,
			inventory->inventory_sale_list );

	printf( "\nInventory Average Balance List:\n" );

	fprintf( output_pipe,
		 "%s\n",
		 HEADING );

	inventory_balance_list_table_display(
		output_pipe,
		inventory->inventory_balance_list );

	pclose( output_pipe );

	output_pipe = popen( STDOUT_PROCESS, "w" );

	printf( "\nINVENTORY:\n" );
	
	inventory_folder_table_display(
		output_pipe,
		application_name,
		inventory->inventory_name,
		FOLDER_TABLE_HEADING );

	pclose( output_pipe );

} /* inventory_balance_detail_stdout() */

void inventory_balance_detail_table(	char *application_name,
					INVENTORY *inventory,
					char *process_name )
{
	FILE *output_pipe;
	char buffer[ 128 ];
	char output_process[ 256 ];

	printf( "<h1>%s</h1>\n",
		 format_initial_capital(
			buffer, process_name ) );

	fflush( stdout );

	sprintf( output_process,
		 TABLE_PROCESS,
		 HEADING );

	output_pipe = popen( output_process, "w" );

	inventory->inventory_balance_list =
		inventory_get_average_cost_inventory_balance_list(
			inventory->inventory_purchase_list,
			inventory->inventory_sale_list );

	inventory_balance_list_table_display(
		output_pipe,
		inventory->inventory_balance_list );

	pclose( output_pipe );

	sprintf( output_process,
		 "html_table.e 'INVENTORY' '%s' '^'",
		 FOLDER_TABLE_HEADING );

	output_pipe = popen( output_process, "w" );

	inventory_folder_table_display(
		output_pipe,
		application_name,
		inventory->inventory_name,
		(char *)0 /* heading */ );

	pclose( output_pipe );

} /* inventory_balance_detail_table() */

