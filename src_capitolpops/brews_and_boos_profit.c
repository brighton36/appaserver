/* ---------------------------------------------------------------	*/
/* src_accountancymodel/brews_and_boos_profit.c				*/
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
#define FILENAME_STEM			"brews_and_boos_profit"
#define PROMPT				"Press here to view statement."
/*
#define FTP_OUTPUT_FILE_TEMPLATE	"http://%s/%s/brews_and_boos_profit_%d.%s"
#define LATEX_FILE_TEMPLATE		"brews_and_boos_profit_%d.tex"
#define LATEX_DVI_FILE_TEMPLATE		"brews_and_boos_profit_%d.dvi"
*/

/* Prototypes */
/* ---------- */
void get_brews_and_boos_amounts(
			double *brews_and_boos_concert_revenue,
			double *brews_and_boos_food_revenue,
			double *brews_and_boos_donation,
			double *brews_and_boos_deposit,
			double *brews_and_boos_expense,
			char *as_of_date,
			char *application_name );

void brews_and_boos_profit_PDF(
					char *application_name,
					char *title,
					char *sub_title,
					char *as_of_date,
					char *document_root_directory );

void brews_and_boos_profit_html_table(
					char *application_name,
					char *title,
					char *sub_title,
					char *as_of_date );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 256 ];
	char sub_title[ 256 ];
	char *database_string = {0};
	char *output_medium;
	char *as_of_date;

	if ( argc != 4 )
	{
		fprintf( stderr,
"Usage: %s application process output_medium\n",
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
	output_medium = argv[ 3 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

	appaserver_parameter_file = new_appaserver_parameter_file();

	as_of_date = date_get_now_yyyy_mm_dd();
	format_initial_capital( title, process_name );
	sprintf( sub_title, "As of %s", as_of_date );

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

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		brews_and_boos_profit_html_table(
			application_name,
			title,
			sub_title,
			as_of_date );
	}
	else
	{
		brews_and_boos_profit_PDF(
			application_name,
			title,
			sub_title,
			as_of_date,
			appaserver_parameter_file->
				document_root );
	}

	document_close();

	return 0;

} /* main() */

void brews_and_boos_profit_html_table(
				char *application_name,
				char *title,
				char *sub_title,
				char *as_of_date )
{
	HTML_TABLE *html_table;
	double brews_and_boos_concert_revenue = 0.0;
	double brews_and_boos_food_revenue = 0.0;
	double brews_and_boos_donation = 0.0;
	double brews_and_boos_deposit = 0.0;
	double brews_and_boos_expense = 0.0;
	double brews_and_boos_profit = 0.0;
	char number_string[ 16 ];

	html_table = new_html_table(
			title,
			sub_title );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 1;
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	get_brews_and_boos_amounts(
		&brews_and_boos_concert_revenue,
		&brews_and_boos_food_revenue,
		&brews_and_boos_donation,
		&brews_and_boos_deposit,
		&brews_and_boos_expense,
		as_of_date,
		application_name );

	brews_and_boos_profit =
		brews_and_boos_concert_revenue +
		brews_and_boos_food_revenue +
		brews_and_boos_donation -
		brews_and_boos_deposit -
		brews_and_boos_expense;

	/* Concert Revenue */
	/* --------------- */
	html_table_set_data(
		html_table->data_list,
		"+ Concert Revenue" );

	sprintf(number_string,
		"%.2lf", 
		brews_and_boos_concert_revenue );

	html_table_set_data(
		html_table->data_list,
		number_string );

	html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	html_table->data_list = list_new();

	/* Food Revenue */
	/* ------------ */
	html_table_set_data(
		html_table->data_list,
		"+ Food Revenue" );
	
	sprintf(number_string,
		"%.2lf", 
		brews_and_boos_food_revenue );

	html_table_set_data(
		html_table->data_list,
		number_string );

	html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	html_table->data_list = list_new();

	/* Donation Revenue */
	/* ---------------- */
	html_table_set_data(
		html_table->data_list,
		"+ Donation Revenue" );
	
	sprintf(number_string,
		"%.2lf", 
		brews_and_boos_donation );

	html_table_set_data(
		html_table->data_list,
		number_string );

	html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	html_table->data_list = list_new();

	/* Deposit */
	/* ------- */
	if ( brews_and_boos_deposit )
	{
		html_table_set_data(
			html_table->data_list,
			"- Deposit" );
		
		sprintf(number_string,
			"%.2lf", 
			brews_and_boos_deposit );
	
		html_table_set_data(
			html_table->data_list,
			number_string );
	
		html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
	
		html_table->data_list = list_new();
	}

	/* Expense */
	/* ------- */
	html_table_set_data(
		html_table->data_list,
		"- Expenses" );
	
	sprintf(number_string,
		"%.2lf", 
		brews_and_boos_expense );

	html_table_set_data(
		html_table->data_list,
		number_string );

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
	html_table_set_data(
		html_table->data_list,
		"= Profit" );
	
	sprintf(number_string,
		"%.2lf", 
		brews_and_boos_profit );

	html_table_set_data(
		html_table->data_list,
		number_string );

	html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	html_table_close();

} /* brews_and_boos_profit_html_table() */

void brews_and_boos_profit_PDF(	char *application_name,
				char *title,
				char *sub_title,
				char *as_of_date,
				char *document_root_directory )
{
	LATEX *latex;
	LATEX_TABLE *latex_table;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	pid_t pid = getpid();
	LATEX_TABLE_HEADING *table_heading;
	double brews_and_boos_concert_revenue = 0.0;
	double brews_and_boos_food_revenue = 0.0;
	double brews_and_boos_donation = 0.0;
	double brews_and_boos_deposit = 0.0;
	double brews_and_boos_expense = 0.0;
	double brews_and_boos_profit = 0.0;
	char number_string[ 16 ];
	LATEX_ROW *latex_row;
	char caption[ 256 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			FILENAME_STEM,
			application_name,
			pid,
			(char *)0 /* session */,
			(char *)0 /* extension */ );

	working_directory =
		appaserver_link_get_source_directory(
			document_root_directory,
			application_name );

/*
	sprintf(	working_directory,
			"%s/%s",
			appaserver_mount_point,
			application_name );
*/

	appaserver_link_file->extension = "tex";

	latex_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

/*
	sprintf(	latex_filename,
			LATEX_FILE_TEMPLATE,
			pid );

	sprintf(	dvi_filename,
			LATEX_DVI_FILE_TEMPLATE,
			pid );
*/

	appaserver_link_file->extension = "dvi";

	dvi_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

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

	get_brews_and_boos_amounts(
		&brews_and_boos_concert_revenue,
		&brews_and_boos_food_revenue,
		&brews_and_boos_donation,
		&brews_and_boos_deposit,
		&brews_and_boos_expense,
		as_of_date,
		application_name );

	brews_and_boos_profit =
		brews_and_boos_concert_revenue +
		brews_and_boos_food_revenue +
		brews_and_boos_donation -
		brews_and_boos_deposit -
		brews_and_boos_expense;

	latex_table->row_list = list_new();

	/* Concert Revenue */
	/* --------------- */
	latex_row = latex_new_latex_row();
	list_append_pointer( latex_table->row_list, latex_row );

	list_append_pointer(
		latex_row->column_data_list,
		"+ Concert Revenue" );

	sprintf(number_string,
		"%.2lf", 
		brews_and_boos_concert_revenue );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( number_string ) );

	/* Food Revenue */
	/* ------------ */
	latex_row = latex_new_latex_row();
	list_append_pointer( latex_table->row_list, latex_row );

	list_append_pointer(
		latex_row->column_data_list,
		"+ Food Revenue" );

	sprintf(number_string,
		"%.2lf", 
		brews_and_boos_food_revenue );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( number_string ) );

	/* Donation Revenue */
	/* ---------------- */
	latex_row = latex_new_latex_row();
	list_append_pointer( latex_table->row_list, latex_row );

	list_append_pointer(
		latex_row->column_data_list,
		"+ Donation Revenue" );

	sprintf(number_string,
		"%.2lf", 
		brews_and_boos_donation );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( number_string ) );

	/* Deposit */
	/* ------- */
	if ( brews_and_boos_deposit )
	{
		latex_row = latex_new_latex_row();
		list_append_pointer( latex_table->row_list, latex_row );
	
		list_append_pointer(
			latex_row->column_data_list,
			"- Deposit" );
	
		sprintf(number_string,
			"%.2lf", 
			brews_and_boos_deposit );
	
		list_append_pointer(
			latex_row->column_data_list,
			strdup( number_string ) );
	}

	/* Expenses */
	/* -------- */
	latex_row = latex_new_latex_row();
	list_append_pointer( latex_table->row_list, latex_row );

	list_append_pointer(
		latex_row->column_data_list,
		"- Expenses" );

	sprintf(number_string,
		"%.2lf", 
		brews_and_boos_expense );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( number_string ) );

	/* Profit */
	/* ------ */
	latex_row = latex_new_latex_row();
	list_append_pointer( latex_table->row_list, latex_row );

	list_append_pointer(
		latex_row->column_data_list,
		"= Profit" );

	sprintf(number_string,
		"%.2lf", 
		brews_and_boos_profit );

	list_append_pointer(
		latex_row->column_data_list,
		strdup( number_string ) );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

	/* Output */
	/* ------ */
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

	latex_tex2pdf(	latex->tex_filename,
			latex->working_directory );

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
		"trial_balance" /* target */,
		(char *)0 /* mime_type */ );

} /* brews_and_boos_profit_PDF() */

void get_brews_and_boos_amounts(
		double *brews_and_boos_concert_revenue,
		double *brews_and_boos_food_revenue,
		double *brews_and_boos_donation,
		double *brews_and_boos_deposit,
		double *brews_and_boos_expense,
		char *as_of_date,
		char *application_name )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( ( journal_ledger =
		ledger_get_latest_ledger(
			application_name,
			"brews_and_boos_concert_revenue",
			as_of_date ) ) )
	{
		*brews_and_boos_concert_revenue = journal_ledger->balance;
	}

	if ( ( journal_ledger =
		ledger_get_latest_ledger(
			application_name,
			"brews_and_boos_food_revenue",
			as_of_date ) ) )
	{
		*brews_and_boos_food_revenue = journal_ledger->balance;
	}

	if ( ( journal_ledger =
		ledger_get_latest_ledger(
			application_name,
			"brews_and_boos_donation",
			as_of_date ) ) )
	{
		*brews_and_boos_donation = journal_ledger->balance;
	}

	if ( ( journal_ledger =
		ledger_get_latest_ledger(
			application_name,
			"brews_and_boos_deposit",
			as_of_date ) ) )
	{
		*brews_and_boos_deposit = journal_ledger->balance;
	}

	if ( ( journal_ledger =
		ledger_get_latest_ledger(
			application_name,
			"brews_and_boos_expense",
			as_of_date ) ) )
	{
		*brews_and_boos_expense = journal_ledger->balance;
	}

} /* get_brews_and_boos_amounts() */

