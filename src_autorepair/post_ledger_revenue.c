/* src_autorepair/post_ledger_revenue.c					*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "piece.h"
#include "list.h"
#include "list_usage.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "autorepair.h"
#include "../src_ledger/ledger.h"

/* Constants */
/* --------- */
#define DAILY_REVENUE			"Daily Revenue"
#define DAILY_RECEIVABLE_PAYMENT	"Daily Receivable Payment"
/* #define SQL_EXECUTABLE			"html_paragraph_wrapper.e" */
#define SQL_EXECUTABLE			"sql.e"

/* Prototypes */
/* ---------- */
void update_ledger_posted_date( char *application_name );

void post_ledger_receivable_payment_completed_date(
				char *application_name,
				char *customer_payment_date,
				char *ledger_cash_account,
				char *ledger_receivable_account,
				char *now_time );

void post_receivable_payment(	char *application_name,
				DICTIONARY *application_constants_dictionary,
				char *now_time );

void insert_journal_ledger_receivable_payment(
				char *application_name,
				char *customer_payment_date,
				char *now_time,
				double sum_total_payment_amount,
				char *ledger_cash_account,
				char *ledger_receivable_account,
				char *memo );

void display_ledger_customer_payment_date_list(
				char *application_name,
				char *ledger_cash_account,
				char *ledger_receivable_account,
				char *customer_payment_date );

LIST *get_part_order_empty_cost_of_goods_sold_list(
				char *application_name,
				LIST *customer_order_completed_date_list );

void display_ledger_revenue_completed_date(
				char *application_name,
				char *ledger_cash_account,
				char *ledger_part_revenue_account,
				char *ledger_service_revenue_account,
				char *ledger_sales_tax_payable_account,
				char *ledger_cost_of_goods_sold_account,
				char *ledger_inventory_account,
				char *ledger_receivable_account,
				char *customer_order_completed_date );

void insert_transaction(	char *application_name,
				char *transaction_date,
				char *now_time,
				char *memo );

void insert_journal_ledger_revenue_completed(
				char *application_name,
				char *customer_order_completed_date,
				char *now_time,
				double sum_invoice_amount,
				double sum_part_order,
				double sum_service_order,
				double sum_sales_tax,
				double sum_cost_of_goods_sold,
				double sum_total_payment_amount,
				char *ledger_cash_account,
				char *ledger_part_revenue_account,
				char *ledger_service_revenue_account,
				char *ledger_sales_tax_payable_account,
				char *ledger_cost_of_goods_sold_account,
				char *ledger_inventory_account,
				char *ledger_receivable_account );

void calculate_receivable_payment_sums(
				double *sum_total_payment_amount,
				char *application_name,
				char *customer_payment_date );

void calculate_customer_order_sums(
				double *sum_invoice_amount,
				double *sum_part_order,
				double *sum_service_order,
				double *sum_sales_tax,
				double *sum_cost_of_goods_sold,
				double *sum_extension,
				double *sum_total_payment_amount,
				char *application_name,
				char *customer_order_completed_date );

void delete_ledger_transaction(	char *application_name,
				char *customer_order_completed_date,
				char *ledger_part_revenue_account,
				char *ledger_service_revenue_account );

boolean get_application_constants(
				char **ledger_cash_account,
				char **ledger_part_revenue_account,
				char **ledger_service_revenue_account,
				char **ledger_sales_tax_payable_account,
				char **ledger_cost_of_goods_sold_account,
				char **ledger_inventory_account,
				char **ledger_receivable_account,
				DICTIONARY *application_constants_dictionary );

void post_ledger_revenue_completed_date(
				char *application_name,
				char *customer_order_completed_date,
				char *ledger_cash_account,
				char *ledger_part_revenue_account,
				char *ledger_service_revenue_account,
				char *ledger_sales_tax_payable_account,
				char *ledger_cost_of_goods_sold_account,
				char *ledger_inventory_account,
				char *ledger_receivable_account,
				char *now_time );

LIST *get_customer_payment_date_list(
				char *application_name );

LIST *get_customer_order_completed_date_list(
				char *application_name );

void post_ledger_revenue(	char *application_name,
				DICTIONARY *application_constants_dictionary,
				char *now_time );

void display_ledger_revenue(	char *application_name,
				DICTIONARY *application_constants_dictionary );

void display_receivable_payment(
				char *application_name,
				DICTIONARY *application_constants_dictionary );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;
	char title[ 128 ];
	char buffer[ 128 ];
	char *database_string = {0};
	APPLICATION_CONSTANTS *application_constants;
	boolean execute;
	char new_path[ 128 ];

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s application process execute_yn\n",
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
	execute = ( *argv[ 3 ] == 'y' );

	appaserver_parameter_file = new_appaserver_parameter_file();

	sprintf(	new_path,
			"%s/src_ledger",
			appaserver_parameter_file->appaserver_mount_point );
	set_path( new_path );

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	sprintf(	title,
			"%s for %s",
			format_initial_capital( buffer, process_name ),
			application_constants_safe_fetch(
				application_constants->dictionary,
				COMPANY_NAME_KEY ) );

	document = document_new( title, application_name );
	document->output_content_type = 1;

	document_output_head_stream(
			stdout,
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */,
			1 /* with close_head */ );

	document_output_body(	document->application_name,
				document->onload_control_string );

	printf( "<h1>%s</h1>\n", title );
	fflush( stdout );

	if ( execute )
	{
		char *now_time;

		now_time = date_get_colon_now_time();
		post_ledger_revenue(	application_name,
					application_constants->dictionary,
					now_time );

		system( "sleep 1" );

		now_time = date_get_colon_now_time();
		post_receivable_payment(
					application_name,
					application_constants->dictionary,
					now_time );

		update_ledger_posted_date( application_name );

		printf( "<h3>Process complete.</h3>\n" );

	}
	else
	{
		printf( "<h3>Ledger Revenue:</h3>\n" );
		display_ledger_revenue(	application_name,
					application_constants->dictionary );

		printf( "<h3>Receivable Payment:</h3>\n" );
		display_receivable_payment(
					application_name,
					application_constants->dictionary );
	}

	document_close();

	return 0;

} /* main() */

LIST *get_customer_payment_date_list(
				char *application_name )
{
	char *where;
	char sys_string[ 1024 ];

	where = "ledger_posted_date is null";

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=payment_date	 "
		 "			folder=customer_payment	 "
		 "			where=\"%s\"		|"
		 "sort -u					 ",
		 application_name,
		 where );

	return pipe2list( sys_string );

} /* get_customer_payment_date_list() */

LIST *get_customer_order_completed_date_list(
			char *application_name )
{
	char *where;
	char sys_string[ 1024 ];

	where = "completed_date is not null and ledger_posted_date is null";

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=completed_date	 "
		 "			folder=customer_order	 "
		 "			where=\"%s\"		|"
		 "sort -u					 ",
		 application_name,
		 where );

	return pipe2list( sys_string );

} /* get_customer_order_completed_date_list() */

void post_receivable_payment(	char *application_name,
				DICTIONARY *application_constants_dictionary,
				char *now_time )
{
	LIST *customer_payment_date_list;
	char *ledger_cash_account;
	char *ledger_part_revenue_account;
	char *ledger_service_revenue_account;
	char *ledger_sales_tax_payable_account;
	char *ledger_cost_of_goods_sold_account;
	char *ledger_inventory_account;
	char *ledger_receivable_account;

	if ( ! get_application_constants(
				&ledger_cash_account,
				&ledger_part_revenue_account,
				&ledger_service_revenue_account,
				&ledger_sales_tax_payable_account,
				&ledger_cost_of_goods_sold_account,
				&ledger_inventory_account,
				&ledger_receivable_account,
				application_constants_dictionary ) )
	{
		fprintf( stderr,
"ERROR In %s/%s()/%d: cannot fetch from application_constants.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		printf(
"<h3>Error: cannot fetch from application_constants.</h3>\n" );
		exit( 1 );
	}

	customer_payment_date_list =
		get_customer_payment_date_list(
			application_name );

	if ( !list_rewind( customer_payment_date_list ) )
	{
		printf( "<h3>Warning: no customer payments to post.</h3>\n" );
		return;
	}

	do {
		post_ledger_receivable_payment_completed_date(
				application_name,
				list_get_pointer(
					customer_payment_date_list ),
				ledger_cash_account,
				ledger_receivable_account,
				now_time );

	} while( list_next( customer_payment_date_list ) );

} /* post_receivable_payment() */

void post_ledger_revenue(	char *application_name,
				DICTIONARY *application_constants_dictionary,
				char *now_time )
{
	LIST *customer_order_completed_date_list;
	char *ledger_cash_account;
	char *ledger_part_revenue_account;
	char *ledger_service_revenue_account;
	char *ledger_sales_tax_payable_account;
	char *ledger_cost_of_goods_sold_account;
	char *ledger_inventory_account;
	char *ledger_receivable_account;

	if ( ! get_application_constants(
				&ledger_cash_account,
				&ledger_part_revenue_account,
				&ledger_service_revenue_account,
				&ledger_sales_tax_payable_account,
				&ledger_cost_of_goods_sold_account,
				&ledger_inventory_account,
				&ledger_receivable_account,
				application_constants_dictionary ) )
	{
		fprintf( stderr,
"ERROR In %s/%s()/%d: cannot fetch from application_constants.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		printf(
"<h3>Error: cannot fetch from application_constants.</h3>\n" );
		exit( 1 );
	}

	customer_order_completed_date_list =
		get_customer_order_completed_date_list(
			application_name );

	if ( !list_rewind( customer_order_completed_date_list ) )
	{
		printf( "<h3>Warning: no customer orders to post.</h3>\n" );
		return;
	}

	do {
		post_ledger_revenue_completed_date(
				application_name,
				list_get_pointer(
					customer_order_completed_date_list ),
				ledger_cash_account,
				ledger_part_revenue_account,
				ledger_service_revenue_account,
				ledger_sales_tax_payable_account,
				ledger_cost_of_goods_sold_account,
				ledger_inventory_account,
				ledger_receivable_account,
				now_time );

	} while( list_next( customer_order_completed_date_list ) );

} /* post_ledger_revenue() */

void display_ledger_revenue(	char *application_name,
				DICTIONARY *application_constants_dictionary )
{
	LIST *customer_order_completed_date_list;
	char *ledger_cash_account;
	char *ledger_part_revenue_account;
	char *ledger_service_revenue_account;
	char *ledger_sales_tax_payable_account;
	char *ledger_cost_of_goods_sold_account;
	char *ledger_inventory_account;
	char *ledger_receivable_account;
	LIST *part_order_empty_cost_of_goods_sold_list;

	if ( ! get_application_constants(
				&ledger_cash_account,
				&ledger_part_revenue_account,
				&ledger_service_revenue_account,
				&ledger_sales_tax_payable_account,
				&ledger_cost_of_goods_sold_account,
				&ledger_inventory_account,
				&ledger_receivable_account,
				application_constants_dictionary ) )
	{
		fprintf( stderr,
"ERROR In %s/%s()/%d: cannot fetch from application_constants.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		printf(
"<h3>Error: cannot fetch from application_constants.</h3>\n" );
		exit( 1 );
	}

	customer_order_completed_date_list =
		get_customer_order_completed_date_list(
			application_name );

	if ( !list_length( customer_order_completed_date_list ) )
	{
		printf( "<h3>Warning: no customer orders to post.</h3>\n" );
		return;
	}

	part_order_empty_cost_of_goods_sold_list =
		get_part_order_empty_cost_of_goods_sold_list(
			application_name,
			customer_order_completed_date_list );

	if ( list_rewind( part_order_empty_cost_of_goods_sold_list ) )
	{
		char *empty_cost;
		char buffer[ 512 ];

		printf(
"<h3>Warning: Not enough inventory quantity remaining for part order:</h3>\n" );

		printf( "<table border=1>\n" );
		printf( "<th>Date<th>Time<th>Customer<th>Make<th>Model<th>Trim<th>Year<th>Inventory<th>Quantity\n" );

		do {
			empty_cost =
			    list_get_pointer( 
				part_order_empty_cost_of_goods_sold_list );

			strcpy( buffer, empty_cost );
			search_replace_string( buffer, "^", "<td>" );
			printf( "<tr><td>%s\n",
				buffer );
		} while( list_next(
				part_order_empty_cost_of_goods_sold_list ) );
		printf( "</table><br>\n" );
	}

	printf( "<table border=1>\n" );
	printf( "<th>Date<th>Account<th>Debit<th>Credit<th>Balance?\n" );

	list_rewind( customer_order_completed_date_list );

	do {
		display_ledger_revenue_completed_date(
			application_name,
			ledger_cash_account,
			ledger_part_revenue_account,
			ledger_service_revenue_account,
			ledger_sales_tax_payable_account,
			ledger_cost_of_goods_sold_account,
			ledger_inventory_account,
			ledger_receivable_account,
			list_get_pointer(
				customer_order_completed_date_list ) );

	} while( list_next( customer_order_completed_date_list ) );

	printf( "</table>\n" );

} /* display_ledger_revenue() */

void display_receivable_payment(
				char *application_name,
				DICTIONARY *application_constants_dictionary )
{
	LIST *customer_payment_date_list;
	char *ledger_cash_account;
	char *ledger_part_revenue_account;
	char *ledger_service_revenue_account;
	char *ledger_sales_tax_payable_account;
	char *ledger_cost_of_goods_sold_account;
	char *ledger_inventory_account;
	char *ledger_receivable_account;

	if ( ! get_application_constants(
				&ledger_cash_account,
				&ledger_part_revenue_account,
				&ledger_service_revenue_account,
				&ledger_sales_tax_payable_account,
				&ledger_cost_of_goods_sold_account,
				&ledger_inventory_account,
				&ledger_receivable_account,
				application_constants_dictionary ) )
	{
		fprintf( stderr,
"ERROR In %s/%s()/%d: cannot fetch from application_constants.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		printf(
"<h3>Error: cannot fetch from application_constants.</h3>\n" );
		exit( 1 );
	}

	customer_payment_date_list =
		get_customer_payment_date_list(
			application_name );

	if ( !list_rewind( customer_payment_date_list ) )
	{
		printf( "<h3>Warning: no customer payments to post.</h3>\n" );
		return;
	}

	printf( "<table border=1>\n" );
	printf( "<th>Date<th>Account<th>Debit<th>Credit\n" );

	do {
		display_ledger_customer_payment_date_list(
			application_name,
			ledger_cash_account,
			ledger_receivable_account,
			list_get_pointer(
				customer_payment_date_list ) );

	} while( list_next( customer_payment_date_list ) );

	printf( "</table>\n" );

} /* display_receivable_payment() */

void display_ledger_customer_payment_date_list(
			char *application_name,
			char *ledger_cash_account,
			char *ledger_receivable_account,
			char *customer_payment_date )
{
	double sum_total_payment_amount;
	char buffer[ 128 ];

	calculate_receivable_payment_sums(
				&sum_total_payment_amount,
				application_name,
				customer_payment_date );

	if ( !sum_total_payment_amount ) return;

	printf( "<tr><td>%s<td>%s<td align=right>%.2lf\n",
		customer_payment_date,
		format_initial_capital( buffer, ledger_cash_account ),
		sum_total_payment_amount );

	printf( "<tr><td><td>%s<td><td align=right>%.2lf\n",
		format_initial_capital(
			buffer,
			ledger_receivable_account ),
		sum_total_payment_amount );

} /* display_ledger_customer_payment_date_list() */

void display_ledger_revenue_completed_date(
				char *application_name,
				char *ledger_cash_account,
				char *ledger_part_revenue_account,
				char *ledger_service_revenue_account,
				char *ledger_sales_tax_payable_account,
				char *ledger_cost_of_goods_sold_account,
				char *ledger_inventory_account,
				char *ledger_receivable_account,
				char *customer_order_completed_date )
{
	double sum_invoice_amount;
	double sum_part_order;
	double sum_service_order;
	double sum_sales_tax;
	double sum_cost_of_goods_sold;
	double sum_extension;
	double sum_total_payment_amount;
	boolean check;
	char buffer[ 128 ];

	calculate_customer_order_sums(
		&sum_invoice_amount,
		&sum_part_order,
		&sum_service_order,
		&sum_sales_tax,
		&sum_cost_of_goods_sold,
		&sum_extension,
		&sum_total_payment_amount,
		application_name,
		customer_order_completed_date );

	if ( !sum_invoice_amount ) return;

	check = timlib_double_virtually_same(
			sum_part_order + sum_service_order + sum_sales_tax,
			sum_invoice_amount );

	if ( check )
	{
		check = timlib_double_virtually_same(
				sum_extension,
				sum_part_order );
	}

	printf( "<tr><td>%s<td>%s<td align=right>%.2lf<td><td>%s\n",
		customer_order_completed_date,
		format_initial_capital( buffer, ledger_cash_account ),
		sum_total_payment_amount,
		(check) ? "Yes" : "NO" );

	if ( !timlib_double_virtually_same(
		sum_invoice_amount,
		sum_total_payment_amount ) )
	{
		printf( "<tr><td><td>%s<td align=right>%.2lf\n",
			format_initial_capital(
				buffer,
				ledger_receivable_account ),
			sum_invoice_amount -
			sum_total_payment_amount );
	}

	printf( "<tr><td><td>%s<td align=right>%.2lf\n",
		format_initial_capital(	buffer,
					ledger_cost_of_goods_sold_account ),
		sum_cost_of_goods_sold );

	printf( "<tr><td><td>%s<td><td align=right>%.2lf\n",
		format_initial_capital( buffer, ledger_part_revenue_account ),
		sum_part_order );

	if ( sum_service_order )
	{
		printf( "<tr><td><td>%s<td><td align=right>%.2lf\n",
			format_initial_capital(
				buffer,
				ledger_service_revenue_account ),
			sum_service_order );
	}

	printf( "<tr><td><td>%s<td><td align=right>%.2lf\n",
		format_initial_capital(	buffer,
					ledger_sales_tax_payable_account ),
		sum_sales_tax );

	printf( "<tr><td><td>%s<td><td align=right>%.2lf\n",
		format_initial_capital(	buffer,
					ledger_inventory_account ),
		sum_cost_of_goods_sold );

} /* display_ledger_revenue_completed_date() */

void post_ledger_receivable_payment_completed_date(
				char *application_name,
				char *customer_payment_date,
				char *ledger_cash_account,
				char *ledger_receivable_account,
				char *now_time )
{
	double sum_total_payment_amount;

	calculate_receivable_payment_sums(
		&sum_total_payment_amount,
		application_name,
		customer_payment_date );

	if ( !sum_total_payment_amount ) return;

	insert_transaction(	application_name,
				customer_payment_date,
				now_time,
				DAILY_RECEIVABLE_PAYMENT );
	
	insert_journal_ledger_receivable_payment(
				application_name,
				customer_payment_date,
				now_time,
				sum_total_payment_amount,
				ledger_cash_account,
				ledger_receivable_account,
				DAILY_RECEIVABLE_PAYMENT );

} /* post_ledger_receivable_payment_completed_date() */

void post_ledger_revenue_completed_date(
				char *application_name,
				char *customer_order_completed_date,
				char *ledger_cash_account,
				char *ledger_part_revenue_account,
				char *ledger_service_revenue_account,
				char *ledger_sales_tax_payable_account,
				char *ledger_cost_of_goods_sold_account,
				char *ledger_inventory_account,
				char *ledger_receivable_account,
				char *now_time )
{
	double sum_invoice_amount;
	double sum_part_order;
	double sum_service_order;
	double sum_sales_tax;
	double sum_cost_of_goods_sold;
	double sum_extension;
	double sum_total_payment_amount;

	calculate_customer_order_sums(
		&sum_invoice_amount,
		&sum_part_order,
		&sum_service_order,
		&sum_sales_tax,
		&sum_cost_of_goods_sold,
		&sum_extension,
		&sum_total_payment_amount,
		application_name,
		customer_order_completed_date );

	if ( !sum_invoice_amount ) return;

/*
	delete_ledger_transaction(
		application_name,
		customer_order_completed_date,
		ledger_part_revenue_account,
		ledger_service_revenue_account );
*/

	insert_journal_ledger_revenue_completed(
				application_name,
				customer_order_completed_date,
				now_time,
				sum_invoice_amount,
				sum_part_order,
				sum_service_order,
				sum_sales_tax,
				sum_cost_of_goods_sold,
				sum_total_payment_amount,
				ledger_cash_account,
				ledger_part_revenue_account,
				ledger_service_revenue_account,
				ledger_sales_tax_payable_account,
				ledger_cost_of_goods_sold_account,
				ledger_inventory_account,
				ledger_receivable_account );

} /* post_ledger_revenue_completed_date() */

boolean get_application_constants(
				char **ledger_cash_account,
				char **ledger_part_revenue_account,
				char **ledger_service_revenue_account,
				char **ledger_sales_tax_payable_account,
				char **ledger_cost_of_goods_sold_account,
				char **ledger_inventory_account,
				char **ledger_receivable_account,
				DICTIONARY *application_constants_dictionary )
{
	*ledger_cash_account =
		application_constants_safe_fetch(
			application_constants_dictionary,
			LEDGER_CASH_ACCOUNT_KEY );
	if ( !*ledger_cash_account ) return 0;

	*ledger_part_revenue_account =
		application_constants_safe_fetch(
			application_constants_dictionary,
			LEDGER_SALES_REVENUE_ACCOUNT_KEY );
	if ( !*ledger_part_revenue_account ) return 0;

	*ledger_sales_tax_payable_account =
		application_constants_safe_fetch(
			application_constants_dictionary,
			LEDGER_SALES_TAX_PAYABLE_ACCOUNT_KEY );
	if ( !*ledger_sales_tax_payable_account ) return 0;

	*ledger_service_revenue_account =
		application_constants_safe_fetch(
			application_constants_dictionary,
			LEDGER_SERVICE_REVENUE_ACCOUNT_KEY );
	if ( !*ledger_service_revenue_account ) return 0;

	*ledger_cost_of_goods_sold_account =
		application_constants_safe_fetch(
			application_constants_dictionary,
			LEDGER_COST_OF_GOODS_SOLD_ACCOUNT_KEY );
	if ( !*ledger_cost_of_goods_sold_account ) return 0;

	*ledger_inventory_account =
		application_constants_safe_fetch(
			application_constants_dictionary,
			LEDGER_INVENTORY_ACCOUNT_KEY );
	if ( !*ledger_inventory_account ) return 0;

	*ledger_receivable_account =
		application_constants_safe_fetch(
			application_constants_dictionary,
			LEDGER_RECEIVABLE_ACCOUNT_KEY );
	if ( !*ledger_receivable_account ) return 0;

	return 1;

} /* get_application_constants() */

void delete_ledger_transaction(	char *application_name,
				char *customer_order_completed_date,
				char *ledger_part_revenue_account,
				char *ledger_service_revenue_account )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "delete_transactions_with_debit_account.sh %s %s '%s,%s'",
		 application_name,
		 customer_order_completed_date,
		 ledger_part_revenue_account,
		 ledger_service_revenue_account );

	system( sys_string );

} /* delete_ledger_transaction() */

void calculate_receivable_payment_sums(
				double *sum_total_payment_amount,
				char *application_name,
				char *customer_payment_date )
{
	char sys_string[ 2048 ];
	char *select;
	char where[ 1024 ];
	char *customer_order_table;
	char *customer_payment_table;
	char *results;

	customer_order_table =
		get_table_name(
			application_name, "customer_order" );

	customer_payment_table =
		get_table_name(
			application_name, "customer_payment" );

	/* From CUSTOMER_PAYMENT */
	/* --------------------- */
	sprintf( where,
		 "payment_date = '%s' and			"
		 "%s.ledger_posted_date is null and		"
		 "%s.ledger_posted_date is not null and		"
		 "%s.order_date = %s.order_date and		"
		 "%s.order_time = %s.order_time and		"
		 "%s.customer_name = %s.customer_name and	"
		 "%s.vehicle_make = %s.vehicle_make and		"
		 "%s.vehicle_model = %s.vehicle_model and	"
		 "%s.vehicle_trim = %s.vehicle_trim and		"
		 "%s.vehicle_year = %s.vehicle_year 		",
		 customer_payment_date,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table );

	select = "sum( payment_amount )";

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s;\" | sql.e",
		 select,
		 customer_payment_table,
		 customer_order_table,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s())/%d: pipe2string() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	*sum_total_payment_amount = atof( results );

} /* calculate_receivable_payment_sums() */

void calculate_customer_order_sums(
		double *sum_invoice_amount,
		double *sum_part_order,
		double *sum_service_order,
		double *sum_sales_tax,
		double *sum_cost_of_goods_sold,
		double *sum_extension,
		double *sum_total_payment_amount,
		char *application_name,
		char *customer_order_completed_date )
{
	char sys_string[ 2048 ];
	char *select;
	char where[ 1024 ];
	char *customer_order_table;
	char *part_order_table;
	char *service_order_table;
	char *customer_payment_table;
	char *results;
	char piece_buffer[ 128 ];

	customer_order_table =
		get_table_name(
			application_name, "customer_order" );

	part_order_table =
		get_table_name(
			application_name, "part_order" );

	service_order_table =
		get_table_name(
			application_name, "service_order" );

	customer_payment_table =
		get_table_name(
			application_name, "customer_payment" );

	/* From CUSTOMER_ORDER */
	/* ------------------- */
	sprintf( where,
		 "completed_date = '%s'",
		 customer_order_completed_date );

	select = "sum(sales_tax),sum(invoice_amount)";

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 select,
		 customer_order_table,
		 where );

	results = pipe2string( sys_string );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*sum_sales_tax = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*sum_invoice_amount = atof( piece_buffer );

	/* From PART_ORDER */
	/* --------------- */
	sprintf( where,
		 "ledger_posted_date is null and		"
		 "completed_date = '%s' and			"
		 "%s.order_date = %s.order_date and		"
		 "%s.order_time = %s.order_time and		"
		 "%s.customer_name = %s.customer_name and	"
		 "%s.vehicle_make = %s.vehicle_make and		"
		 "%s.vehicle_model = %s.vehicle_model and	"
		 "%s.vehicle_trim = %s.vehicle_trim and		"
		 "%s.vehicle_year = %s.vehicle_year		",
		 customer_order_completed_date,
		 customer_order_table,
		 part_order_table,
		 customer_order_table,
		 part_order_table,
		 customer_order_table,
		 part_order_table,
		 customer_order_table,
		 part_order_table,
		 customer_order_table,
		 part_order_table,
		 customer_order_table,
		 part_order_table,
		 customer_order_table,
		 part_order_table );

	select =
"sum( quantity * retail_price - ifnull( discount_amount, 0 ) ), sum( cost_of_goods_sold ), sum( extension )";

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s;\" | sql.e",
		 select,
		 customer_order_table,
		 part_order_table,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s())/%d: pipe2string() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*sum_part_order = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*sum_cost_of_goods_sold = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
	*sum_extension = atof( piece_buffer );

	/* From SERVICE_ORDER */
	/* ------------------ */
	sprintf( where,
		 "ledger_posted_date is null and		"
		 "completed_date = '%s' and			"
		 "%s.order_date = %s.order_date and		"
		 "%s.order_time = %s.order_time and		"
		 "%s.customer_name = %s.customer_name and	"
		 "%s.vehicle_make = %s.vehicle_make and		"
		 "%s.vehicle_model = %s.vehicle_model and	"
		 "%s.vehicle_trim = %s.vehicle_trim and		"
		 "%s.vehicle_year = %s.vehicle_year		",
		 customer_order_completed_date,
		 customer_order_table,
		 service_order_table,
		 customer_order_table,
		 service_order_table,
		 customer_order_table,
		 service_order_table,
		 customer_order_table,
		 service_order_table,
		 customer_order_table,
		 service_order_table,
		 customer_order_table,
		 service_order_table,
		 customer_order_table,
		 service_order_table );

	select = "sum( retail_price - ifnull( discount_amount, 0 ) )";

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s;\" | sql.e",
		 select,
		 customer_order_table,
		 service_order_table,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s())/%d: pipe2string() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	*sum_service_order = atof( results );

	/* From CUSTOMER_PAYMENT */
	/* --------------------- */
	sprintf( where,
		 "%s.ledger_posted_date is null and		"
		 "%s.ledger_posted_date is null and		"
		 "completed_date = '%s' and			"
		 "%s.order_date = %s.order_date and		"
		 "%s.order_time = %s.order_time and		"
		 "%s.customer_name = %s.customer_name and	"
		 "%s.vehicle_make = %s.vehicle_make and		"
		 "%s.vehicle_model = %s.vehicle_model and	"
		 "%s.vehicle_trim = %s.vehicle_trim and		"
		 "%s.vehicle_year = %s.vehicle_year		",
		 customer_order_table,
		 customer_payment_table,
		 customer_order_completed_date,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table,
		 customer_order_table,
		 customer_payment_table );

	select = "sum( payment_amount )";

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s;\" | sql.e",
		 select,
		 customer_order_table,
		 customer_payment_table,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s())/%d: pipe2string() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	*sum_total_payment_amount = atof( results );

} /* calculate_customer_order_sums() */

void insert_transaction(	char *application_name,
				char *transaction_date,
				char *now_time,
				char *memo )
{
	char *table_name;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *field;
	char now_year[ 16 ];

	piece( now_year, '-', transaction_date, 0 );

	table_name = get_table_name( application_name, "transaction" );
	field = "transaction_date,transaction_time,memo,fiscal_year";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s delimiter='^' | %s",
		 table_name,
		 field,
		 SQL_EXECUTABLE );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s^%s^%s^%s\n",
		 transaction_date,
		 now_time,
		 memo,
		 now_year );

	pclose( output_pipe );

} /* insert_transaction() */

void insert_journal_ledger_receivable_payment(
				char *application_name,
				char *customer_payment_date,
				char *now_time,
				double sum_total_payment_amount,
				char *ledger_cash_account,
				char *ledger_receivable_account,
				char *memo )
{
	char *table_name;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *field;

	table_name =
		get_table_name(
			application_name, LEDGER_FOLDER_NAME );

	if ( !sum_total_payment_amount ) return;

	/* Cash */
	/* ---- */
	field = "transaction_date,transaction_time,memo,account,debit_amount";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s delimiter='^' | %s",
		 table_name,
		 field,
		 SQL_EXECUTABLE );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^%s^%.2lf\n",
	 	customer_payment_date,
	 	now_time,
		memo,
	 	ledger_cash_account,
	 	sum_total_payment_amount );

	pclose( output_pipe );

	sprintf( sys_string,
		 "ledger_propagate %s %s %s '%s'",
		 application_name,
		 customer_payment_date,
		 now_time,
		 ledger_cash_account );
	system( sys_string );

	/* Account Receivable */
	/* ------------------ */
	field = "transaction_date,transaction_time,memo,account,credit_amount";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s delimiter='^' | %s",
		 table_name,
		 field,
		 SQL_EXECUTABLE );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^%s^%.2lf\n",
	 	customer_payment_date,
	 	now_time,
		memo,
	 	ledger_receivable_account,
	 	sum_total_payment_amount );

	pclose( output_pipe );

	sprintf( sys_string,
		 "ledger_propagate %s %s %s '%s'",
		 application_name,
		 customer_payment_date,
		 now_time,
		 ledger_receivable_account );
	system( sys_string );

} /* insert_journal_ledger_receivable_payment() */

void insert_journal_ledger_revenue_completed(
				char *application_name,
				char *customer_order_completed_date,
				char *now_time,
				double sum_invoice_amount,
				double sum_part_order,
				double sum_service_order,
				double sum_sales_tax,
				double sum_cost_of_goods_sold,
				double sum_total_payment_amount,
				char *ledger_cash_account,
				char *ledger_part_revenue_account,
				char *ledger_service_revenue_account,
				char *ledger_sales_tax_payable_account,
				char *ledger_cost_of_goods_sold_account,
				char *ledger_inventory_account,
				char *ledger_receivable_account )
{
	char *table_name;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *field;

	table_name =
		get_table_name(
			application_name, LEDGER_FOLDER_NAME );

	if ( !sum_invoice_amount ) return;

	insert_transaction(	application_name,
				customer_order_completed_date,
				now_time,
				DAILY_REVENUE );

	field = "transaction_date,transaction_time,memo,account,debit_amount";

	/* Cash */
	/* ---- */
	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s delimiter='^' | %s",
		 table_name,
		 field,
		 SQL_EXECUTABLE );

	output_pipe = popen( sys_string, "w" );

	if ( timlib_double_virtually_same(
		sum_invoice_amount,
		sum_total_payment_amount ) )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^%s^%.2lf\n",
		 	customer_order_completed_date,
		 	now_time,
			DAILY_REVENUE,
		 	ledger_cash_account,
		 	sum_invoice_amount );
	}
	else
	if ( sum_total_payment_amount )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^%s^%.2lf\n",
		 	customer_order_completed_date,
		 	now_time,
			DAILY_REVENUE,
		 	ledger_cash_account,
		 	sum_total_payment_amount );
	}

	pclose( output_pipe );

	sprintf( sys_string,
		 "ledger_propagate %s %s %s '%s'",
		 application_name,
		 customer_order_completed_date,
		 now_time,
		 ledger_cash_account );
	system( sys_string );

	/* Account Receivable */
	/* ------------------ */
	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s delimiter='^' | %s",
		 table_name,
		 field,
		 SQL_EXECUTABLE );

	output_pipe = popen( sys_string, "w" );

	if ( !timlib_double_virtually_same(
		sum_invoice_amount,
		sum_total_payment_amount ) )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^%s^%.2lf\n",
		 	customer_order_completed_date,
		 	now_time,
			DAILY_REVENUE,
		 	ledger_receivable_account,
		 	sum_invoice_amount - sum_total_payment_amount );
	}

	pclose( output_pipe );

	sprintf( sys_string,
		 "ledger_propagate %s %s %s '%s'",
		 application_name,
		 customer_order_completed_date,
		 now_time,
		 ledger_receivable_account );
	system( sys_string );

	/* Part Revenue */
	/* ------------ */
	if ( sum_part_order )
	{
		field =
		"transaction_date,transaction_time,memo,account,credit_amount";
	
		sprintf( sys_string,
			 "insert_statement.e t=%s f=%s d='^' | %s",
			 table_name,
			 field,
			 SQL_EXECUTABLE );
	
		output_pipe = popen( sys_string, "w" );
	
		fprintf( output_pipe,
			 "%s^%s^%s^%s^%.2lf\n",
			 customer_order_completed_date,
			 now_time,
			 DAILY_REVENUE,
			 ledger_part_revenue_account,
			 sum_part_order );

		/* Inventory */
		/* --------- */
		fprintf( output_pipe,
			 "%s^%s^%s^%s^%.2lf\n",
			 customer_order_completed_date,
			 now_time,
			 DAILY_REVENUE,
			 ledger_inventory_account,
			 sum_cost_of_goods_sold );

		pclose( output_pipe );

		sprintf( sys_string,
		 	"ledger_propagate %s %s %s '%s'",
		 	application_name,
		 	customer_order_completed_date,
		 	now_time,
		 	ledger_part_revenue_account );
		system( sys_string );

		sprintf( sys_string,
		 	"ledger_propagate %s %s %s '%s'",
		 	application_name,
		 	customer_order_completed_date,
		 	now_time,
		 	ledger_inventory_account );
		system( sys_string );

		/* Cost of goods sold */
		/* ------------------ */
		field =
		"transaction_date,transaction_time,memo,account,debit_amount";
	
		sprintf( sys_string,
			 "insert_statement.e t=%s f=%s d='^' | %s",
			 table_name,
			 field,
			 SQL_EXECUTABLE );
	
		output_pipe = popen( sys_string, "w" );
	
		fprintf( output_pipe,
			 "%s^%s^%s^%s^%.2lf\n",
			 customer_order_completed_date,
			 now_time,
			 DAILY_REVENUE,
			 ledger_cost_of_goods_sold_account,
			 sum_cost_of_goods_sold );

		pclose( output_pipe );

		sprintf( sys_string,
		 	"ledger_propagate %s %s %s '%s'",
		 	application_name,
		 	customer_order_completed_date,
		 	now_time,
		 	ledger_cost_of_goods_sold_account );
		system( sys_string );

	}

	/* Service Revenue */
	/* --------------- */
	if ( sum_service_order )
	{
		field =
		"transaction_date,transaction_time,memo,account,credit_amount";
	
		sprintf( sys_string,
			 "insert_statement.e t=%s f=%s d='^' | %s",
			 table_name,
			 field,
			 SQL_EXECUTABLE );
	
		output_pipe = popen( sys_string, "w" );
	
		fprintf( output_pipe,
			 "%s^%s^%s^%s^%.2lf\n",
			 customer_order_completed_date,
			 now_time,
			 DAILY_REVENUE,
			 ledger_service_revenue_account,
			 sum_service_order );
	
		pclose( output_pipe );

		sprintf( sys_string,
		 	"ledger_propagate %s %s %s '%s'",
		 	application_name,
		 	customer_order_completed_date,
		 	now_time,
		 	ledger_service_revenue_account );
		system( sys_string );
	}

	/* Sales Tax Payable */
	/* ----------------- */
	if ( sum_sales_tax )
	{
		field =
		"transaction_date,transaction_time,memo,account,credit_amount";
	
		sprintf( sys_string,
			 "insert_statement.e t=%s f=%s d='^' | %s",
			 table_name,
			 field,
			 SQL_EXECUTABLE );
	
		output_pipe = popen( sys_string, "w" );
	
		fprintf( output_pipe,
			 "%s^%s^%s^%s^%.2lf\n",
			 customer_order_completed_date,
			 now_time,
			 DAILY_REVENUE,
			 ledger_sales_tax_payable_account,
			 sum_sales_tax );
	
		pclose( output_pipe );

		sprintf( sys_string,
		 	"ledger_propagate %s %s %s '%s'",
		 	application_name,
		 	customer_order_completed_date,
		 	now_time,
		 	ledger_sales_tax_payable_account );
		system( sys_string );
	}

} /* insert_journal_ledger_revenue_completed() */

void update_ledger_posted_date( char *application_name )
{
	char sys_string[ 512 ];
	char *table_name;
	char *now;

	now = pipe2string( "now.sh ymd" );

	table_name =
		get_table_name(
			application_name,
			"customer_order" );

	sprintf( sys_string,
		 "echo \"	update %s			 	 "
		 "		set ledger_posted_date = '%s'	 	 "
		 "		where ledger_posted_date is null;\"	|"
		 "%s						 	 ",
		 table_name,
		 now,
		 SQL_EXECUTABLE );

	system( sys_string );

	table_name =
		get_table_name(
			application_name,
			"customer_payment" );

	sprintf( sys_string,
		 "echo \"	update %s			 	 "
		 "		set ledger_posted_date = '%s'	 	 "
		 "		where ledger_posted_date is null;\"	|"
		 "%s						 	 ",
		 table_name,
		 now,
		 SQL_EXECUTABLE );

	system( sys_string );

} /* update_ledger_posted_date() */

LIST *get_part_order_empty_cost_of_goods_sold_list(
			char *application_name,
			LIST *customer_order_completed_date_list )
{
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *select;

	select =
"order_date,order_time,customer_name,vehicle_make,vehicle_model,vehicle_trim,vehicle_year,inventory_name,quantity";

	sprintf( where,
		 "cost_of_goods_sold = 0 and order_date in %s",
		 list_usage_get_in_clause(
			customer_order_completed_date_list ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=part_order	"
		 "			where=\"%s\"		"
		 "			order=select		",
		 application_name,
		 select,
		 where );

	return pipe2list( sys_string );

} /* get_part_order_empty_cost_of_goods_sold_list() */

