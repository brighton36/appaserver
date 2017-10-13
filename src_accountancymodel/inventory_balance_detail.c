/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/inventory_balance_detail.c	*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "application_constants.h"
#include "appaserver_parameter_file.h"
#include "appaserver_link_file.h"
#include "inventory.h"
#include "entity.h"
#include "application.h"
#include "ledger.h"
#include "document.h"
#include "purchase.h"
#include "customer.h"

/* Constants */
/* --------- */
#define STDOUT_PROCESS		"delimiter2padded_columns.e '^'"
#define TABLE_PROCESS		"html_table.e '' '%s' '^' '%s'"
#define HEADING			"Arrived/Completed^Operation^FullName^Quantity^CostPer^OnHand^Avg. Cost^Balance"
#define FOLDER_TABLE_HEADING	"OnHand^Avg^Balance"
#define PROMPT			"Press here to view report."

/* Prototypes */
/* ---------- */
LIST *get_PDF_row_list(			LIST *inventory_balance_list );

LIST *get_PDF_heading_list(		void );

void inventory_balance_detail_stdout(	char *application_name,
					INVENTORY *inventory );

void inventory_balance_detail_table(	char *application_name,
					INVENTORY *inventory,
					char *process_name );

void inventory_balance_detail_PDF(	char *application_name,
					INVENTORY *inventory,
					char *process_name,
					char *document_root_directory );

int main( int argc, char **argv )
{
	char *application_name;
	char *inventory_name;
	char *process_name;
	char *output_medium;
	ENTITY_SELF *entity_self;
	char *database_string = {0};
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

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

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
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
	else
	if ( strcmp( output_medium, "PDF" ) == 0 )
	{
		inventory_balance_detail_PDF(
			application_name,
			entity_self->sale_inventory
				/* inventory */,
			process_name,
			appaserver_parameter_file->document_root );
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
		 HEADING,
		 "left,left,left,right"
			/* justify_comma_list */ );

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

void inventory_balance_detail_PDF(	char *application_name,
					INVENTORY *inventory,
					char *process_name,
					char *document_root_directory )
{
	char buffer[ 128 ];
	char caption[ 128 ];
	LATEX *latex;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	char *logo_filename;
	APPASERVER_LINK_FILE *appaserver_link_file;
	LATEX_TABLE *latex_table;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			process_name /* filename_stem */,
			application_name,
			getpid() /* process_id */,
			(char *)0 /* session */,
			(char *)0 /* extension */ );

	appaserver_link_file->extension = "tex";

	latex_filename =
		strdup( appaserver_link_get_tail_half(
				(char *)0 /* application_name */,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension ) );

	appaserver_link_file->extension = "dvi";

	dvi_filename =
		strdup( appaserver_link_get_tail_half(
				(char *)0 /* application_name */,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension ) );

	appaserver_link_file->extension = "pdf";

	ftp_output_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	working_directory =
		appaserver_link_get_source_directory(
			document_root_directory,
			application_name );

	logo_filename =
		application_constants_quick_fetch(
			application_name,
			"logo_filename" /* key */ );

	printf( "<h1>%s</h1>\n",
		 format_initial_capital(
			buffer, process_name ) );

	fflush( stdout );

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			logo_filename );

	sprintf( caption,
		 "%s for %s",
		 format_initial_capital(
			buffer,
			process_name ),
		 inventory->inventory_name );

	latex_table =
		latex_new_latex_table(
			caption );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list = get_PDF_heading_list();

	inventory->inventory_balance_list =
		inventory_get_average_cost_inventory_balance_list(
			inventory->inventory_purchase_list,
			inventory->inventory_sale_list );

	latex_table->row_list =
		get_PDF_row_list(
			inventory->inventory_balance_list );

	latex_longtable_output(
		latex->output_stream,
		latex->landscape_flag,
		latex->table_list,
		latex->logo_filename,
		0 /* not omit_page_numbers */ );

	fclose( latex->output_stream );

	latex_tex2pdf(	latex->tex_filename,
			latex->working_directory );

	appaserver_library_output_ftp_prompt(
		ftp_output_filename, 
		PROMPT,
		process_name /* target */,
		(char *)0 /* mime_type */ );

} /* inventory_balance_detail_PDF() */

LIST *get_PDF_heading_list( void )
{
	LATEX_TABLE_HEADING *table_heading;
	LIST *heading_list;
	LIST *heading_name_list;
	char *heading_name;
	int count = 0;
	boolean right_justify_flag = 0;

	heading_list = list_new();

	heading_name_list = list_string2list( HEADING, '^' );

	if ( !list_rewind( heading_name_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get heading_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		heading_name = list_get_pointer( heading_name_list );

		if ( count++ == 3 ) right_justify_flag = 1;

		table_heading = latex_new_latex_table_heading();
		table_heading->heading = heading_name;
		table_heading->right_justified_flag = right_justify_flag;
		list_append_pointer( heading_list, table_heading );

	} while( list_next( heading_name_list ) );

	return heading_list;

} /* get_PDF_heading_list() */

LIST *get_PDF_row_list( LIST *inventory_balance_list )
{
	LATEX_ROW *latex_row;
	LIST *row_list;
	INVENTORY_BALANCE *inventory_balance;
	char buffer[ 128 ];

	if ( !list_rewind( inventory_balance_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		inventory_balance = list_get( inventory_balance_list );

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		if ( inventory_balance->inventory_purchase )
		{
			latex_append_column_data_list(
				latex_row->column_data_list,
				inventory_balance->
					inventory_purchase->
					arrived_date_time,
				0 /* not large_bold */ );

			latex_append_column_data_list(
				latex_row->column_data_list,
				"Purchase",
				0 /* not large_bold */ );

			latex_append_column_data_list(
				latex_row->column_data_list,
				inventory_balance->
					inventory_purchase->
					full_name,
				0 /* not large_bold */ );

			sprintf(buffer,
				"%d",
				inventory_balance->
					inventory_purchase->
					ordered_quantity );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( buffer ),
				0 /* not large_bold */ );

			sprintf(buffer,
				"%.4lf",
				inventory_balance->
					inventory_purchase->
					capitalized_unit_cost );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( buffer ),
				0 /* not large_bold */ );
		}
		else
		if ( inventory_balance->inventory_sale )
		{
			latex_append_column_data_list(
				latex_row->column_data_list,
				inventory_balance->
					inventory_sale->
					completed_date_time,
				0 /* not large_bold */ );

			latex_append_column_data_list(
				latex_row->column_data_list,
				"Sale",
				0 /* not large_bold */ );

			latex_append_column_data_list(
				latex_row->column_data_list,
				inventory_balance->
					inventory_sale->
					full_name,
				0 /* not large_bold */ );

			sprintf(buffer,
				"%d",
				inventory_balance->
					inventory_sale->
					quantity );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( buffer ),
				0 /* not large_bold */ );

			sprintf(buffer,
				"%.4lf",
				inventory_balance->
					inventory_sale->
					cost_of_goods_sold );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( buffer ),
				0 /* not large_bold */ );
		}
		else
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: both inventory_purchase and inventory_sale are missing.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
			exit( 1 );
		}

		sprintf(buffer,
			"%d",
			inventory_balance->quantity_on_hand );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( buffer ),
			0 /* not large_bold */ );

		sprintf(buffer,
			"%.4lf",
			inventory_balance->average_unit_cost );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( buffer ),
			0 /* not large_bold */ );

		sprintf(buffer,
			"%.2lf",
			inventory_balance->total_cost_balance );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( buffer ),
			0 /* not large_bold */ );

	} while ( list_next( inventory_balance_list ) );

	return row_list;

} /* get_PDF_row_list() */
