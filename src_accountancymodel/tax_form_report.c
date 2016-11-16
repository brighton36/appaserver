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
void tax_form_report_html_table(
					char *application_name,
					char *title,
					char *sub_title,
					char *as_of_date );

/*
LIST *build_PDF_row_list(		char *application_name,
					LIST *element_list );

LIST *build_PDF_heading_list(		void );

void tax_form_report_PDF(			char *application_name,
					char *fund_name,
					char *as_of_date,
					char *document_root_directory,
					char *process_name,
					char *aggregation );

void tax_form_report_PDF_fund(		
					LATEX *latex,
					char *application_name,
					char *sub_title,
					char *fund_name,
					char *as_of_date );
*/

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

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		tax_form_report_html_table(
			application_name,
			title,
			sub_title,
			as_of_date );
	}
/*
	else
	{
		tax_form_report_PDF(
			application_name,
			fund_name,
			as_of_date,
			appaserver_parameter_file->
				document_root,
			process_name,
			aggregation );
	}
*/

	document_close();
	exit( 0 );

} /* main() */

void tax_form_report_html_table(
			char *application_name,
			char *title,
			char *sub_title,
			char *as_of_date )
{
	HTML_TABLE *html_table;
	LIST *heading_list;
	LIST *tax_form_category_list;
	TAX_FORM_CATEGORY *tax_form_category;
	char buffer[ 128 ];
	int count = 0;

	tax_form_category_list =
		ledger_tax_form_fetch_category_list(
			application_name,
			as_of_date );

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

#ifdef NOT_DEFINED
void tax_form_report_PDF(
			char *application_name,
			char *fund_name,
			char *as_of_date,
			char *document_root_directory,
			char *process_name,
			char *aggregation )
{
	LATEX *latex;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	int pid = getpid();
	char title[ 256 ];
	char sub_title[ 256 ];
	LIST *fund_name_list;
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

	fund_name_list = ledger_get_fund_name_list( application_name );

	ledger_get_report_title_sub_title(
		title,
		sub_title,
		process_name,
		application_name,
		fund_name,
		as_of_date,
		list_length( fund_name_list ) );

	printf( "<h1>%s</h1>\n", title );

/*
	sprintf(	working_directory,
			"%s/%s",
			appaserver_mount_point,
			application_name );

	sprintf(	latex_filename,
			LATEX_FILE_TEMPLATE,
			pid );

	sprintf(	dvi_filename,
			LATEX_DVI_FILE_TEMPLATE,
			pid );
*/

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			application_constants_quick_fetch(
				application_name,
				"logo_filename" /* key */ ) );

	if ( list_rewind( fund_name_list )
	&&   strcmp( aggregation, "sequential" ) == 0 )
	{
		do {
			fund_name = list_get_pointer( fund_name_list );

			ledger_get_report_title_sub_title(
				title,
				sub_title,
				process_name,
				application_name,
				fund_name,
				as_of_date,
				0 /* fund_name_list_length */ );

			tax_form_report_PDF_fund(
					latex,
					application_name,
					sub_title,
					fund_name,
					as_of_date );

		} while( list_next( fund_name_list ) );
	}
	else
	{
		/* Either single fund or consolidated. */
		/* ----------------------------------- */
		tax_form_report_PDF_fund(
				latex,
				application_name,
				sub_title,
				fund_name,
				as_of_date );
	}

	latex_table_output(
		latex->output_stream,
		latex->landscape_flag,
		latex->table_list,
		latex->logo_filename );

	fclose( latex->output_stream );

/*
	sprintf( ftp_output_filename,
		 FTP_OUTPUT_FILE_TEMPLATE,
		 appaserver_library_get_server_address(),
		 application_name,
		 pid,
		 "pdf" );
*/
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

void tax_form_report_PDF_fund(
			LATEX *latex,
			char *application_name,
			char *sub_title,
			char *fund_name,
			char *as_of_date )
{
	LATEX_TABLE *latex_table;
	LIST *element_list;

	printf( "<h2>%s</h2>\n", sub_title );

	latex_table =
		latex_new_latex_table(
			strdup( sub_title ) /* caption */ );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list = build_PDF_heading_list();

	element_list =
		ledger_get_element_list(
			application_name,
			(LIST *)0 /* filter_element_name_list */,
			fund_name,
			as_of_date );

	latex_table->row_list =
		build_PDF_row_list(
			application_name,
			element_list );

} /* tax_form_report_PDF_fund() */

LIST *build_PDF_row_list(	char *application_name,
				LIST *element_list )
{
	LATEX_ROW *latex_row;
	LIST *row_list;
	char *debit_amount;
	char *credit_amount;
	double debit_sum = 0.0;
	double credit_sum = 0.0;
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	boolean accumulate_debit;
	char element_title[ 128 ];
	char subclassification_title[ 128 ];
	char *account_title;
	char transaction_count_string[ 16 ];
	char transaction_date_string[ 16 ];
	double balance;
	char transaction_date_american[ 16 ];

	if ( !list_rewind( element_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		element = list_get_pointer( element_list );

		if ( !list_rewind( element->subclassification_list ) )
			continue;

		do {
			subclassification =
				list_get_pointer(
					element->
					   subclassification_list );

			if ( !list_rewind( subclassification->account_list ) )
				continue;

			do {
				account = 
					list_get_pointer(
						subclassification->
							account_list );

				if ( !account->latest_ledger
				||   !account->latest_ledger->balance )
					continue;

				accumulate_debit =
					ledger_account_get_accumulate_debit(
						application_name,
						account->account_name );

				latex_row = latex_new_latex_row();
				list_append_pointer( row_list, latex_row );

				if ( element->element_name )
				{
					format_initial_capital(
						element_title,
						element->element_name );

				}
				else
				{
					*element_title = '\0';
				}

				if ( subclassification->subclassification_name )
				{
					format_initial_capital(
						subclassification_title,
						subclassification->
						     subclassification_name );
				}
				else
				{
					*subclassification_title = '\0';
				}

				list_append_pointer(
					latex_row->column_data_list,
					strdup( element_title ) );

				list_append_pointer(
					latex_row->column_data_list,
					strdup( subclassification_title ) );

				date_convert_source_international(
					transaction_date_american,
					american,
			 		column( transaction_date_string,
			 			0,
			 			account->
						latest_ledger->
						transaction_date_time ) );

				account_title =
					get_latex_account_title(
						account->account_name,
						account->
							latest_ledger->
							full_name,
						account->
							latest_ledger->
							debit_amount,
						account->
							latest_ledger->
							credit_amount,
						transaction_date_american,
						account->
							latest_ledger->
							memo );

				list_append_pointer(
					latex_row->column_data_list,
					strdup( account_title ) );

				sprintf(transaction_count_string,
					"%d",
					account->
						latest_ledger->
						transaction_count );

				list_append_pointer(
					latex_row->column_data_list,
					strdup( transaction_count_string ) );

				balance = account->latest_ledger->balance;

				/* See if negative balance. */
				/* ------------------------ */
				if ( balance < 0.0 )
				{
					balance = float_abs( balance );
					accumulate_debit = 1 - accumulate_debit;
				}

				if ( accumulate_debit )
				{
					debit_amount =
						place_commas_in_money(
							balance );
					debit_sum += balance;
				}
				else
					debit_amount = "";

				list_append_pointer(
					latex_row->column_data_list,
					strdup( debit_amount ) );

				if ( !accumulate_debit )
				{
					credit_amount =
						place_commas_in_money(
							balance );
					credit_sum += balance;
				}
				else
					credit_amount = "";

				list_append_pointer(
					latex_row->column_data_list,
					strdup( credit_amount ) );

				subclassification->
					subclassification_name =
						(char *)0;

				element->element_name = (char *)0;

			} while( list_next( subclassification->account_list ) );

		} while( list_next( element->subclassification_list ) );

	} while( list_next( element_list ) );

	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

	list_append_pointer( latex_row->column_data_list, "Total" );
	list_append_pointer( latex_row->column_data_list, (char *)0 );
	list_append_pointer( latex_row->column_data_list, (char *)0 );
	list_append_pointer( latex_row->column_data_list, (char *)0 );

	debit_amount = place_commas_in_money( debit_sum );
	list_append_pointer(
		latex_row->column_data_list,
		strdup( debit_amount ) );

	credit_amount = place_commas_in_money( credit_sum );
	list_append_pointer(
		latex_row->column_data_list,
		strdup( credit_amount ) );

	return row_list;

} /* build_PDF_row_list() */

LIST *build_PDF_heading_list( void )
{
	LATEX_TABLE_HEADING *table_heading;
	LIST *heading_list;

	heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Element";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Subclassification";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Account";
	table_heading->paragraph_size = "5.5cm";
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Transactions";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Debit";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Credit";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	return heading_list;

} /* build_PDF_heading_list() */

char *get_latex_account_title(	char *account_name,
				char *full_name,
				double debit_amount,
				double credit_amount,
				char *transaction_date,
				char *memo )
{
	static char account_title[ 128 ];
	char *ptr = account_title;
	char account_name_formatted[ 128 ];
	char full_name_formatted[ 128 ];
	char *transaction_amount_string;

	if ( !transaction_date || !*transaction_date )
		return "Can't get account title";

	*ptr = '\0';

	format_initial_capital( account_name_formatted, account_name );
	ptr += sprintf( ptr, "\\textbf{%s}", account_name_formatted );

	ptr += sprintf( ptr, " (\\scriptsize{%s:", transaction_date );

	format_initial_capital( full_name_formatted, full_name );
	ptr += sprintf( ptr, " %s", full_name_formatted );

	if ( memo && *memo )
	{
		ptr += sprintf( ptr, "; %s", memo );
	}

	if ( debit_amount )
	{
		transaction_amount_string =
			place_commas_in_money(
				debit_amount );
	}
	else
	if ( credit_amount )
	{
		transaction_amount_string =
			place_commas_in_money(
				credit_amount );
	}
	else
	{
		transaction_amount_string = "Unknown";
	}

	ptr += sprintf( ptr, " $%s})", transaction_amount_string );

	return account_title;

} /* get_latex_account_title() */
#endif

