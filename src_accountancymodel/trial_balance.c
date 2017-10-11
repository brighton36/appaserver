/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/trial_balance.c		*/
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
#define DAYS_FOR_EMPHASIS		35
#define ROWS_BETWEEN_HEADING		10
#define PROMPT				"Press here to view statement."

/* Prototypes */
/* ---------- */
char *get_action_string(
					char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					char *beginning_date,
					char *as_of_date,
					char *account_name );

void output_stdout(			char *element_name,
					char *subclassification_name,
					char *account_name,
					char *full_name,
					int transaction_count,
					double balance,
					char *transaction_date_time,
					char *memo,
					boolean accumulate_debit,
					double debit_amount,
					double credit_amount,
					double prior_balance_change,
					double subclassification_total );

void trial_balance_stdout(
					char *application_name,
					char *fund_name,
					char *as_of_date );

void build_PDF_account_row(		LIST *column_data_list,
					boolean *accumulate_debit,
					double *balance,
					char *application_name,
					ACCOUNT *account,
					LIST *prior_element_list,
					double subclassification_total );

double trial_balance_get_prior_balance_change(
					LIST *prior_element_list,
					char *account_name,
					double current_balance );

void trial_balance_account_html_table(
					double *balance,
					boolean *accumulate_debit,
					HTML_TABLE *html_table,
					char *application_name,
					ACCOUNT *account,
					LIST *prior_element_list,
					char *element_name,
					char *subclassification_name,
					double subclassification_total,
					char *beginning_date,
					char *as_of_date,
					char *session,
					char *login_name,
					char *role_name );

void trial_balance_account_stdout(
					double *balance,
					boolean *accumulate_debit,
					char *application_name,
					ACCOUNT *account,
					LIST *prior_element_list,
					char *element_name,
					char *subclassification_name,
					double subclassification_total );

char *get_html_table_account_title(	char *account_name,
					char *full_name,
					double debit_amount,
					double credit_amount,
					char *transaction_date,
					char *memo );

char *get_latex_account_title(		char *account_name,
					char *full_name,
					double debit_amount,
					double credit_amount,
					char *transaction_date,
					char *memo );

void trial_balance_html_table(
					char *application_name,
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					char *session,
					char *login_name,
					char *role_name );

LIST *build_PDF_row_list(		char *application_name,
					LIST *current_element_list,
					LIST *prior_element_list );

LIST *build_PDF_heading_list(		void );

void trial_balance_PDF(			char *application_name,
					char *fund_name,
					char *as_of_date,
					char *document_root_directory,
					char *process_name,
					char *aggregation,
					char *logo_filename );

void trial_balance_PDF_fund(		
					LATEX *latex,
					char *application_name,
					char *sub_title,
					char *fund_name,
					char *as_of_date );

void output_html_table(			LIST *data_list,
					char *element_name,
					char *subclassification_name,
					char *account_name,
					char *full_name,
					int transaction_count,
					double balance,
					char *transaction_date_time,
					char *memo,
					int number_left_justified_columns,
					int number_right_justified_columns,
					boolean background_shaded,
					LIST *justify_list,
					boolean accumulate_debit,
					double debit_amount,
					double credit_amount,
					double prior_balance_change,
					double subclassification_total,
					char *application_name,
					char *beginning_date,
					char *as_of_date,
					char *session,
					char *login_name,
					char *role_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 256 ];
	char sub_title[ 256 ];
	char *fund_name;
	char *aggregation;
	char *as_of_date;
	char *database_string = {0};
	char *output_medium;
	char *logo_filename;

	if ( argc != 10 )
	{
		fprintf( stderr,
"Usage: %s application session login_name role process fund as_of_date aggregation output_medium\n",
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

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	process_name = argv[ 5 ];
	fund_name = argv[ 6 ];
	as_of_date = argv[ 7 ];
	aggregation = argv[ 8 ];
	output_medium = argv[ 9 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !*as_of_date
	||   strcmp(	as_of_date,
			"as_of_date" ) == 0 )
	{
		as_of_date = date_get_now_yyyy_mm_dd( date_get_utc_offset() );
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document = document_new(
				(char *)0 /* title */,
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
		trial_balance_html_table(
			application_name,
			title,
			sub_title,
			fund_name,
			as_of_date,
			session,
			login_name,
			role_name );
	}
	else
	if ( strcmp( output_medium, "PDF" ) == 0 )
	{
		trial_balance_PDF(
			application_name,
			fund_name,
			as_of_date,
			appaserver_parameter_file->
				document_root,
			process_name,
			aggregation,
			logo_filename );
	}
	else
	{
		trial_balance_stdout(
			application_name,
			fund_name,
			as_of_date );
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
		document_close();

	exit( 0 );

} /* main() */

void trial_balance_html_table(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			char *session,
			char *login_name,
			char *role_name )
{
	HTML_TABLE *html_table;
	LIST *heading_list;
	char *debit_string;
	char *credit_string;
	double debit_sum = 0.0;
	double credit_sum = 0.0;
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	boolean accumulate_debit;
	double balance;
	LIST *current_element_list;
	LIST *prior_element_list;
	LIST *prior_filter_element_name_list;
	DATE *prior_closing_transaction_date;
	char *prior_closing_transaction_date_string = {0};
	double subclassification_total;
	int count = 0;
	char *element_name = {0};
	char *beginning_date;

	if ( ! ( beginning_date = 
			ledger_beginning_transaction_date(
				application_name,
				fund_name,
				as_of_date ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get beginning_date.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}


	/* Populate the current_element_list */
	/* --------------------------------- */
	current_element_list =
		ledger_get_element_list(
			application_name,
			(LIST *)0 /* filter_element_name_list */,
			fund_name,
			as_of_date );

	/* Populate the prior_element_list */
	/* ------------------------------- */
	prior_closing_transaction_date =
		ledger_prior_closing_transaction_date(
			application_name,
			fund_name,
			as_of_date /* ending_transaction_date */ );

	if ( prior_closing_transaction_date )
	{
		prior_closing_transaction_date_string =
			date_get_yyyy_mm_dd_string(
				prior_closing_transaction_date );
	}
	else
	{
		prior_closing_transaction_date_string =
			ledger_beginning_transaction_date(
				application_name,
				fund_name,
				as_of_date );
	}

	prior_filter_element_name_list = list_new();

	list_append_pointer(
		prior_filter_element_name_list, 
		LEDGER_ASSET_ELEMENT );

	list_append_pointer(
		prior_filter_element_name_list, 
		LEDGER_LIABILITY_ELEMENT );

	prior_element_list =
		ledger_get_element_list(
			application_name,
			prior_filter_element_name_list,
			fund_name,
			prior_closing_transaction_date_string );

	/* Create the table heading */
	/* ------------------------ */
	heading_list = list_new();
	list_append_string( heading_list, "Element" );
	list_append_string( heading_list, "Subclassification" );
	list_append_string( heading_list, "Account" );
	list_append_string( heading_list, "Count" );
	list_append_string( heading_list, "Debit" );
	list_append_string( heading_list, "Credit" );
	list_append_string( heading_list, "change_or_percent" );
	
	html_table = new_html_table(
			title,
			sub_title );

	html_table->number_left_justified_columns = 3;
	html_table->number_right_justified_columns = 4;
	html_table_set_heading_list( html_table, heading_list );

	html_table_output_table_heading(

					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->justify_list );

	if ( !list_rewind( current_element_list ) )
	{
		printf(
	"<h3>ERROR: there are no elements for this statement.</h3>\n" );
		document_close();
		exit( 1 );
	}

	do {
		element = list_get_pointer( current_element_list );

		if ( !list_rewind( element->subclassification_list ) )
			continue;

		element_name = element->element_name;

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

				if ( ledger_is_period_element(
					element->element_name ) )
				{
					subclassification_total =
						subclassification->
							subclassification_total;
				}
				else
				{
					subclassification_total = 0.0;
				}

				trial_balance_account_html_table(
					&balance,
					&accumulate_debit,
					html_table,
					application_name,
					account,
					prior_element_list,
					element_name,
					subclassification->
						subclassification_name,
					subclassification_total,
					beginning_date,
					as_of_date,
					session,
					login_name,
					role_name );

				list_free( html_table->data_list );
				html_table->data_list = list_new();

				if ( accumulate_debit )
				{
					debit_sum += balance;
				}
				else
				{
					credit_sum += balance;
				}

				subclassification->
					subclassification_name =
						(char *)0;

				element_name = (char *)0;

			} while( list_next( subclassification->account_list ) );

		} while( list_next( element->subclassification_list ) );

	} while( list_next( current_element_list ) );

	html_table_set_data( html_table->data_list, "Total" );
	html_table_set_data( html_table->data_list, "" );
	html_table_set_data( html_table->data_list, "" );
	html_table_set_data( html_table->data_list, "" );

	debit_string = timlib_place_commas_in_money( debit_sum );
	html_table_set_data( html_table->data_list, strdup( debit_string ) );

	credit_string = timlib_place_commas_in_money( credit_sum );
	html_table_set_data( html_table->data_list, strdup( credit_string ) );

	html_table_output_data(
		html_table->data_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	html_table_close();

} /* trial_balance_html_table() */

void trial_balance_account_html_table(
					double *balance,
					boolean *accumulate_debit,
					HTML_TABLE *html_table,
					char *application_name,
					ACCOUNT *account,
					LIST *prior_element_list,
					char *element_name,
					char *subclassification_name,
					double subclassification_total,
					char *beginning_date,
					char *as_of_date,
					char *session,
					char *login_name,
					char *role_name )
{
	double prior_balance_change;

	*accumulate_debit =
		ledger_account_get_accumulate_debit(
			application_name,
			account->account_name );

	*balance = account->latest_ledger->balance;

	prior_balance_change =
		trial_balance_get_prior_balance_change(
			prior_element_list,
			account->account_name,
			*balance /* current_balance */ );

	/* See if negative balance. */
	/* ------------------------ */
	if ( *balance < 0.0 )
	{
		*balance = float_abs( *balance );
		*accumulate_debit = 1 - *accumulate_debit;
	}

	output_html_table(
		html_table->data_list,
		element_name,
		subclassification_name,
		account->account_name,
		account->
			latest_ledger->
			full_name,
		account->
			latest_ledger->
			transaction_count,
		*balance,
		account->
			latest_ledger->
			transaction_date_time,
		account->
			latest_ledger->
			memo,
		html_table->
		 number_left_justified_columns,
		html_table->
		 number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list,
		*accumulate_debit,
		account->
			latest_ledger->
			debit_amount,
		account->
			latest_ledger->
			credit_amount,
		prior_balance_change,
		subclassification_total,
		application_name,
		beginning_date,
		as_of_date,
		session,
		login_name,
		role_name );

} /* trial_balance_account_html_table() */

void trial_balance_PDF(
			char *application_name,
			char *fund_name,
			char *as_of_date,
			char *document_root_directory,
			char *process_name,
			char *aggregation,
			char *logo_filename )
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
		list_length( fund_name_list ),
		logo_filename );

	printf( "<h1>%s</h1>\n", title );

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			logo_filename );

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
				0 /* fund_name_list_length */,
				logo_filename );

			trial_balance_PDF_fund(
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
		trial_balance_PDF_fund(
				latex,
				application_name,
				sub_title,
				fund_name,
				as_of_date );
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

} /* trial_balance_PDF() */

void trial_balance_PDF_fund(
			LATEX *latex,
			char *application_name,
			char *sub_title,
			char *fund_name,
			char *as_of_date )
{
	LATEX_TABLE *latex_table;
	LIST *current_element_list;
	LIST *prior_element_list;
	LIST *prior_filter_element_name_list;
	DATE *prior_closing_transaction_date;
	char *prior_closing_transaction_date_string = {0};

	printf( "<h2>%s</h2>\n", sub_title );

	latex_table =
		latex_new_latex_table(
			strdup( sub_title ) /* caption */ );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list = build_PDF_heading_list();

	/* Populate the current_element_list */
	/* --------------------------------- */
	current_element_list =
		ledger_get_element_list(
			application_name,
			(LIST *)0 /* filter_element_name_list */,
			fund_name,
			as_of_date );

	/* Populate the prior_element_list */
	/* ------------------------------- */
	prior_closing_transaction_date =
		ledger_prior_closing_transaction_date(
			application_name,
			fund_name,
			as_of_date /* ending_transaction_date */ );

	if ( prior_closing_transaction_date )
	{
		prior_closing_transaction_date_string =
			date_get_yyyy_mm_dd_string(
				prior_closing_transaction_date );
	}
	else
	{
		prior_closing_transaction_date_string =
			ledger_beginning_transaction_date(
				application_name,
				fund_name,
				as_of_date );
	}

	prior_filter_element_name_list = list_new();

	list_append_pointer(
		prior_filter_element_name_list, 
		LEDGER_ASSET_ELEMENT );

	list_append_pointer(
		prior_filter_element_name_list, 
		LEDGER_LIABILITY_ELEMENT );

	prior_element_list =
		ledger_get_element_list(
			application_name,
			prior_filter_element_name_list,
			fund_name,
			prior_closing_transaction_date_string );

	latex_table->row_list =
		build_PDF_row_list(
			application_name,
			current_element_list,
			prior_element_list );

} /* trial_balance_PDF_fund() */

LIST *build_PDF_row_list(	char *application_name,
				LIST *current_element_list,
				LIST *prior_element_list )
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
	double balance;
	double subclassification_total;
	char *element_name;

	if ( !list_rewind( current_element_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		element = list_get_pointer( current_element_list );

		if ( !list_rewind( element->subclassification_list ) )
			continue;

		element_name = element->element_name;

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

				if ( ledger_is_period_element(
					element->element_name ) )
				{
					subclassification_total =
						subclassification->
							subclassification_total;
				}
				else
				{
					subclassification_total = 0.0;
				}

				latex_row = latex_new_latex_row();
				list_append_pointer( row_list, latex_row );

				if ( element_name )
				{
					format_initial_capital(
						element_title,
						element_name );

					element_name = (char *)0;

				}
				else
				{
					*element_title = '\0';
				}

/*
				list_append_pointer(
					latex_row->column_data_list,
					strdup( element_title ) );
*/

				latex_append_column_data_list(
					latex_row->column_data_list,
					strdup( element_title ),
					0 /* not large_bold */ );

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

/*
				list_append_pointer(
					latex_row->column_data_list,
					strdup( subclassification_title ) );
*/

				latex_append_column_data_list(
					latex_row->column_data_list,
					strdup( subclassification_title ),
					0 /* not large_bold */ );

				build_PDF_account_row(
					latex_row->column_data_list,
					&accumulate_debit,
					&balance,
					application_name,
					account,
					prior_element_list,
					subclassification_total );

				if ( accumulate_debit )
					debit_sum += balance;
				else
					credit_sum += balance;

				subclassification->
					subclassification_name =
						(char *)0;

			} while( list_next( subclassification->account_list ) );

		} while( list_next( element->subclassification_list ) );

	} while( list_next( current_element_list ) );

	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

/*
	list_append_pointer( latex_row->column_data_list, "Total" );
	list_append_pointer( latex_row->column_data_list, (char *)0 );
	list_append_pointer( latex_row->column_data_list, (char *)0 );
	list_append_pointer( latex_row->column_data_list, (char *)0 );
*/

	latex_append_column_data_list(
		latex_row->column_data_list,
		"Total",
		0 /* not large_bold */ );

	latex_append_column_data_list(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_bold */ );

	latex_append_column_data_list(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_bold */ );

	latex_append_column_data_list(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_bold */ );

	debit_amount = timlib_place_commas_in_dollars( debit_sum );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( debit_amount ),
		0 /* not large_bold */ );

	credit_amount = timlib_place_commas_in_dollars( credit_sum );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( credit_amount ),
		0 /* not large_bold */ );

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
	table_heading->right_justified_flag = 0;
	table_heading->heading = "Subclassification";
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Account";
	table_heading->paragraph_size = "6.0cm";
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Count";
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

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "$\\Delta$ or \\%";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	return heading_list;

} /* build_PDF_heading_list() */

void output_html_table(	LIST *data_list,
			char *element_name,
			char *subclassification_name,
			char *account_name,
			char *full_name,
			int transaction_count,
			double balance,
			char *transaction_date_time,
			char *memo,
			int number_left_justified_columns,
			int number_right_justified_columns,
			boolean background_shaded,
			LIST *justify_list,
			boolean accumulate_debit,
			double debit_amount,
			double credit_amount,
			double prior_balance_change,
			double subclassification_total,
			char *application_name,
			char *beginning_date,
			char *as_of_date,
			char *session,
			char *login_name,
			char *role_name )
{
	char element_title[ 128 ];
	char subclassification_title[ 128 ];
	char *account_title;
	char transaction_count_string[ 16 ];
	char debit_string[ 4096 ];
	char credit_string[ 4096 ];
	char *prior_balance_change_string;
	char subclassification_total_ratio_string[ 16 ];
	char transaction_date_string[ 16 ];
	char *action_string;

	action_string =
		get_action_string(
			application_name,
			session,
			login_name,
			role_name,
			beginning_date,
			as_of_date,
			account_name );

	if ( element_name && *element_name )
	{
		format_initial_capital(
			element_title,
			element_name );

		html_table_set_data(
			data_list,
			strdup( element_title ) );
	}
	else
		html_table_set_data( data_list, strdup( "" ) );

	if ( subclassification_name && *subclassification_name )
	{
		format_initial_capital(
			subclassification_title,
			subclassification_name );

		html_table_set_data(
			data_list,
			strdup( subclassification_title )  );
	}
	else
		html_table_set_data( data_list, strdup( "" ) );

	account_title =
		get_html_table_account_title(
			account_name,
			full_name,
			debit_amount,
			credit_amount,
			column( transaction_date_string,
				0,
				transaction_date_time ),
			memo );

	html_table_set_data(
		data_list,
		strdup( account_title ) );

	sprintf( transaction_count_string, "%d", transaction_count );

	html_table_set_data(	data_list,
				strdup( transaction_count_string ) );

	/* Set the debit account. */
	/* ---------------------- */
	if ( accumulate_debit )
	{
		sprintf( debit_string,
			 "<a href=\"%s\">%s</a>",
			 action_string,
			 timlib_place_commas_in_money( balance ) );
	}
	else
	{
		*debit_string = '\0';
	}

	html_table_set_data(	data_list,
				strdup( debit_string ) );

	/* Set the credit account. */
	/* ----------------------- */
	if ( !accumulate_debit )
	{
		sprintf( credit_string,
			 "<a href=\"%s\">%s</a>",
			 action_string,
			 timlib_place_commas_in_money( balance ) );
	}
	else
	{
		*credit_string = '\0';
	}

	html_table_set_data(	data_list,
				strdup( credit_string ) );

	/* Set prior_balance_change (maybe) */
	/* -------------------------------- */
	if ( !timlib_dollar_virtually_same(
			prior_balance_change,
			0.0 ) )
	{
		char buffer[ 32 ];

		prior_balance_change_string =
			timlib_place_commas_in_money(
				prior_balance_change );

		sprintf( buffer, "&Delta;%s", prior_balance_change_string );

		html_table_set_data(
			data_list,
			strdup( buffer ) );
	}
	else
	/* Set subclassification_total ratio (maybe) */
	/* ----------------------------------------- */
	if ( !timlib_dollar_virtually_same(
			subclassification_total,
			0.0 ) )
	{
		sprintf( subclassification_total_ratio_string,
			 "%.0lf%c",
			 (balance / subclassification_total) * 100.0,
			 '%' );

		html_table_set_data(
			data_list,
			strdup( subclassification_total_ratio_string ) );
	}

	/* Output the row */
	/* -------------- */
	html_table_output_data(
		data_list,
		number_left_justified_columns,
		number_right_justified_columns,
		background_shaded,
		justify_list );

} /* output_html_table() */

char *get_latex_account_title(	char *account_name,
				char *full_name,
				double debit_amount,
				double credit_amount,
				char *transaction_date,
				char *memo )
{
	static char account_title[ 1024 ];
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
			timlib_place_commas_in_money(
				debit_amount );
	}
	else
	if ( credit_amount )
	{
		transaction_amount_string =
			timlib_place_commas_in_money(
				credit_amount );
	}
	else
	{
		transaction_amount_string = "Unknown";
	}

	ptr += sprintf( ptr, " \\$%s})", transaction_amount_string );

	return account_title;

} /* get_latex_account_title() */

char *get_html_table_account_title(
			char *account_name,
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

	if ( !full_name || !*full_name )
		return "Can't get full name";

	*ptr = '\0';

	format_initial_capital( account_name_formatted, account_name );
	ptr += sprintf( ptr, "%s", account_name_formatted );

	ptr += sprintf( ptr, " <br>(%s:", transaction_date );

	format_initial_capital( full_name_formatted, full_name );
	ptr += sprintf( ptr, " %s", full_name_formatted );

	if ( memo && *memo )
	{
		ptr += sprintf( ptr, "; %s", memo );
	}

	if ( debit_amount )
	{
		transaction_amount_string =
			timlib_place_commas_in_money(
				debit_amount );
	}
	else
	if ( credit_amount )
	{
		transaction_amount_string =
			timlib_place_commas_in_money(
				credit_amount );
	}
	else
	{
		transaction_amount_string = "Unknown";
	}

	ptr += sprintf( ptr, " $%s)", transaction_amount_string );

	return account_title;

} /* get_html_table_account_title() */

double trial_balance_get_prior_balance_change(
			LIST *prior_element_list,
			char *account_name,
			double current_balance )
{
	ACCOUNT *account;
	double balance;
	double balance_change;

	if ( ( account =
		ledger_element_list_account_seek(
			prior_element_list,
			account_name ) ) )
	{
		balance = account->latest_ledger->balance;
		balance_change = current_balance - balance;

		if ( balance < 0.0 && balance_change < 0.0 )
			return 0.0 - balance_change;
		else
			return balance_change;
	}
	else
	{
		return 0.0;
	}

} /* trial_balance_get_prior_balance_change() */

void build_PDF_account_row(	LIST *column_data_list,
				boolean *accumulate_debit,
				double *balance,
				char *application_name,
				ACCOUNT *account,
				LIST *prior_element_list,
				double subclassification_total )
{
	char transaction_date_american[ 16 ];
	char transaction_date_string[ 16 ];
	char *account_title;
	char transaction_count_string[ 32 ];
	char *debit_string;
	char *credit_string;
	char *prior_balance_change_string;
	double prior_balance_change;
	char subclassification_total_ratio_string[ 16 ];
	char *today_date_string;
	int days_between;

	today_date_string = date_get_now_yyyy_mm_dd( date_get_utc_offset() );

	*accumulate_debit =
		ledger_account_get_accumulate_debit(
			application_name,
			account->account_name );

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

	latex_append_column_data_list(
		column_data_list,
		strdup( account_title ),
		0 /* not large_bold */ );

	sprintf( transaction_count_string,
		 "%d",
		 account->
			latest_ledger->
			transaction_count );

	latex_append_column_data_list(
		column_data_list,
		strdup( transaction_count_string ),
		0 /* not large_bold */ );

	*balance = account->latest_ledger->balance;

	prior_balance_change =
		trial_balance_get_prior_balance_change(
			prior_element_list,
			account->account_name,
			*balance /* current_balance */ );

	/* See if negative balance. */
	/* ------------------------ */
	if ( *balance < 0.0 )
	{
		*balance = float_abs( *balance );
		*accumulate_debit = 1 - *accumulate_debit;
	}

	days_between =
		date_days_between(
			transaction_date_string,
			today_date_string,
			date_get_utc_offset() );

	if ( *accumulate_debit )
		debit_string = timlib_place_commas_in_dollars( *balance );
	else
		debit_string = "";

	latex_append_column_data_list(
		column_data_list,
		strdup( debit_string ),
		(days_between <= DAYS_FOR_EMPHASIS) /* large_bold */ );

	if ( !*accumulate_debit )
		credit_string = timlib_place_commas_in_dollars( *balance );
	else
		credit_string = "";

	latex_append_column_data_list(
		column_data_list,
		strdup( credit_string ),
		(days_between <= DAYS_FOR_EMPHASIS) /* large_bold */ );

	/* Set prior_balance_change (maybe) */
	/* -------------------------------- */
	if ( !timlib_dollar_virtually_same(
		prior_balance_change,
		0.0 ) )
	{
		char buffer[ 32 ];

		prior_balance_change_string =
			timlib_place_commas_in_dollars(
				prior_balance_change );

		sprintf( buffer, "$\\Delta$%s", prior_balance_change_string );

		latex_append_column_data_list(
			column_data_list,
			strdup( buffer ),
			0 /* not large_bold */ );
	}

	/* Set subclassification_total ratio (maybe) */
	/* ----------------------------------------- */
	if ( !timlib_dollar_virtually_same(
			subclassification_total,
			0.0 ) )
	{
		sprintf( subclassification_total_ratio_string,
			 "%.0lf%c",
			 (*balance / subclassification_total) * 100.0,
			 '%' );

		latex_append_column_data_list(
			column_data_list,
			strdup( subclassification_total_ratio_string ),
			0 /* not large_bold */ );
	}

} /* build_PDF_account_row() */

void trial_balance_stdout(
			char *application_name,
			char *fund_name,
			char *as_of_date )
{
	LIST *heading_list;
	char *debit_string;
	char *credit_string;
	double debit_sum = 0.0;
	double credit_sum = 0.0;
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	boolean accumulate_debit;
	double balance;
	LIST *current_element_list;
	LIST *prior_element_list;
	LIST *prior_filter_element_name_list;
	DATE *prior_closing_transaction_date;
	char *prior_closing_transaction_date_string = {0};
	double subclassification_total;
	char *element_name = {0};
	LIST *data_list = list_new();

	/* Populate the current_element_list */
	/* --------------------------------- */
	current_element_list =
		ledger_get_element_list(
			application_name,
			(LIST *)0 /* filter_element_name_list */,
			fund_name,
			as_of_date );

	/* Populate the prior_element_list */
	/* ------------------------------- */
	prior_closing_transaction_date =
		ledger_prior_closing_transaction_date(
			application_name,
			fund_name,
			as_of_date /* ending_transaction_date */ );

	if ( prior_closing_transaction_date )
	{
		prior_closing_transaction_date_string =
			date_get_yyyy_mm_dd_string(
				prior_closing_transaction_date );
	}
	else
	{
		prior_closing_transaction_date_string =
			ledger_beginning_transaction_date(
				application_name,
				fund_name,
				as_of_date );
	}

	prior_filter_element_name_list = list_new();

	list_append_pointer(
		prior_filter_element_name_list, 
		LEDGER_ASSET_ELEMENT );

	list_append_pointer(
		prior_filter_element_name_list, 
		LEDGER_LIABILITY_ELEMENT );

	prior_element_list =
		ledger_get_element_list(
			application_name,
			prior_filter_element_name_list,
			fund_name,
			prior_closing_transaction_date_string );

	/* Create the table heading */
	/* ------------------------ */
	heading_list = list_new();
	list_append_string( heading_list, "Element" );
	list_append_string( heading_list, "Subclassification" );
	list_append_string( heading_list, "Account" );
	list_append_string( heading_list, "Count" );
	list_append_string( heading_list, "Debit" );
	list_append_string( heading_list, "Credit" );
	list_append_string( heading_list, "change_or_percent" );
	
	printf( "%s\n", list_display_delimited( heading_list, '^' ) );

	if ( !list_rewind( current_element_list ) )
	{
		printf(
	"ERROR: there are no elements for this statement.\n" );
		exit( 1 );
	}

	do {
		element = list_get_pointer( current_element_list );

		if ( !list_rewind( element->subclassification_list ) )
			continue;

		element_name = element->element_name;

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

				if ( ledger_is_period_element(
					element->element_name ) )
				{
					subclassification_total =
						subclassification->
							subclassification_total;
				}
				else
				{
					subclassification_total = 0.0;
				}

				trial_balance_account_stdout(
					&balance,
					&accumulate_debit,
					application_name,
					account,
					prior_element_list,
					element_name,
					subclassification->
						subclassification_name,
					subclassification_total );

				if ( accumulate_debit )
				{
					debit_sum += balance;
				}
				else
				{
					credit_sum += balance;
				}

				subclassification->
					subclassification_name =
						(char *)0;

				element_name = (char *)0;

			} while( list_next( subclassification->account_list ) );

		} while( list_next( element->subclassification_list ) );

	} while( list_next( current_element_list ) );

	list_append_pointer( data_list, "Total" );
	list_append_pointer( data_list, "" );
	list_append_pointer( data_list, "" );
	list_append_pointer( data_list, "" );

	debit_string = timlib_place_commas_in_money( debit_sum );
	list_append_pointer( data_list, strdup( debit_string ) );

	credit_string = timlib_place_commas_in_money( credit_sum );
	list_append_pointer( data_list, strdup( credit_string ) );

	printf( "%s\n", list_display_delimited( data_list, '^' ) );

} /* trial_balance_stdout() */

void trial_balance_account_stdout(
					double *balance,
					boolean *accumulate_debit,
					char *application_name,
					ACCOUNT *account,
					LIST *prior_element_list,
					char *element_name,
					char *subclassification_name,
					double subclassification_total )
{
	double prior_balance_change;

	*accumulate_debit =
		ledger_account_get_accumulate_debit(
			application_name,
			account->account_name );

	*balance = account->latest_ledger->balance;

	prior_balance_change =
		trial_balance_get_prior_balance_change(
			prior_element_list,
			account->account_name,
			*balance /* current_balance */ );

	/* See if negative balance. */
	/* ------------------------ */
	if ( *balance < 0.0 )
	{
		*balance = float_abs( *balance );
		*accumulate_debit = 1 - *accumulate_debit;
	}

	output_stdout(
		element_name,
		subclassification_name,
		account->account_name,
		account->
			latest_ledger->
			full_name,
		account->
			latest_ledger->
			transaction_count,
		*balance,
		account->
			latest_ledger->
			transaction_date_time,
		account->
			latest_ledger->
			memo,
		*accumulate_debit,
		account->
			latest_ledger->
			debit_amount,
		account->
			latest_ledger->
			credit_amount,
		prior_balance_change,
		subclassification_total );

} /* trial_balance_account_stdout() */

void output_stdout(	char *element_name,
			char *subclassification_name,
			char *account_name,
			char *full_name,
			int transaction_count,
			double balance,
			char *transaction_date_time,
			char *memo,
			boolean accumulate_debit,
			double debit_amount,
			double credit_amount,
			double prior_balance_change,
			double subclassification_total )
{
	char element_title[ 128 ];
	char subclassification_title[ 128 ];
	char *account_title;
	char transaction_count_string[ 16 ];
	char *debit_string;
	char *credit_string;
	char *prior_balance_change_string;
	char subclassification_total_ratio_string[ 16 ];
	char transaction_date_string[ 16 ];
	LIST *data_list = list_new();

	if ( element_name && *element_name )
	{
		format_initial_capital(
			element_title,
			element_name );

		list_append_pointer(
			data_list,
			strdup( element_title ) );
	}
	else
		list_append_pointer( data_list, strdup( "" ) );

	if ( subclassification_name && *subclassification_name )
	{
		format_initial_capital(
			subclassification_title,
			subclassification_name );

		list_append_pointer(
			data_list,
			strdup( subclassification_title )  );
	}
	else
		list_append_pointer( data_list, strdup( "" ) );

	account_title =
		get_html_table_account_title(
			account_name,
			full_name,
			debit_amount,
			credit_amount,
			column( transaction_date_string,
				0,
				transaction_date_time ),
			memo );

	list_append_pointer(
		data_list,
		strdup( account_title ) );

	sprintf( transaction_count_string, "%d", transaction_count );

	list_append_pointer(	data_list,
				strdup( transaction_count_string ) );

	/* Set the debit account. */
	/* ---------------------- */
	if ( accumulate_debit )
	{
		debit_string = timlib_place_commas_in_money( balance );
	}
	else
		debit_string = "";

	list_append_pointer(	data_list,
				strdup( debit_string ) );

	/* Set the credit account. */
	/* ----------------------- */
	if ( !accumulate_debit )
	{
		credit_string = timlib_place_commas_in_money( balance );
	}
	else
		credit_string = "";

	list_append_pointer(	data_list,
				strdup( credit_string ) );

	/* Set prior_balance_change (maybe) */
	/* -------------------------------- */
	if ( !timlib_dollar_virtually_same(
			prior_balance_change,
			0.0 ) )
	{
		char buffer[ 32 ];

		prior_balance_change_string =
			timlib_place_commas_in_money(
				prior_balance_change );

		sprintf( buffer, "&Delta;%s", prior_balance_change_string );

		list_append_pointer(
			data_list,
			strdup( buffer ) );
	}
	else
	/* Set subclassification_total ratio (maybe) */
	/* ----------------------------------------- */
	if ( !timlib_dollar_virtually_same(
			subclassification_total,
			0.0 ) )
	{
		sprintf( subclassification_total_ratio_string,
			 "%.0lf%c",
			 (balance / subclassification_total) * 100.0,
			 '%' );

		list_append_pointer(
			data_list,
			strdup( subclassification_total_ratio_string ) );
	}

	printf( "%s\n", list_display_delimited( data_list, '^' ) );

} /* output_stdout() */

char *get_action_string(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *beginning_date,
			char *as_of_date,
			char *account_name )
{
	char action_string[ 4096 ];

	sprintf( action_string,
"/cgi-bin/post_prompt_edit_form?%s^%s:%s^%s^journal_ledger^%s^lookup^prompt^edit_frame^0^lookup_option_radio_button~lookup@llookup_before_drop_down_state~skipped@relation_operator_account_0~equals@account_1~%s@llookup_before_drop_down_base_folder~journal_ledger@relation_operator_transaction_date_time_0~between@from_transaction_date_time_0~%s 00:00:00@to_transaction_date_time_0~%s 23:59:59",
		 login_name,
		 application_name,
		 application_name,
		 session,
		 role_name,
		 account_name,
		 beginning_date,
		 as_of_date );

	return strdup( action_string );

} /* get_action_string() */

