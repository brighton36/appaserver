/* ---------------------------------------------------------------	*/
/* src_accountancymodel/balance_sheet.c					*/
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
LIST *build_aggregate_subclassification_PDF_row_list(
					LIST *element_list,
					char *application_name,
					char *fund_name,
					char *as_of_date,
					boolean is_financial_position );

LIST *build_full_PDF_row_list(		LIST *element_list,
					char *application_name,
					char *fund_name,
					char *as_of_date,
					boolean is_financial_position );

LIST *build_consolidate_PDF_heading_list(
					void );

LIST *build_full_PDF_heading_list(	void );

void balance_sheet_aggregate_subclassification_PDF(
					char *application_name,
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					char *document_root_directory,
					char *process_name,
					boolean is_financial_position,
					char *logo_filename );

void balance_sheet_full_PDF(
					char *application_name,
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					char *document_root_directory,
					char *process_name,
					boolean is_financial_position,
					char *logo_filename );

void balance_sheet_consolidate_html_table(
					char *application_name,
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					boolean is_financial_position );

void balance_sheet_full_html_table(
					char *application_name,
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					boolean is_financial_position );

double get_net_income(			char *application_name,
					char *fund_name,
					char *as_of_date );

void output_liabilities_plus_equity(
					HTML_TABLE *html_table,
					double liabilities_plus_equity,
					boolean aggregate_subclassification );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *fund_name;
	boolean aggregate_subclassification;
	char *as_of_date;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 256 ];
	char sub_title[ 256 ];
	char *database_string = {0};
	char *output_medium;
	boolean is_financial_position;
	char *logo_filename;

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s application process fund as_of_date aggregate_subclassification_yn output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	is_financial_position =
		( strcmp( argv[ 0 ], "financial_position" ) == 0 );

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
	fund_name = argv[ 3 ];
	as_of_date = argv[ 4 ];
	aggregate_subclassification = ( *argv[ 5 ] == 'y' );
	output_medium = argv[ 6 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( !*as_of_date
	||   strcmp(	as_of_date,
			"as_of_date" ) == 0 )
	{
		as_of_date =
			date_get_now_yyyy_mm_dd(
				date_get_utc_offset() );
	}

	document = document_new( (char *)0 /* title */, application_name );
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

	logo_filename =
		application_constants_quick_fetch(
			application_name,
			"logo_filename" /* key */ );

	if ( !ledger_get_report_title_sub_title(
		title,
		sub_title,
		process_name,
		application_name,
		fund_name,
		as_of_date,
		list_length(
			ledger_get_fund_name_list(
				application_name ) ),
		logo_filename ) )
	{
		printf( "<h3>Error. No transactions.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		if ( aggregate_subclassification )
		{
			balance_sheet_consolidate_html_table(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				is_financial_position );
		}
		else
		{
			balance_sheet_full_html_table(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				is_financial_position );
		}
	}
	else
	{
		if ( aggregate_subclassification )
		{
			balance_sheet_aggregate_subclassification_PDF(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				appaserver_parameter_file->document_root,
				process_name,
				is_financial_position,
				logo_filename );
		}
		else
		{
			balance_sheet_full_PDF(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				appaserver_parameter_file->document_root,
				process_name,
				is_financial_position,
				logo_filename );
		}
	}

	document_close();

	return 0;

} /* main() */

void balance_sheet_aggregate_subclassification_PDF(
				char *application_name,
				char *title,
				char *sub_title,
				char *fund_name,
				char *as_of_date,
				char *document_root_directory,
				char *process_name,
				boolean is_financial_position,
				char *logo_filename )
{
	LATEX *latex;
	LATEX_TABLE *latex_table;
	LIST *element_list;
	LIST *filter_element_name_list;
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

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			logo_filename );

	latex_table =
		latex_new_latex_table(
			sub_title /* caption */ );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list = build_consolidate_PDF_heading_list();

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				LEDGER_ASSET_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LIABILITY_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EQUITY_ELEMENT );

	element_list =
		ledger_get_element_list(
				application_name,
				filter_element_name_list,
				fund_name,
				as_of_date );

	latex_table->row_list =
		build_aggregate_subclassification_PDF_row_list(
			element_list,
			application_name,
			fund_name,
			as_of_date,
			is_financial_position );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

	if ( !list_length( latex_table->row_list ) )
	{
		printf(
		"<p>ERROR: there are no elements for this statement.\n" );
		document_close();
		exit( 1 );
	}

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

} /* balance_sheet_aggregate_subclassification_PDF() */

void balance_sheet_full_PDF(	char *application_name,
				char *title,
				char *sub_title,
				char *fund_name,
				char *as_of_date,
				char *document_root_directory,
				char *process_name,
				boolean is_financial_position,
				char *logo_filename )
{
	LATEX *latex;
	LATEX_TABLE *latex_table;
	LIST *element_list;
	LIST *filter_element_name_list;
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

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			logo_filename );

	latex_table =
		latex_new_latex_table(
			sub_title /* caption */ );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list = build_full_PDF_heading_list();

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				LEDGER_ASSET_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LIABILITY_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EQUITY_ELEMENT );

	element_list =
		ledger_get_element_list(
				application_name,
				filter_element_name_list,
				fund_name,
				as_of_date );

	latex_table->row_list =
		build_full_PDF_row_list(
			element_list,
			application_name,
			fund_name,
			as_of_date,
			is_financial_position );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

	if ( !list_length( latex_table->row_list ) )
	{
		printf(
		"<p>ERROR: there are no elements for this statement.\n" );
		document_close();
		exit( 1 );
	}

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

} /* balance_sheet_full_PDF() */

void balance_sheet_consolidate_html_table(
				char *application_name,
				char *title,
				char *sub_title,
				char *fund_name,
				char *as_of_date,
				boolean is_financial_position )
{
	HTML_TABLE *html_table;
	LIST *filter_element_name_list;
	LIST *element_list;
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *net_income_account;
	double total_liabilities;
	double total_equity;
	double net_income;

	html_table = new_html_table( title, sub_title );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 2;

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				LEDGER_ASSET_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LIABILITY_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EQUITY_ELEMENT );

	element_list =
		ledger_get_element_list(
				application_name,
				filter_element_name_list,
				fund_name,
				as_of_date );

	/* Output total_assets */
	/* ------------------- */
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_ASSET_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_ASSET_ELEMENT );
	}

	ledger_output_subclassification_html_element(
					html_table,
					element->subclassification_list,
					LEDGER_ASSET_ELEMENT,
					element->accumulate_debit,
					0.0 /* percent_denominator */ );

	/* Calculate total_liabilities */
	/* --------------------------- */
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_LIABILITY_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LIABILITY_ELEMENT );
	}

	total_liabilities = ledger_output_subclassification_html_element(
					html_table,
					element->subclassification_list,
					LEDGER_LIABILITY_ELEMENT,
					element->accumulate_debit,
					0.0 /* percent_denominator */ );

	/* Calculate total_equity */
	/* ----------------------- */
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_EQUITY_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EQUITY_ELEMENT );
	}

	/* Add net income to equity */
	/* ------------------------- */
	net_income = get_net_income(	application_name,
					fund_name,
					as_of_date );

	if ( is_financial_position )
	{
		net_income_account =
			ledger_account_new(
				LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS );
	}
	else
	{
		net_income_account =
			ledger_account_new(
				LEDGER_ACCOUNT_NET_INCOME );
	}

	net_income_account->accumulate_debit = 0;

	net_income_account->latest_ledger =
			journal_ledger_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				net_income_account->account_name );

	net_income_account->latest_ledger->balance = net_income;

	subclassification =
		ledger_new_subclassification(
			net_income_account->account_name );

	subclassification->account_list = list_new();

	list_append_pointer(	subclassification->account_list,
				net_income_account );

	list_append_pointer(	element->subclassification_list,
				subclassification );

	total_equity = ledger_output_subclassification_html_element(
					html_table,
					element->subclassification_list,
					LEDGER_EQUITY_ELEMENT,
					element->accumulate_debit,
					0.0 /* percent_denominator */ );

	output_liabilities_plus_equity(
					html_table,
					total_liabilities + total_equity,
					1 /* aggregate_subclassification */ );

	html_table_close();

} /* balance_sheet_consolidate_html_table() */

void balance_sheet_full_html_table(
				char *application_name,
				char *title,
				char *sub_title,
				char *fund_name,
				char *as_of_date,
				boolean is_financial_position )
{
	HTML_TABLE *html_table;
	LIST *filter_element_name_list;
	LIST *element_list;
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *net_income_account;
	double total_liabilities;
	double total_equity;
	double net_income;

	html_table = new_html_table(
			title,
			sub_title );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 3;
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				LEDGER_ASSET_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LIABILITY_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EQUITY_ELEMENT );

	element_list =
		ledger_get_element_list(
				application_name,
				filter_element_name_list,
				fund_name,
				as_of_date );

	/* Output total_assets */
	/* ------------------- */
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_ASSET_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_ASSET_ELEMENT );
	}

	ledger_output_html_element(	html_table,
					element->subclassification_list,
					LEDGER_ASSET_ELEMENT,
					element->accumulate_debit,
					0.0 /* percent_denominator */ );

	/* Calculate total_liabilities */
	/* --------------------------- */
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_LIABILITY_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LIABILITY_ELEMENT );
	}

	total_liabilities = ledger_output_html_element(
					html_table,
					element->subclassification_list,
					LEDGER_LIABILITY_ELEMENT,
					element->accumulate_debit,
					0.0 /* percent_denominator */ );

	/* Calculate total_equity */
	/* ----------------------- */
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_EQUITY_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EQUITY_ELEMENT );
	}

	/* Add net income to equity */
	/* ------------------------- */
	net_income = get_net_income(	application_name,
					fund_name,
					as_of_date );

	if ( is_financial_position )
	{
		net_income_account =
			ledger_account_new(
				LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS );
	}
	else
	{
		net_income_account =
			ledger_account_new(
				LEDGER_ACCOUNT_NET_INCOME );
	}

	net_income_account->accumulate_debit = 0;

	net_income_account->latest_ledger =
			journal_ledger_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				net_income_account->account_name );

	net_income_account->latest_ledger->balance = net_income;

	subclassification =
		ledger_new_subclassification(
			net_income_account->account_name );

	subclassification->account_list = list_new();

	list_append_pointer(	subclassification->account_list,
				net_income_account );

	list_append_pointer(	element->subclassification_list,
				subclassification );

	total_equity = ledger_output_html_element(
					html_table,
					element->subclassification_list,
					LEDGER_EQUITY_ELEMENT,
					element->accumulate_debit,
					0.0 /* percent_denominator */ );

	output_liabilities_plus_equity(
				html_table,
				total_liabilities + total_equity,
				0 /* not aggregate_subclassification */ );

	html_table_close();

} /* balance_sheet_full_html_table() */

void output_liabilities_plus_equity(
			HTML_TABLE *html_table,
			double liabilities_plus_equity,
			boolean aggregate_subclassification )
{
	char element_title[ 128 ];

	sprintf(	element_title,
			"<h2>Liabilities + Equity</h2>" );

	html_table_set_data( html_table->data_list, element_title );

	html_table_set_data(	html_table->data_list, "" );

	if ( !aggregate_subclassification )
		html_table_set_data( html_table->data_list, "" );

	html_table_set_data(	html_table->data_list,
				strdup( place_commas_in_money(
					liabilities_plus_equity ) ) );

	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	html_table->data_list = list_new();

} /* output_liabilities_plus_equity() */

double get_net_income(	char *application_name,
			char *fund_name,
			char *as_of_date )
{
	char sys_string[ 1024 ];
	char *results_string;

	sprintf(sys_string,
"income_statement \"%s\" process \"%s\" \"%s\" %s output_medium y",
		application_name,
		(fund_name) ? fund_name : "",
		as_of_date,
		"ignored" );

	results_string = pipe2string( sys_string );

	if ( !results_string )
		return 0.0;
	else
		return atof( results_string );

} /* get_net_income() */

LIST *build_consolidate_PDF_heading_list( void )
{
	LATEX_TABLE_HEADING *table_heading;
	LIST *heading_list;

	heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	return heading_list;

} /* build_consolidate_PDF_heading_list() */

LIST *build_full_PDF_heading_list( void )
{
	LATEX_TABLE_HEADING *table_heading;
	LIST *heading_list;

	heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Account";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Subclassification";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Element";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	return heading_list;

} /* build_full_PDF_heading_list() */

LIST *build_aggregate_subclassification_PDF_row_list(
					LIST *element_list,
					char *application_name,
					char *fund_name,
					char *as_of_date,
					boolean is_financial_position )
{
	ELEMENT *element;
	double total_assets = 0.0;
	double total_liabilities = 0.0;
	double total_equity = 0.0;
	double net_income = 0.0;
	LIST *row_list;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *net_income_account;

	row_list = list_new();

	/* Compute total_assets */
	/* -------------------- */
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_ASSET_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_ASSET_ELEMENT );
	}

	list_append_list(	row_list,
				ledger_get_subclassification_latex_row_list(
					&total_assets,
					element->subclassification_list,
					LEDGER_ASSET_ELEMENT,
					element->accumulate_debit ) );

	/* Compute total_liabilities */
	/* ------------------------- */
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_LIABILITY_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LIABILITY_ELEMENT );
	}

	list_append_list(	row_list,
				ledger_get_subclassification_latex_row_list(
					&total_liabilities,
					element->subclassification_list,
					LEDGER_LIABILITY_ELEMENT,
					element->accumulate_debit ) );

	/* Calculate total_equity */
	/* ----------------------- */
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_EQUITY_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EQUITY_ELEMENT );
	}

	/* Build net income subclassification. */
	/* ----------------------------------- */
	net_income = get_net_income(	application_name,
					fund_name,
					as_of_date );

	if ( is_financial_position )
	{
		net_income_account =
			ledger_account_new(
				LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS );
	}
	else
	{
		net_income_account =
			ledger_account_new(
				LEDGER_ACCOUNT_NET_INCOME );
	}

	net_income_account->accumulate_debit = 0;

	net_income_account->latest_ledger =
			journal_ledger_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				net_income_account->account_name );

	net_income_account->latest_ledger->balance = net_income;

	subclassification =
		ledger_new_subclassification(
			net_income_account->account_name );

	subclassification->subclassification_total = net_income;

	subclassification->account_list = list_new();

	list_append_pointer(	subclassification->account_list,
				net_income_account );

	list_append_pointer(	element->subclassification_list,
				subclassification );

	list_append_list(	row_list,
				ledger_get_subclassification_latex_row_list(
					&total_equity,
					element->subclassification_list,
					LEDGER_EQUITY_ELEMENT,
					element->accumulate_debit ) );

	list_append_pointer(	row_list,
				ledger_get_latex_liabilities_plus_equity_row(
					total_liabilities + total_equity,
					1 /* aggregate_subclassification */ ) );

	return row_list;

} /* build_aggregate_subclassification_PDF_row_list() */

LIST *build_full_PDF_row_list(	LIST *element_list,
				char *application_name,
				char *fund_name,
				char *as_of_date,
				boolean is_financial_position )
{
	ELEMENT *element;
	double total_assets = 0.0;
	double total_liabilities = 0.0;
	double total_equity = 0.0;
	double net_income = 0.0;
	LIST *row_list;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *net_income_account;

	row_list = list_new();

	/* Compute total_assets */
	/* -------------------- */
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_ASSET_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_ASSET_ELEMENT );
	}

	list_append_list(	row_list,
				ledger_get_latex_row_list(
					&total_assets,
					element->subclassification_list,
					LEDGER_ASSET_ELEMENT,
					element->accumulate_debit,
					0.0 /* percent_denominator */ ) );

	/* Compute total_liabilities */
	/* ------------------------- */
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_LIABILITY_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LIABILITY_ELEMENT );
	}

	list_append_list(	row_list,
				ledger_get_latex_row_list(
					&total_liabilities,
					element->subclassification_list,
					LEDGER_LIABILITY_ELEMENT,
					element->accumulate_debit,
					0.0 /* percent_denominator */ ) );

	/* Calculate total_equity */
	/* ----------------------- */
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_EQUITY_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EQUITY_ELEMENT );
	}

	/* Build net income subclassification. */
	/* ----------------------------------- */
	net_income = get_net_income(	application_name,
					fund_name,
					as_of_date );

	if ( is_financial_position )
	{
		net_income_account =
			ledger_account_new(
				LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS );
	}
	else
	{
		net_income_account =
			ledger_account_new(
				LEDGER_ACCOUNT_NET_INCOME );
	}

	net_income_account->accumulate_debit = 0;

	net_income_account->latest_ledger =
			journal_ledger_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				net_income_account->account_name );

	net_income_account->latest_ledger->balance = net_income;

	subclassification =
		ledger_new_subclassification(
					net_income_account->account_name );
	subclassification->account_list = list_new();

	list_append_pointer(	subclassification->account_list,
				net_income_account );

	list_append_pointer(	element->subclassification_list,
				subclassification );

	list_append_list(	row_list,
				ledger_get_latex_row_list(
					&total_equity,
					element->subclassification_list,
					LEDGER_EQUITY_ELEMENT,
					element->accumulate_debit,
					0.0 /* percent_denominator */ ) );

	list_append_pointer(
			row_list,
			ledger_get_latex_liabilities_plus_equity_row(
				total_liabilities + total_equity,
				0 /* not aggregate_subclassification */ ) );

	return row_list;

} /* build_full_PDF_row_list() */

