/* ---------------------------------------------------------------	*/
/* src_accountancymodel/program_profit.c				*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "environ.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "application_constants.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "date.h"
#include "ledger.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define PROMPT				"Press here to view statement."

/* Prototypes */
/* ---------- */
double program_profit_aggregate_account_PDF_element(
					LIST *row_list,
					char *program_name,
					char *begin_date,
					char *end_date,
					char *element );

double program_profit_PDF_element(
					LIST *row_list,
					char *program_name,
					char *begin_date,
					char *end_date,
					char *element );

void program_journal_aggregate_account_parse(
					char *account_name,
					char *amount_string,
					char *record );

void program_journal_parse(
					char *entity_name,
					char *transaction_date_time,
					char *account_name,
					char *amount_string,
					char *record );

double program_profit_html_table_element(
					HTML_TABLE *html_table,
					char *program_name,
					char *begin_date,
					char *end_date,
					char *element );

double program_profit_aggregate_account_html_table_element(
					HTML_TABLE *html_table,
					char *program_name,
					char *begin_date,
					char *end_date,
					char *element );

LIST *program_journal_ledger_aggregate_account_fetch_record_list(
					char *program_name,
					char *element,
					char *begin_date,
					char *end_date );

LIST *program_journal_ledger_fetch_record_list(
					char *program_name,
					char *element,
					char *begin_date,
					char *end_date );

void program_profit_PDF(		char *application_name,
					char *title,
					char *sub_title,
					char *program_name,
					char *begin_date,
					char *end_date,
					char *document_root_directory,
					char *process_name );

void program_profit_aggregate_account_PDF(
					char *application_name,
					char *title,
					char *sub_title,
					char *program_name,
					char *begin_date,
					char *end_date,
					char *document_root_directory,
					char *process_name );

void program_profit_html_table(
					char *title,
					char *sub_title,
					char *program_name,
					char *begin_date,
					char *end_date );

void program_profit_aggregate_account_html_table(
					char *title,
					char *sub_title,
					char *program_name,
					char *begin_date,
					char *end_date );

int main( int argc, char **argv )
{
	char *application_name;
	char *program_name;
	char *process_name;
	char *begin_date;
	char *end_date;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 256 ];
	char sub_title[ 256 ];
	char *database_string = {0};
	boolean aggregate_account;
	char *output_medium;

	appaserver_error_stderr( argc, argv );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s application process program begin_date end_date aggregate_account_yn output_medium\n",
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
	program_name = argv[ 3 ];
	begin_date = argv[ 4 ];
	end_date = argv[ 5 ];
	aggregate_account = ( *argv[ 6 ] == 'y' );
	output_medium = argv[ 7 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !*end_date || strcmp( end_date, "end_date" ) == 0 )
	{
		end_date = date_get_now_yyyy_mm_dd();
	}

	sprintf( title, "%s for %s", process_name, program_name );
	format_initial_capital( title, title );

	sprintf( sub_title, "As of %s", end_date );

	document = document_new( title, application_name );
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

	if ( !*program_name || strcmp( program_name, "program" ) == 0 )
	{
		printf( "<h3>Please select a program.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		if ( aggregate_account )
		{
			program_profit_aggregate_account_html_table(
				title,
				sub_title,
				program_name,
				begin_date,
				end_date );
		}
		else
		{
			program_profit_html_table(
				title,
				sub_title,
				program_name,
				begin_date,
				end_date );
		}
	}
	else
	{
		if ( aggregate_account )
		{
			program_profit_aggregate_account_PDF(
				application_name,
				title,
				sub_title,
				program_name,
				begin_date,
				end_date,
				appaserver_parameter_file->
					document_root,
				process_name );
		}
		else
		{
			program_profit_PDF(
				application_name,
				title,
				sub_title,
				program_name,
				begin_date,
				end_date,
				appaserver_parameter_file->
					document_root,
				process_name );
		}
	}

	document_close();

	return 0;

} /* main() */

void program_profit_aggregate_account_html_table(
				char *title,
				char *sub_title,
				char *program_name,
				char *begin_date,
				char *end_date )
{
	HTML_TABLE *html_table;
	double total_revenue;
	double total_expense;
	double profit;
	LIST *heading_list;
	char buffer[ 128 ];

	heading_list = list_new();
	list_append_pointer( heading_list, "Account" );
	list_append_pointer( heading_list, "Amount" );

	html_table = new_html_table( title, sub_title );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 1;
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	html_table_output_data_heading(
			heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	/* Revenue */
	/* ------- */
	total_revenue =
		program_profit_aggregate_account_html_table_element(
			html_table,
			program_name,
			begin_date,
			end_date,
			"revenue" );

	html_table_set_data(
		html_table->data_list,
		"<big>Total Revenue</big>" );

	sprintf( buffer,
		 "<big>%s</big>",
		 place_commas_in_money( total_revenue ) );

	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	html_table->data_list = list_new();

	/* Prepaid */
	/* ------- */
	total_expense =
		program_profit_aggregate_account_html_table_element(
			html_table,
			program_name,
			begin_date,
			end_date,
			"prepaid" );

	total_expense +=
		program_profit_aggregate_account_html_table_element(
			html_table,
			program_name,
			begin_date,
			end_date,
			"expense" );

	html_table_set_data(
		html_table->data_list,
		"<big>Total Expense</big>" );

	sprintf( buffer,
		 "<big>%s</big>",
		 place_commas_in_money( total_expense ) );

	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	html_table->data_list = list_new();

	/* Profit */
	/* ------ */
	profit = total_revenue - total_expense;

	html_table_set_data(
		html_table->data_list,
		"<big>Profit</big>" );

	sprintf( buffer,
		 "<big>%s</big>",
		 place_commas_in_money( profit ) );

	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	html_table_close();

} /* program_profit_aggregate_account_html_table() */

void program_profit_html_table(	char *title,
				char *sub_title,
				char *program_name,
				char *begin_date,
				char *end_date )
{
	HTML_TABLE *html_table;
	double total_revenue;
	double total_expense;
	double profit;
	LIST *heading_list;
	char buffer[ 128 ];

	heading_list = list_new();
	list_append_pointer( heading_list, "Account" );
	list_append_pointer( heading_list, "Entity" );
	list_append_pointer( heading_list, "Date/Time" );
	list_append_pointer( heading_list, "Amount" );

	html_table = new_html_table( title, sub_title );

	html_table->number_left_justified_columns = 3;
	html_table->number_right_justified_columns = 1;
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	html_table_output_data_heading(
			heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	/* Revenue */
	/* ------- */
	total_revenue =
		program_profit_html_table_element(
			html_table,
			program_name,
			begin_date,
			end_date,
			"revenue" );

	html_table_set_data(
		html_table->data_list,
		"<big>Total Revenue</big>" );
	html_table_set_data( html_table->data_list, "" );
	html_table_set_data( html_table->data_list, "" );

	sprintf( buffer,
		 "<big>%s</big>",
		 place_commas_in_money( total_revenue ) );

	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	html_table->data_list = list_new();

	/* Prepaid */
	/* ------- */
	total_expense =
		program_profit_html_table_element(
			html_table,
			program_name,
			begin_date,
			end_date,
			"prepaid" );

	total_expense +=
		program_profit_html_table_element(
			html_table,
			program_name,
			begin_date,
			end_date,
			"expense" );

	html_table_set_data(
		html_table->data_list,
		"<big>Total Expense</big>" );
	html_table_set_data( html_table->data_list, "" );
	html_table_set_data( html_table->data_list, "" );

	sprintf( buffer,
		 "<big>%s</big>",
		 place_commas_in_money( total_expense ) );

	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	html_table->data_list = list_new();

	/* Profit */
	/* ------ */
	profit = total_revenue - total_expense;

	html_table_set_data(
		html_table->data_list,
		"<big>Profit</big>" );
	html_table_set_data( html_table->data_list, "" );
	html_table_set_data( html_table->data_list, "" );

	sprintf( buffer,
		 "<big>%s</big>",
		 place_commas_in_money( profit ) );

	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	html_table_close();

} /* program_profit_html_table() */

double program_profit_aggregate_account_html_table_element(
				HTML_TABLE *html_table,
				char *program_name,
				char *begin_date,
				char *end_date,
				char *element )
{
	double total_element;
	LIST *program_journal_ledger_record_list;
	char *record;
	char account_name[ 128 ];
	char buffer[ 128 ];
	char amount_string[ 16 ];
	double amount_double;

	program_journal_ledger_record_list =
		program_journal_ledger_aggregate_account_fetch_record_list(
			program_name,
			element,
			begin_date,
			end_date );

	if ( !list_rewind( program_journal_ledger_record_list ) )
		return 0.0;

	total_element = 0.0;

	do {
		record = list_get_pointer( program_journal_ledger_record_list );

		program_journal_aggregate_account_parse(
			account_name,
			amount_string,
			record );

		html_table_set_data(
			html_table->data_list,
			strdup( format_initial_capital(
					buffer,
					account_name ) ) );

		amount_double = atof( amount_string );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money( amount_double ) ) );

		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		html_table->data_list = list_new();

		total_element += amount_double;

	} while( list_next( program_journal_ledger_record_list ) );

	return total_element;

} /* program_profit_aggregate_account_html_table_element() */

double program_profit_html_table_element(
				HTML_TABLE *html_table,
				char *program_name,
				char *begin_date,
				char *end_date,
				char *element )
{
	double total_element;
	LIST *program_journal_ledger_record_list;
	char *record;
	char entity_name[ 128 ];
	char transaction_date_time[ 128 ];
	char account_name[ 128 ];
	char buffer[ 128 ];
	char amount_string[ 16 ];
	double amount_double;

	program_journal_ledger_record_list =
		program_journal_ledger_fetch_record_list(
			program_name,
			element,
			begin_date,
			end_date );

	if ( !list_rewind( program_journal_ledger_record_list ) )
		return 0.0;

	total_element = 0.0;

	do {
		record = list_get_pointer( program_journal_ledger_record_list );

		program_journal_parse(
			entity_name,
			transaction_date_time,
			account_name,
			amount_string,
			record );

		html_table_set_data(
			html_table->data_list,
			strdup( format_initial_capital(
					buffer,
					account_name ) ) );

		html_table_set_data(
			html_table->data_list,
			strdup( entity_name ) );

		html_table_set_data(
			html_table->data_list,
			strdup( transaction_date_time ) );

		amount_double = atof( amount_string );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money( amount_double ) ) );

		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		html_table->data_list = list_new();

		total_element += amount_double;

	} while( list_next( program_journal_ledger_record_list ) );

	return total_element;

} /* program_profit_html_table_element() */

void program_profit_aggregate_account_PDF(
				char *application_name,
				char *title,
				char *sub_title,
				char *program_name,
				char *begin_date,
				char *end_date,
				char *document_root_directory,
				char *process_name )
{
	LATEX *latex;
	LATEX_TABLE *latex_table;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	pid_t pid = getpid();
	LATEX_TABLE_HEADING *table_heading;
	LATEX_ROW *latex_row;
	char caption[ 256 ];
	double total_revenue;
	double total_expense;
	double profit;
	char buffer[ 128 ];

	APPASERVER_LINK_FILE *appaserver_link_file;

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			process_name /* filename_stem */,
			application_name,
			pid /* process_id */,
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

	working_directory =
		appaserver_link_get_source_directory(
			document_root_directory,
			application_name );

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			application_constants_quick_fetch(
				application_name,
				"logo_filename" /* key */ ) );

	sprintf( caption, "%s %s", title, sub_title );

	latex_table =
		latex_new_latex_table(
			caption );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Account";
	table_heading->right_justified_flag = 0;
	list_append_pointer( latex_table->heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Amount";
	table_heading->right_justified_flag = 1;
	list_append_pointer( latex_table->heading_list, table_heading );

	latex_table->row_list = list_new();

	/* Revenue */
	/* ------- */
	total_revenue =
		program_profit_aggregate_account_PDF_element(
				latex_table->row_list,
				program_name,
				begin_date,
				end_date,
				"revenue" );

	latex_row = latex_new_latex_row();
	list_append_pointer( latex_table->row_list, latex_row );

	list_append_pointer(
		latex_row->column_data_list,
		"\\bf{Total Revenue}" );

	sprintf( buffer,
		 "\\bf{%s}",
		 place_commas_in_money( total_revenue ) );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( buffer ) );

	/* Expense */
	/* ------- */
	total_expense =
		program_profit_aggregate_account_PDF_element(
				latex_table->row_list,
				program_name,
				begin_date,
				end_date,
				"prepaid" );

	total_expense +=
		program_profit_aggregate_account_PDF_element(
				latex_table->row_list,
				program_name,
				begin_date,
				end_date,
				"expense" );

	latex_row = latex_new_latex_row();
	list_append_pointer( latex_table->row_list, latex_row );

	list_append_pointer(
		latex_row->column_data_list,
		"\\bf{Total Expense}" );

	sprintf( buffer,
		 "\\bf{%s}",
		 place_commas_in_money( total_expense ) );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( buffer ) );

	/* Profit */
	/* ------ */
	profit = total_revenue - total_expense;

	latex_row = latex_new_latex_row();
	list_append_pointer( latex_table->row_list, latex_row );

	list_append_pointer(
		latex_row->column_data_list,
		"\\bf{Profit}" );

	sprintf( buffer,
		 "\\bf{%s}",
		 place_commas_in_money( profit ) );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( buffer ) );

	/* Output */
	/* ------ */
	latex_longtable_output(
		latex->output_stream,
		latex->landscape_flag,
		latex->table_list,
		latex->logo_filename,
		0 /* not omit_page_numbers */ );

	fclose( latex->output_stream );

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

	latex_tex2pdf(	latex->tex_filename,
			latex->working_directory );

	appaserver_library_output_ftp_prompt(
		ftp_output_filename, 
		PROMPT,
		process_name /* target */,
		(char *)0 /* mime_type */ );

} /* program_profit_aggregate_account_PDF() */

void program_profit_PDF(	char *application_name,
				char *title,
				char *sub_title,
				char *program_name,
				char *begin_date,
				char *end_date,
				char *document_root_directory,
				char *process_name )
{
	LATEX *latex;
	LATEX_TABLE *latex_table;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	pid_t pid = getpid();
	LATEX_TABLE_HEADING *table_heading;
	LATEX_ROW *latex_row;
	char caption[ 256 ];
	double total_revenue;
	double total_expense;
	double profit;
	char buffer[ 128 ];

	APPASERVER_LINK_FILE *appaserver_link_file;

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			process_name /* filename_stem */,
			application_name,
			pid /* process_id */,
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

	working_directory =
		appaserver_link_get_source_directory(
			document_root_directory,
			application_name );

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			application_constants_quick_fetch(
				application_name,
				"logo_filename" /* key */ ) );

	sprintf( caption, "%s %s", title, sub_title );

	latex_table =
		latex_new_latex_table(
			caption );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Account";
	table_heading->paragraph_size = "5.5cm";
	list_append_pointer( latex_table->heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Entity";
	table_heading->paragraph_size = "5.5cm";
	list_append_pointer( latex_table->heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Date/Time";
	table_heading->right_justified_flag = 0;
	list_append_pointer( latex_table->heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Amount";
	table_heading->right_justified_flag = 1;
	list_append_pointer( latex_table->heading_list, table_heading );

	latex_table->row_list = list_new();

	/* Revenue */
	/* ------- */
	total_revenue =
		program_profit_PDF_element(
				latex_table->row_list,
				program_name,
				begin_date,
				end_date,
				"revenue" );

	latex_row = latex_new_latex_row();
	list_append_pointer( latex_table->row_list, latex_row );

	list_append_pointer(
		latex_row->column_data_list,
		"\\bf{Total Revenue}" );

	list_append_pointer( latex_row->column_data_list, "" );
	list_append_pointer( latex_row->column_data_list, "" );

	sprintf( buffer,
		 "\\bf{%s}",
		 place_commas_in_money( total_revenue ) );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( buffer ) );

	/* Expense */
	/* ------- */
	total_expense =
		program_profit_PDF_element(
				latex_table->row_list,
				program_name,
				begin_date,
				end_date,
				"prepaid" );

	total_expense +=
		program_profit_PDF_element(
				latex_table->row_list,
				program_name,
				begin_date,
				end_date,
				"expense" );

	latex_row = latex_new_latex_row();
	list_append_pointer( latex_table->row_list, latex_row );

	list_append_pointer(
		latex_row->column_data_list,
		"\\bf{Total Expense}" );

	list_append_pointer( latex_row->column_data_list, "" );
	list_append_pointer( latex_row->column_data_list, "" );

	sprintf( buffer,
		 "\\bf{%s}",
		 place_commas_in_money( total_expense ) );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( buffer ) );

	/* Profit */
	/* ------ */
	profit = total_revenue - total_expense;

	latex_row = latex_new_latex_row();
	list_append_pointer( latex_table->row_list, latex_row );

	list_append_pointer(
		latex_row->column_data_list,
		"\\bf{Profit}" );

	list_append_pointer( latex_row->column_data_list, "" );
	list_append_pointer( latex_row->column_data_list, "" );

	sprintf( buffer,
		 "\\bf{%s}",
		 place_commas_in_money( profit ) );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( buffer ) );

	/* Output */
	/* ------ */
	latex_longtable_output(
		latex->output_stream,
		latex->landscape_flag,
		latex->table_list,
		latex->logo_filename,
		0 /* not omit_page_numbers */ );

	fclose( latex->output_stream );

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

	latex_tex2pdf(	latex->tex_filename,
			latex->working_directory );

	appaserver_library_output_ftp_prompt(
		ftp_output_filename, 
		PROMPT,
		process_name /* target */,
		(char *)0 /* mime_type */ );

} /* program_profit_PDF() */

double program_profit_aggregate_account_PDF_element(
				LIST *row_list,
				char *program_name,
				char *begin_date,
				char *end_date,
				char *element )
{
	double total_element;
	LIST *program_journal_ledger_record_list;
	char *record;
	char account_name[ 128 ];
	char buffer[ 128 ];
	char amount_string[ 16 ];
	double amount_double;
	LATEX_ROW *latex_row;

	program_journal_ledger_record_list =
		program_journal_ledger_aggregate_account_fetch_record_list(
			program_name,
			element,
			begin_date,
			end_date );

	if ( !list_rewind( program_journal_ledger_record_list ) )
		return 0.0;

	total_element = 0.0;

	do {
		record = list_get_pointer( program_journal_ledger_record_list );

		program_journal_aggregate_account_parse(
			account_name,
			amount_string,
			record );

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					buffer,
					account_name ) ) );

		amount_double = atof( amount_string );
		list_append_pointer(
			latex_row->column_data_list,
			strdup( place_commas_in_money( amount_double ) ) );

		total_element += amount_double;

	} while( list_next( program_journal_ledger_record_list ) );

	return total_element;

} /* program_profit_aggregate_account_PDF_element() */

double program_profit_PDF_element(
				LIST *row_list,
				char *program_name,
				char *begin_date,
				char *end_date,
				char *element )
{
	double total_element;
	LIST *program_journal_ledger_record_list;
	char *record;
	char entity_name[ 128 ];
	char transaction_date_time[ 128 ];
	char account_name[ 128 ];
	char buffer[ 128 ];
	char amount_string[ 16 ];
	double amount_double;
	LATEX_ROW *latex_row;

	program_journal_ledger_record_list =
		program_journal_ledger_fetch_record_list(
			program_name,
			element,
			begin_date,
			end_date );

	if ( !list_rewind( program_journal_ledger_record_list ) )
		return 0.0;

	total_element = 0.0;

	do {
		record = list_get_pointer( program_journal_ledger_record_list );

		program_journal_parse(
			entity_name,
			transaction_date_time,
			account_name,
			amount_string,
			record );

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					buffer,
					account_name ) ) );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( entity_name ) );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( transaction_date_time ) );

		amount_double = atof( amount_string );
		list_append_pointer(
			latex_row->column_data_list,
			strdup( place_commas_in_money( amount_double ) ) );

		total_element += amount_double;

	} while( list_next( program_journal_ledger_record_list ) );

	return total_element;

} /* program_profit_PDF_element() */

LIST *program_journal_ledger_aggregate_account_fetch_record_list(
			char *program_name,
			char *element,
			char *begin_date,
			char *end_date )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "program_journal_ledger_aggregate_account.sh '%s' '%s' %s %s",
		 program_name,
		 element,
		 begin_date,
		 end_date );

	return pipe2list( sys_string );

} /* program_journal_ledger_aggregate_account_fetch_record_list() */

LIST *program_journal_ledger_fetch_record_list(
			char *program_name,
			char *element,
			char *begin_date,
			char *end_date )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "program_journal_ledger.sh '%s' '%s' %s %s",
		 program_name,
		 element,
		 begin_date,
		 end_date );

	return pipe2list( sys_string );

} /* program_journal_ledger_fetch_record_list() */

void program_journal_aggregate_account_parse(
			char *account_name,
			char *amount_string,
			char *record )
{
	piece( account_name, FOLDER_DATA_DELIMITER, record, 0 );
	piece( amount_string, FOLDER_DATA_DELIMITER, record, 1 );

} /* program_journal_aggregate_account_parse() */

void program_journal_parse(
			char *entity_name,
			char *transaction_date_time,
			char *account_name,
			char *amount_string,
			char *record )
{
	piece( entity_name, FOLDER_DATA_DELIMITER, record, 0 );
	piece( transaction_date_time, FOLDER_DATA_DELIMITER, record, 1 );
	piece( account_name, FOLDER_DATA_DELIMITER, record, 2 );
	piece( amount_string, FOLDER_DATA_DELIMITER, record, 3 );

} /* program_journal_parse() */

