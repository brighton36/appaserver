/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/income_statement.c			*/
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
#include "date.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define PROMPT				"Press here to view statement."

/* Prototypes */
/* ---------- */
void income_statement_subclassification_omit_PDF(
					char *application_name,
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					char *document_root_directory,
					boolean is_statement_of_activities,
					char *process_name,
					char *logo_filename );

void income_statement_net_income_only(
					char *application_name,
					char *fund_name,
					char *as_of_date );

LIST *build_subclassification_display_PDF_heading_list(
					void );

LIST *build_subclassification_omit_PDF_heading_list(
					void );

LIST *build_subclassification_aggregate_PDF_heading_list(
					void );

LIST *build_subclassification_omit_PDF_row_list(
					LIST *element_list,
					boolean is_statement_of_activities );

LIST *build_subclassification_display_PDF_row_list(
					LIST *element_list,
					boolean is_statement_of_activities );

LIST *build_subclassification_aggregate_PDF_row_list(
					LIST *element_list,
					boolean is_statement_of_activities );

void income_statement_subclassification_aggregate_html_table(
					char *application_name,
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					boolean is_statement_of_activities );

void income_statement_subclassification_display_html_table(
					char *application_name,
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					boolean is_statement_of_activities );

void income_statement_subclassification_omit_html_table(
					char *application_name,
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					boolean is_statement_of_activities );

void income_statement_subclassification_aggregate_PDF(
					char *application_name,
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					char *document_root_directory,
					boolean is_statement_of_activities,
					char *process_name,
					char *logo_filename );

void income_statement_subclassification_display_PDF(
					char *application_name,
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					char *document_root_directory,
					boolean is_statement_of_activities,
					char *process_name,
					char *logo_filename );

double get_shares_outstanding(		char *appaserver_mount_point,
					char *application_name,
					char *as_of_date );

void output_earnings_per_share(		HTML_TABLE *html_table,
					double net_income,
					double shares_outstanding );

double get_income_tax_expense(		LIST *account_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *fund_name;
	char *as_of_date;
	char *subclassification_option;
	boolean net_income_only = 0;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *output_medium;
	char title[ 256 ];
	char sub_title[ 256 ];
	boolean is_statement_of_activities;
	char *logo_filename;
/*
	double shares_outstanding;
*/

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc < 7 )
	{
		fprintf( stderr,
"Usage: %s ignored process fund as_of_date subclassification_option output_medium [net_income_only_yn]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	is_statement_of_activities =
		( strcmp( argv[ 0 ], "statement_of_activities" ) == 0 );

	process_name = argv[ 2 ];
	fund_name = argv[ 3 ];
	as_of_date = argv[ 4 ];
	subclassification_option = argv[ 5 ];

	if ( strcmp( subclassification_option, "aggregate" ) != 0
	&&   strcmp( subclassification_option, "display" ) != 0
	&&   strcmp( subclassification_option, "omit" ) != 0 )
	{
		subclassification_option = "display";
	}

	output_medium = argv[ 6 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

	if ( argc == 8 )
	{
		net_income_only = (*argv[ 7 ] == 'y');
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !*as_of_date
	||   strcmp(	as_of_date,
			"as_of_date" ) == 0 )
	{
		as_of_date = date_get_now_yyyy_mm_dd( date_get_utc_offset() );
	}

	if ( net_income_only )
	{
		income_statement_net_income_only(
			application_name,
			fund_name,
			as_of_date );

		exit( 0 );
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

	if ( !(boolean)ledger_get_report_title_sub_title(
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
		if ( strcmp( subclassification_option, "aggregate" ) == 0 )
		{
			income_statement_subclassification_aggregate_html_table(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				is_statement_of_activities );
		}
		else
		if ( strcmp( subclassification_option, "display" ) == 0 )
		{
			income_statement_subclassification_display_html_table(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				is_statement_of_activities );
		}
		else
		/* ------------ */
		/* Must be omit */
		/* ------------ */
		{
			income_statement_subclassification_omit_html_table(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				is_statement_of_activities );
		}
	}
	else
	/* ----------- */
	/* Must be PDF */
	/* ----------- */
	{
		if ( strcmp( subclassification_option, "aggregate" ) == 0 )
		{
			income_statement_subclassification_aggregate_PDF(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				appaserver_parameter_file->
					document_root,
				is_statement_of_activities,
				process_name,
				logo_filename );
		}
		else
		if ( strcmp( subclassification_option, "display" ) == 0 )
		{
			income_statement_subclassification_display_PDF(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				appaserver_parameter_file->
					document_root,
				is_statement_of_activities,
				process_name,
				logo_filename );
		}
		else
		/* ------------ */
		/* Must be omit */
		/* ------------ */
		{
			income_statement_subclassification_omit_PDF(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				appaserver_parameter_file->
					document_root,
				is_statement_of_activities,
				process_name,
				logo_filename );
		}
	}

	return 0;

} /* main() */

void income_statement_net_income_only(
			char *application_name,
			char *fund_name,
			char *as_of_date )
{
	LIST *element_list;
	LEDGER_ELEMENT *element;
	LIST *filter_element_name_list;
	double total_revenues;
	double total_expenses;
	double total_gains;
	double total_losses;
	double net_income;

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				LEDGER_REVENUE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EXPENSE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_GAIN_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LOSS_ELEMENT );

	element_list =
		ledger_get_element_list(
			application_name,
			filter_element_name_list,
			fund_name,
			as_of_date,
			0 /* not omit_subclassification */ );

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_REVENUE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_REVENUE_ELEMENT );
	}

	total_revenues = ledger_get_element_value(
					element->subclassification_list,
					element->accumulate_debit );

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_EXPENSE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EXPENSE_ELEMENT );
	}

	total_expenses = ledger_get_element_value(
					element->subclassification_list,
					element->accumulate_debit );

	/* Compute total gains */
	/* ------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_GAIN_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_GAIN_ELEMENT );
	}

	total_gains = ledger_get_element_value(
				element->subclassification_list,
				element->accumulate_debit );

	/* Compute total losses */
	/* -------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_LOSS_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LOSS_ELEMENT );
	}

	total_losses = ledger_get_element_value(
					element->subclassification_list,
					element->accumulate_debit );

	net_income = ledger_get_net_income(
				total_revenues,
				total_expenses,
				total_gains,
				total_losses );

	printf( "%.2lf\n", net_income );

} /* income_statement_net_income_only() */

void income_statement_subclassification_aggregate_html_table(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			boolean is_statement_of_activities )
{
	LIST *element_list;
	LEDGER_ELEMENT *element;
	LIST *filter_element_name_list;
	HTML_TABLE *html_table;
	double total_revenues;
	double total_expenses;
	double total_gains;
	double total_losses;
	double net_income;

	html_table = html_table_new( title, sub_title, "" );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 3;

	html_table_set_heading( html_table, "" );
	html_table_set_heading( html_table, "Subclassification" );
	html_table_set_heading( html_table, "Element" );
	html_table_set_heading( html_table, "Percent" );

	html_table_output_table_heading(
		html_table->title,
		html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		(LIST *)0 /* justify_list */ );

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				LEDGER_REVENUE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EXPENSE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_GAIN_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LOSS_ELEMENT );

	element_list =
		ledger_get_element_list(
			application_name,
			filter_element_name_list,
			fund_name,
			as_of_date,
			0 /* not omit_subclassification */ );

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_REVENUE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_REVENUE_ELEMENT );
	}

	total_revenues =
		ledger_output_subclassification_aggregate_html_element(
			html_table,
			element->subclassification_list,
			element->element_name,
			element->element_total
				/* percent_denominator */ );

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_EXPENSE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EXPENSE_ELEMENT );
	}

	total_expenses =
		ledger_output_subclassification_aggregate_html_element(
			html_table,
			element->subclassification_list,
			element->element_name,
			total_revenues /* percent_denominator */ );

	/* Compute total gains */
	/* ------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_GAIN_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_GAIN_ELEMENT );
	}

	total_gains =
		ledger_output_subclassification_aggregate_html_element(
			html_table,
			element->subclassification_list,
			element->element_name,
			total_revenues /* percent_denominator */ );

	/* Compute total losses */
	/* -------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_LOSS_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LOSS_ELEMENT );
	}

	total_losses =
		ledger_output_subclassification_aggregate_html_element(
			html_table,
			element->subclassification_list,
			element->element_name,
			total_revenues
				/* percent_denominator */ );

	net_income = ledger_get_net_income(
				total_revenues,
				total_expenses,
				total_gains,
				total_losses );

	ledger_output_subclassification_aggregate_net_income(
		html_table,
		net_income,
		is_statement_of_activities,
		total_revenues /* percent_denominator */ );

	html_table_close();
	document_close();

} /* income_statement_subclassification_aggregate_html_table() */

void income_statement_subclassification_display_html_table(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			boolean is_statement_of_activities )
{
	LIST *element_list;
	LEDGER_ELEMENT *element;
	LIST *filter_element_name_list;
	HTML_TABLE *html_table;
	double total_revenues;
	double total_expenses;
	double total_gains;
	double total_losses;
	double net_income;

	html_table = html_table_new( title, sub_title, "" );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 4;

	html_table_set_heading( html_table, "" );
	html_table_set_heading( html_table, "Account" );
	html_table_set_heading( html_table, "Subclassification" );
	html_table_set_heading( html_table, "Element" );
	html_table_set_heading( html_table, "Percent" );

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		(LIST *)0 /* justify_list */ );

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				LEDGER_REVENUE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EXPENSE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_GAIN_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LOSS_ELEMENT );

	element_list =
		ledger_get_element_list(
			application_name,
			filter_element_name_list,
			fund_name,
			as_of_date,
			0 /* not omit_subclassification */ );

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_REVENUE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_REVENUE_ELEMENT );
	}

	total_revenues =
		ledger_output_html_subclassification_list(
			html_table,
			element->subclassification_list,
			LEDGER_REVENUE_ELEMENT,
			element->accumulate_debit,
			element->element_total
				/* percent_denominator */ );

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_EXPENSE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EXPENSE_ELEMENT );
	}

	total_expenses =
		ledger_output_html_subclassification_list(
			html_table,
			element->subclassification_list,
			LEDGER_EXPENSE_ELEMENT,
			element->accumulate_debit,
			total_revenues
				/* percent_denominator */ );

	/* Compute total gains */
	/* ------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_GAIN_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_GAIN_ELEMENT );
	}

	total_gains =
		ledger_output_html_subclassification_list(
			html_table,
			element->subclassification_list,
			LEDGER_GAIN_ELEMENT,
			element->accumulate_debit,
			total_revenues
				/* percent_denominator */ );

	/* Compute total losses */
	/* -------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_LOSS_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LOSS_ELEMENT );
	}

	total_losses =
		ledger_output_html_subclassification_list(
			html_table,
			element->subclassification_list,
			LEDGER_LOSS_ELEMENT,
			element->accumulate_debit,
			total_revenues
				/* percent_denominator */ );

	net_income = ledger_get_net_income(
				total_revenues,
				total_expenses,
				total_gains,
				total_losses );

	ledger_output_net_income(
		html_table,
		net_income,
		is_statement_of_activities,
		total_revenues,
		2 /* skip_columns */);

/*
		shares_outstanding = get_shares_outstanding(
					appaserver_parameter_file->
						appaserver_mount_point,
					application_name,
					as_of_date );
		if( shares_outstanding )
		{
			output_earnings_per_share(
						html_table,
						net_income,
						shares_outstanding );
		}
*/

	html_table_close();
	document_close();

} /* income_statement_subclassification_display_html_table() */

void income_statement_subclassification_aggregate_PDF(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			char *document_root_directory,
			boolean is_statement_of_activities,
			char *process_name,
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
	LEDGER_ELEMENT *element;

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
			logo_filename );

	latex_table =
		latex_new_latex_table(
			sub_title /* caption */ );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list =
		build_subclassification_aggregate_PDF_heading_list();

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				LEDGER_REVENUE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EXPENSE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_GAIN_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LOSS_ELEMENT );

	element_list =
		ledger_get_element_list(
			application_name,
			filter_element_name_list,
			fund_name,
			as_of_date,
			0 /* not omit_subclassification */ );

	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_REVENUE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_REVENUE_ELEMENT );
	}

	latex_table->row_list =
		build_subclassification_aggregate_PDF_row_list(
			element_list,
			is_statement_of_activities );

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

	latex_tex2pdf(	latex->tex_filename,
			latex->working_directory );

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

	appaserver_library_output_ftp_prompt(
		ftp_output_filename, 
		PROMPT,
		process_name /* target */,
		(char *)0 /* mime_type */ );

} /* income_statement_subclassification_aggregate_PDF() */

void income_statement_subclassification_display_PDF(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			char *document_root_directory,
			boolean is_statement_of_activities,
			char *process_name,
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
			logo_filename );

	latex_table =
		latex_new_latex_table(
			sub_title /* caption */ );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list =
		build_subclassification_display_PDF_heading_list();

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				LEDGER_REVENUE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EXPENSE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_GAIN_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LOSS_ELEMENT );

	element_list =
		ledger_get_element_list(
			application_name,
			filter_element_name_list,
			fund_name,
			as_of_date,
			0 /* not omit_subclassification */ );

	latex_table->row_list =
		build_subclassification_display_PDF_row_list(
			element_list,
			is_statement_of_activities );

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

	latex_tex2pdf(	latex->tex_filename,
			latex->working_directory );

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

	appaserver_library_output_ftp_prompt(
		ftp_output_filename, 
		PROMPT,
		process_name /* target */,
		(char *)0 /* mime_type */ );

} /* income_statement_subclassification_display_PDF() */

LIST *build_subclassification_aggregate_PDF_row_list(
					LIST *element_list,
					boolean is_statement_of_activities )
{
	LEDGER_ELEMENT *element;
	double total_revenues = 0.0;
	double total_expenses = 0.0;
	double total_gains = 0.0;
	double total_losses = 0.0;
	double net_income = 0.0;
	LIST *row_list;
	LATEX_ROW *latex_row;

	row_list = list_new();

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_REVENUE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_REVENUE_ELEMENT );
	}

	list_append_list(
		row_list,
		ledger_get_subclassification_aggregate_latex_row_list(
			&total_revenues,
			element->subclassification_list,
			element->element_name,
			0.0 /* percent_denominator */ ) );

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_EXPENSE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EXPENSE_ELEMENT );
	}

	list_append_list(
		row_list,
		ledger_get_subclassification_aggregate_latex_row_list(
			&total_expenses,
			element->subclassification_list,
			element->element_name,
			0.0 /* percent_denominator */ ) );

	/* Compute total gains */
	/* ------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_GAIN_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_GAIN_ELEMENT );
	}

	list_append_list(
		row_list,
		ledger_get_subclassification_aggregate_latex_row_list(
			&total_gains,
			element->subclassification_list,
			element->element_name,
			0.0 /* percent_denominator */ ) );

	/* Compute total losses */
	/* -------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_LOSS_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LOSS_ELEMENT );
	}

	list_append_list(
		row_list,
		ledger_get_subclassification_aggregate_latex_row_list(
			&total_losses,
			element->subclassification_list,
			element->element_name,
			0.0 /* percent_denominator */ ) );

	net_income = ledger_get_net_income(
				total_revenues,
				total_expenses,
				total_gains,
				total_losses );

	/* Blank line */
	/* ---------- */
	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( "" ),
		0 /* not large_bold */ );

	list_append_pointer(
		row_list,
		ledger_get_subclassification_latex_net_income_row(
			net_income,
			is_statement_of_activities,
			total_revenues
				/* percent_denominator */ ) );

	return row_list;

} /* build_subclassification_aggregate_PDF_row_list() */

LIST *build_subclassification_display_PDF_row_list(
				LIST *element_list,
				boolean is_statement_of_activities )
{
	LEDGER_ELEMENT *element;
	double total_revenues = 0.0;
	double total_expenses = 0.0;
	double total_gains = 0.0;
	double total_losses = 0.0;
	double net_income = 0.0;
	LIST *row_list;
	LATEX_ROW *latex_row;

	row_list = list_new();

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_REVENUE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_REVENUE_ELEMENT );
	}

	list_append_list(
		row_list,
		ledger_get_subclassification_display_latex_row_list(
			&total_revenues,
			element->subclassification_list,
			element->element_name,
			element->accumulate_debit,
			element->element_total /* percent_denominator */ ) );

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_EXPENSE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EXPENSE_ELEMENT );
	}

	list_append_list(
		row_list,
		ledger_get_subclassification_display_latex_row_list(
			&total_expenses,
			element->subclassification_list,
			element->element_name,
			element->accumulate_debit,
			total_revenues /* percent_denominator */ ) );

	/* Compute total gains */
	/* ------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_GAIN_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_GAIN_ELEMENT );
	}

	list_append_list(
		row_list,
		ledger_get_subclassification_display_latex_row_list(
			&total_gains,
			element->subclassification_list,
			element->element_name,
			element->accumulate_debit,
			total_revenues /* percent_denominator */ ) );

	/* Compute total losses */
	/* -------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_LOSS_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LOSS_ELEMENT );
	}

	list_append_list(
		row_list,
		ledger_get_subclassification_display_latex_row_list(
			&total_losses,
			element->subclassification_list,
			element->element_name,
			element->accumulate_debit,
			total_revenues /* percent_denominator */ ) );

	net_income = ledger_get_net_income(
				total_revenues,
				total_expenses,
				total_gains,
				total_losses );

	/* Blank line */
	/* ---------- */
	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( "" ),
		0 /* not large_bold */ );

	list_append_pointer(	row_list,
				ledger_get_latex_net_income_row(
					net_income,
					is_statement_of_activities,
					total_revenues
						/* percent_denominator */,
					0 /* not omit_subclassification */ ) );

	return row_list;

} /* build_subclassification_display_PDF_row_list() */

#ifdef NOT_DEFINED
void output_earnings_per_share(	HTML_TABLE *html_table,
				double net_income,
				double shares_outstanding )
{
	char buffer[ 128 ];

	html_table_set_data( html_table->data_list,
			     "<h2>Earnings Per Share</h2>" );

	html_table_set_data(
			html_table->data_list,
			"Net Income" );

	html_table_set_data(
			html_table->data_list,
			"Shares Outstanding" );

	html_table_set_data(
			html_table->data_list,
			"Earnings Per Share" );

	html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );
	html_table->data_list = list_new();

	html_table_set_data( html_table->data_list, "" );

	sprintf(	buffer, 
			"%.2lf",
			net_income );

	html_table_set_data(	html_table->data_list,
				strdup( buffer ) );

	sprintf(	buffer, 
			"%.2lf",
			shares_outstanding );

	html_table_set_data(	html_table->data_list,
				strdup( buffer ) );

	if ( shares_outstanding )
	{
		sprintf(	buffer, 
				"%.2lf",
				net_income / shares_outstanding );
	}
	else
	{
		strcpy( buffer, "0.0" );
	}

	html_table_set_data(	html_table->data_list,
				strdup( buffer ) );

	html_table_output_data(
		html_table->data_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

} /* output_earnings_per_share() */
#endif

#ifdef NOT_DEFINED
double get_shares_outstanding(	char *appaserver_mount_point,
				char *application_name,
				char *as_of_date )
{
	char sys_string[ 1024 ];
	char *results_string;

	sprintf(sys_string,
		"%s/%s/shares_outstanding \"%s\" \"%s\" 2>>%s",
		appaserver_mount_point,
		application_get_relative_source_directory(
			application_name ),
		application_name,
		as_of_date,
		appaserver_error_get_filename(
			application_name ) );

	results_string = pipe2string( sys_string );
	if ( !results_string )
		return 0.0;
	else
		return atof( results_string );
} /* get_shares_outstanding() */
#endif

LIST *build_subclassification_omit_PDF_heading_list( void )
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
	table_heading->heading = "Element";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Percent";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	return heading_list;

} /* build_subclassification_omit_PDF_heading_list() */

LIST *build_subclassification_display_PDF_heading_list( void )
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

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Percent";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	return heading_list;

} /* build_subclassification_display_PDF_heading_list() */

LIST *build_subclassification_aggregate_PDF_heading_list( void )
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

} /* build_subclassification_aggregate_PDF_heading_list() */

void income_statement_subclassification_omit_html_table(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			boolean is_statement_of_activities )
{
	LIST *element_list;
	LEDGER_ELEMENT *element;
	LIST *filter_element_name_list;
	HTML_TABLE *html_table;
	double total_revenues;
	double total_expenses;
	double total_gains;
	double total_losses;
	double net_income;

	html_table = html_table_new( title, sub_title, "" );

	html_table_set_heading( html_table, "" );
	html_table_set_heading( html_table, "Account" );
	html_table_set_heading( html_table, "Element" );
	html_table_set_heading( html_table, "Percent" );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 3;

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		(LIST *)0 /* justify_list */ );

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				LEDGER_REVENUE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EXPENSE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_GAIN_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LOSS_ELEMENT );

	element_list =
		ledger_get_element_list(
			application_name,
			filter_element_name_list,
			fund_name,
			as_of_date,
			1 /* omit_subclassification */ );

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_REVENUE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_REVENUE_ELEMENT );
	}

	total_revenues =
		ledger_output_html_account_list(
			html_table,
			element->account_list,
			LEDGER_REVENUE_ELEMENT,
			element->accumulate_debit,
			element->element_total
				/* percent_denominator */ );

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_EXPENSE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EXPENSE_ELEMENT );
	}

	total_expenses =
		ledger_output_html_account_list(
			html_table,
			element->account_list,
			LEDGER_EXPENSE_ELEMENT,
			element->accumulate_debit,
			total_revenues
				/* percent_denominator */ );

	/* Compute total gains */
	/* ------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_GAIN_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_GAIN_ELEMENT );
	}

	total_gains =
		ledger_output_html_account_list(
			html_table,
			element->account_list,
			LEDGER_GAIN_ELEMENT,
			element->accumulate_debit,
			total_revenues
				/* percent_denominator */ );

	/* Compute total losses */
	/* -------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_LOSS_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LOSS_ELEMENT );
	}

	total_losses =
		ledger_output_html_account_list(
			html_table,
			element->account_list,
			LEDGER_LOSS_ELEMENT,
			element->accumulate_debit,
			total_revenues
				/* percent_denominator */ );

	net_income = ledger_get_net_income(
				total_revenues,
				total_expenses,
				total_gains,
				total_losses );

	ledger_output_net_income(
		html_table,
		net_income,
		is_statement_of_activities,
		total_revenues,
		1 /* skip_columns */ );

/*
		shares_outstanding = get_shares_outstanding(
					appaserver_parameter_file->
						appaserver_mount_point,
					application_name,
					as_of_date );
		if( shares_outstanding )
		{
			output_earnings_per_share(
						html_table,
						net_income,
						shares_outstanding );
		}
*/

	html_table_close();
	document_close();

} /* income_statement_subclassification_omit_html_table() */

void income_statement_subclassification_omit_PDF(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			char *document_root_directory,
			boolean is_statement_of_activities,
			char *process_name,
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
			logo_filename );

	latex_table =
		latex_new_latex_table(
			sub_title /* caption */ );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list =
		build_subclassification_omit_PDF_heading_list();

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				LEDGER_REVENUE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EXPENSE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_GAIN_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LOSS_ELEMENT );

	element_list =
		ledger_get_element_list(
			application_name,
			filter_element_name_list,
			fund_name,
			as_of_date,
			1 /* omit_subclassification */ );

	latex_table->row_list =
		build_subclassification_omit_PDF_row_list(
			element_list,
			is_statement_of_activities );

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

	latex_tex2pdf(	latex->tex_filename,
			latex->working_directory );

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

	appaserver_library_output_ftp_prompt(
		ftp_output_filename, 
		PROMPT,
		process_name /* target */,
		(char *)0 /* mime_type */ );

} /* income_statement_subclassification_omit_PDF() */

LIST *build_subclassification_omit_PDF_row_list(
				LIST *element_list,
				boolean is_statement_of_activities )
{
	LEDGER_ELEMENT *element;
	double total_revenues = 0.0;
	double total_expenses = 0.0;
	double total_gains = 0.0;
	double total_losses = 0.0;
	double net_income = 0.0;
	LIST *row_list;
	LATEX_ROW *latex_row;

	row_list = list_new();

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_REVENUE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_REVENUE_ELEMENT );
	}

	list_append_list(
		row_list,
		ledger_get_subclassification_omit_latex_row_list(
			&total_revenues,
			element->account_list,
			element->element_name,
			element->accumulate_debit,
			element->element_total /* percent_denominator */ ) );

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_EXPENSE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EXPENSE_ELEMENT );
	}

	list_append_list(
		row_list,
		ledger_get_subclassification_omit_latex_row_list(
			&total_expenses,
			element->account_list,
			element->element_name,
			element->accumulate_debit,
			total_revenues /* percent_denominator */ ) );

	/* Compute total gains */
	/* ------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_GAIN_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_GAIN_ELEMENT );
	}

	list_append_list(
		row_list,
		ledger_get_subclassification_omit_latex_row_list(
			&total_gains,
			element->account_list,
			element->element_name,
			element->accumulate_debit,
			total_revenues /* percent_denominator */ ) );

	/* Compute total losses */
	/* -------------------- */ 
	if ( ! ( element =
			ledger_element_list_seek(
				element_list,
				LEDGER_LOSS_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LOSS_ELEMENT );
	}

	list_append_list(
		row_list,
		ledger_get_subclassification_omit_latex_row_list(
			&total_losses,
			element->account_list,
			element->element_name,
			element->accumulate_debit,
			total_revenues /* percent_denominator */ ) );

	net_income = ledger_get_net_income(
				total_revenues,
				total_expenses,
				total_gains,
				total_losses );

	/* Blank line */
	/* ---------- */
	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( "" ),
		0 /* not large_bold */ );

	list_append_pointer(	row_list,
				ledger_get_latex_net_income_row(
					net_income,
					is_statement_of_activities,
					total_revenues
						/* percent_denominator */,
					1 /* omit_subclassification */ ) );

	return row_list;

} /* build_subclassification_omit_PDF_row_list() */

