/* ----------------------------------------------------------------	*/
/* src_accountancymodel/tax_form_report.c				*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
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
#include "ledger.h"
#include "application_constants.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "latex.h"
#include "date.h"
#include "date_convert.h"
#include "boolean.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define ROWS_BETWEEN_HEADING		10
#define PROMPT				"Press here to view statement."

/* Prototypes */
/* ---------- */
LIST *build_detail_PDF_row_list(
					TAX_FORM_CATEGORY *tax_form_category );

LIST *tax_form_report_detail_PDF_table_list(
					LIST *tax_form_category_list );

LATEX_TABLE *tax_form_report_PDF_table(
					char *sub_title,
					LIST *tax_form_category_list );

void tax_form_report_html_table(	char *title,
					char *sub_title,
					LIST *tax_form_category_list );

void tax_form_detail_report_html_table(	LIST *tax_form_category_list );

LIST *build_PDF_row_list(		LIST *tax_form_category_list );

LIST *build_detail_PDF_heading_list(	void );

LIST *build_PDF_heading_list(		void );

void tax_form_report_PDF(		char *application_name,
					char *title,
					char *sub_title,
					char *document_root_directory,
					char *process_name,
					LIST *tax_form_category_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 256 ];
	char sub_title[ 256 ];
	char *as_of_date;
	char *database_string = {0};
	char *output_medium;
	LIST *tax_form_category_list;

	if ( argc != 5 )
	{
		fprintf( stderr,
		"Usage: %s application process as_of_date output_medium\n",
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

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	process_name = argv[ 2 ];
	as_of_date = argv[ 3 ];
	output_medium = argv[ 4 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( !*as_of_date
	||   strcmp(	as_of_date,
			"as_of_date" ) == 0 )
	{
		as_of_date = date_get_now_yyyy_mm_dd();
	}

	ledger_get_report_title_sub_title(
		title,
		sub_title,
		process_name,
		application_name,
		(char *)0 /* fund_name */,
		as_of_date,
		0 /* length_fund_name_list */ );

	document = document_new( title, application_name );
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

	document_output_body(	document->application_name,
				document->onload_control_string );

	tax_form_category_list =
		ledger_tax_form_fetch_category_list(
			application_name,
			as_of_date );

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		tax_form_report_html_table(
			title,
			sub_title,
			tax_form_category_list );

		tax_form_detail_report_html_table(
			tax_form_category_list );
	}
	else
	{
		tax_form_report_PDF(
			application_name,
			title,
			sub_title,
			appaserver_parameter_file->document_root,
			process_name,
			tax_form_category_list );
	}

	document_close();
	exit( 0 );

} /* main() */

void tax_form_detail_report_html_table(
			LIST *tax_form_category_list )
{
	HTML_TABLE *html_table;
	LIST *heading_list;
	TAX_FORM_CATEGORY *tax_form_category;
	char buffer[ 128 ];
	char sub_title[ 128 ];
	int count;
	ACCOUNT *account;

	heading_list = list_new();
	list_append_string( heading_list, "account" );
	list_append_string( heading_list, "balance" );

	if ( !list_rewind( tax_form_category_list ) ) return;

	do {
		tax_form_category = list_get( tax_form_category_list );

		if ( !tax_form_category->itemize_accounts ) continue;

		if ( !list_rewind( tax_form_category->account_list ) )
			continue;

		sprintf( sub_title,
			 "Line: %s, Category: %s, Sum: $%s",
			 tax_form_category->tax_form_line,
			 format_initial_capital(
				buffer,
			 	tax_form_category->tax_form_category_name ),
			 timlib_place_commas_in_money(
			 	tax_form_category->balance_sum ) );

		html_table = html_table_new(
				(char *)0 /* title */,
				sub_title,
				(char *)0 /* sub_sub_title */ );

		html_table->number_left_justified_columns = 1;
		html_table->number_right_justified_columns = 1;
		html_table_set_heading_list( html_table, heading_list );

		html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

		html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

		count = 0;

		do {
			account = list_get( tax_form_category->account_list );

			if ( !account->latest_ledger ) continue;

			if ( timlib_double_virtually_same(
				account->latest_ledger->balance,
				0.0 ) )
			{
				continue;
			}

			if ( ++count == ROWS_BETWEEN_HEADING )
			{
				html_table_output_data_heading(
					html_table->heading_list,
					html_table->
					number_left_justified_columns,
					html_table->
					number_right_justified_columns,
					html_table->justify_list );
				count = 0;
			}

			format_initial_capital(
				buffer,
				account->account_name );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );

			html_table_set_data(
				html_table->data_list,
				strdup( timlib_place_commas_in_money(
						account->
							latest_ledger->
							balance ) ) );

			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

				list_free( html_table->data_list );
				html_table->data_list = list_new();

		} while( list_next( tax_form_category->account_list ) );

		html_table_close();

	} while( list_next( tax_form_category_list ) );

} /* tax_form_detail_report_html_table() */

void tax_form_report_html_table(
			char *title,
			char *sub_title,
			LIST *tax_form_category_list )
{
	HTML_TABLE *html_table;
	LIST *heading_list;
	TAX_FORM_CATEGORY *tax_form_category;
	char buffer[ 128 ];
	int count = 0;

	heading_list = list_new();
	list_append_string( heading_list, "tax_form_line" );
	list_append_string( heading_list, "tax_form_category" );
	list_append_string( heading_list, "balance" );

	html_table = new_html_table(
			title,
			sub_title );

	html_table->number_left_justified_columns = 2;
	html_table->number_right_justified_columns = 1;
	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->justify_list );

	if ( !list_rewind( tax_form_category_list ) )
	{
		printf(
"<h3>ERROR: there are no tax form categories for this statement.</h3>\n" );
		html_table_close();
		document_close();
		exit( 1 );
	}

	do {
		tax_form_category = list_get( tax_form_category_list );

		if ( ++count == ROWS_BETWEEN_HEADING )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->
				number_left_justified_columns,
				html_table->
				number_right_justified_columns,
				html_table->justify_list );
			count = 0;
		}

		html_table_set_data(
			html_table->data_list,
			strdup( tax_form_category->tax_form_line ) );

		format_initial_capital(
			buffer,
			tax_form_category->tax_form_category_name );

		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		html_table_set_data(
			html_table->data_list,
			strdup( timlib_place_commas_in_money(
					tax_form_category->
						balance_sum ) ) );

		html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

			list_free( html_table->data_list );
			html_table->data_list = list_new();

	} while( list_next( tax_form_category_list ) );

	html_table_close();

} /* tax_form_report_html_table() */

void tax_form_report_PDF(
			char *application_name,
			char *title,
			char *sub_title,
			char *document_root_directory,
			char *process_name,
			LIST *tax_form_category_list )
{
	LATEX *latex;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	int pid = getpid();

	APPASERVER_LINK_FILE *appaserver_link_file;

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

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			application_constants_quick_fetch(
				application_name,
				"logo_filename" /* key */ ) );

	list_append_pointer(
		latex->table_list,
		tax_form_report_PDF_table(
			sub_title,
			tax_form_category_list ) );

	list_append_list(
		latex->table_list,
		tax_form_report_detail_PDF_table_list(
			tax_form_category_list ) );

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

} /* tax_form_report_PDF() */

LIST *build_PDF_row_list( LIST *tax_form_category_list )
{
	LATEX_ROW *latex_row;
	LIST *row_list;
	TAX_FORM_CATEGORY *tax_form_category;
	char buffer[ 128 ];

	if ( !list_rewind( tax_form_category_list ) )
	{
		printf(
"<h3>ERROR: there are no tax form categories for this statement.</h3>\n" );
		document_close();
		exit( 1 );
	}

	row_list = list_new();

	do {
		tax_form_category = list_get( tax_form_category_list );

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		list_append_pointer(
			latex_row->column_data_list,
			tax_form_category->tax_form_line );

		format_initial_capital(
			buffer,
			tax_form_category->tax_form_category_name );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( buffer ) );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( timlib_place_commas_in_money(
					tax_form_category->
						balance_sum ) ) );

	} while( list_next( tax_form_category_list ) );

	return row_list;

} /* build_PDF_row_list() */

LIST *build_detail_PDF_heading_list( void )
{
	LATEX_TABLE_HEADING *table_heading;
	LIST *heading_list;

	heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "account";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "balance";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	return heading_list;

} /* build_detail_PDF_heading_list() */

LIST *build_PDF_heading_list( void )
{
	LATEX_TABLE_HEADING *table_heading;
	LIST *heading_list;

	heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "tax_form_line";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "tax_form_category";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "balance";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	return heading_list;

} /* build_PDF_heading_list() */

LATEX_TABLE *tax_form_report_PDF_table(
			char *sub_title,
			LIST *tax_form_category_list )
{
	LATEX_TABLE *latex_table;

	latex_table =
		latex_new_latex_table(
			strdup( sub_title ) /* caption */ );

	latex_table->heading_list = build_PDF_heading_list();

	latex_table->row_list =
		build_PDF_row_list(
			tax_form_category_list );

	return latex_table;

} /* tax_form_report_PDF_table() */

LIST *tax_form_report_detail_PDF_table_list(
			LIST *tax_form_category_list )
{
	LATEX_TABLE *latex_table;
	TAX_FORM_CATEGORY *tax_form_category;
	LIST *table_list;
	char sub_title[ 128 ];
	char buffer[ 128 ];

	if ( !list_rewind( tax_form_category_list ) ) return (LIST *)0;

	table_list = list_new();

	do {
		tax_form_category = list_get( tax_form_category_list );

		if ( !tax_form_category->itemize_accounts ) continue;

		if ( !list_rewind( tax_form_category->account_list ) )
			continue;

		sprintf( sub_title,
			 "Line: %s, Category: %s, Sum: \\$%s",
			 tax_form_category->tax_form_line,
			 format_initial_capital(
				buffer,
			 	tax_form_category->tax_form_category_name ),
			 timlib_place_commas_in_money(
			 	tax_form_category->balance_sum ) );

		latex_table =
			latex_new_latex_table(
				strdup( sub_title ) /* caption */ );

		latex_table->heading_list = build_detail_PDF_heading_list();

		latex_table->row_list =
			build_detail_PDF_row_list(
				tax_form_category );

		list_append_pointer( table_list, latex_table );

	} while( list_next( tax_form_category_list ) );

	return table_list;

} /* tax_form_report_detail_PDF_table_list() */

LIST *build_detail_PDF_row_list( TAX_FORM_CATEGORY *tax_form_category )
{
	LIST *row_list;
	ACCOUNT *account;
	char buffer[ 128 ];
	LATEX_ROW *latex_row;

	if ( !list_rewind( tax_form_category->account_list ) )
		return (LIST *)0;

	row_list = list_new();

	do {
		account = list_get( tax_form_category->account_list );

		if ( !account->latest_ledger ) continue;

		if ( timlib_double_virtually_same(
			account->latest_ledger->balance,
			0.0 ) )
		{
			continue;
		}

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		format_initial_capital(
			buffer,
			account->account_name );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( buffer ) );

		list_append_pointer(
			latex_row->column_data_list,
			strdup( timlib_place_commas_in_money(
					account->
						latest_ledger->
						balance ) ) );

	} while( list_next( tax_form_category->account_list ) );

	return row_list;

} /* build_detail_PDF_row_list() */

