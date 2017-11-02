/* -----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/post_employee_work_period.c	*/
/* -----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "environ.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "ledger.h"
#include "appaserver_parameter_file.h"
#include "date.h"
#include "employee.h"
#include "entity.h"
#include "boolean.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_employee_work_period_delete(
			char *application_name,
			int payroll_year,
			int payroll_period_number,
			boolean execute,
			ENTITY_SELF *self );

void post_employee_work_period_propagate(
			char *application_name,
			char *salary_wage_expense_account,
			char *payroll_expense_account,
			char *payroll_payable_account,
			char *federal_withholding_payable_account,
			char *state_withholding_payable_account,
			char *social_security_payable_account,
			char *medicare_payable_account,
			char *retirement_plan_payable_account,
			char *health_insurance_payable_account,
			char *union_dues_payable_account,
			char *federal_unemployment_tax_payable_account,
			char *state_unemployment_tax_payable_account,
			char *propagate_transaction_date_time );

void post_employee_work_period_insert(
			char *application_name,
			LIST *employee_work_period_list );

void post_employee_work_period_journal_display(
			char *application_name,
			LIST *employee_work_period_list );

double calculate_payroll_tax_percent(
			double gross_pay,
		 	double payroll_tax_amount );

void post_employee_work_period(
			char *application_name,
			LIST *employee_list,
			int payroll_year,
			int payroll_period_number,
			char *begin_work_date,
			char *end_work_date,
			ENTITY_SELF *self );

void post_employee_work_period_employee_display(
			boolean delete,
			int payroll_year,
			int payroll_period_number,
			LIST *employee_work_period_list );

void post_employee_work_period_display(
			char *application_name,
			boolean delete,
			LIST *employee_list,
			int payroll_year,
			int payroll_period_number,
			char *begin_work_date,
			char *end_work_date,
			ENTITY_SELF *self );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *begin_work_date = {0};
	char *end_work_date = {0};
	int payroll_year = 0;
	int payroll_period_number = 0;
	boolean delete;
	boolean with_html;
	boolean execute;
	char title[ 128 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	ENTITY_SELF *self;

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s application process payroll_year period_number delete_yn withhtml_yn execute_yn\n",
			 argv[ 0 ] );
		fprintf( stderr,
"Note: if payroll_year and period_number are missing, then the prior period will be posted.\n" );
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

	process_name = argv[ 2 ];
	payroll_year = atoi( argv[ 3 ] );
	payroll_period_number = atoi( argv[ 4 ] );
	delete = (*argv[ 5 ] == 'y');
	with_html = (*argv[ 6 ] == 'y');
	execute = (*argv[ 7 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( with_html )
	{
		format_initial_capital( title, process_name );
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

		document_output_body(
			document->application_name,
			document->onload_control_string );

		printf( "<h1>%s</h1>\n", title );
		fflush( stdout );
	}

	if ( ! ( self = entity_self_load( application_name ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot fetch from SELF.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !payroll_year )
	{
		employee_get_prior_period(
			&begin_work_date,
			&end_work_date,
			&payroll_year,
			&payroll_period_number,
			self->payroll_pay_period,
			self->payroll_beginning_day );
	}
	else
	if ( !employee_get_payroll_begin_end_work_dates(
			&begin_work_date,
			&end_work_date,
			entity_get_payroll_pay_period_string(
				self->payroll_pay_period ),
			payroll_year,
			payroll_period_number,
			self->payroll_beginning_day ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot get begin/end dates.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( delete )
	{
		post_employee_work_period_delete(
			application_name,
			payroll_year,
			payroll_period_number,
			execute,
			self );

		if ( with_html ) document_close();

		exit( 0 );
	}

	self->employee_list =
		employee_fetch_list(
			application_name,
			begin_work_date,
			end_work_date );

	if ( !execute )
	{
		post_employee_work_period_display(
			application_name,
			0 /* not delete */,
			self->employee_list,
			payroll_year,
			payroll_period_number,
			begin_work_date,
			end_work_date,
			self );
	}
	else
	{
		post_employee_work_period(
			application_name,
			self->employee_list,
			payroll_year,
			payroll_period_number,
			begin_work_date,
			end_work_date,
			self );

		if ( with_html )
		{
			printf( "<h3>Process complete.</h3>\n" );
		}
	}

	if ( with_html ) document_close();

	exit( 0 );

} /* main() */

void post_employee_work_period_display(
			char *application_name,
			boolean delete,
			LIST *employee_list,
			int payroll_year,
			int payroll_period_number,
			char *begin_work_date,
			char *end_work_date,
			ENTITY_SELF *self )
{
	PAYROLL_POSTING *payroll_posting;
	EMPLOYEE_TAX_WITHHOLDING_TABLE *employee_tax_withholding_table;

	employee_tax_withholding_table =
		employee_tax_withholding_table_new(
			application_name );

	if ( ! ( payroll_posting =
			employee_get_payroll_posting(
				employee_list,
				payroll_year,
				payroll_period_number,
				begin_work_date,
				end_work_date,
				self,
				employee_tax_withholding_table ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get payroll_posting.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	post_employee_work_period_employee_display(
		delete,
		payroll_year,
		payroll_period_number,
		payroll_posting->employee_work_period_list );

	post_employee_work_period_journal_display(
		application_name,
		payroll_posting->employee_work_period_list );

} /* post_employee_work_period_display() */

void post_employee_work_period_employee_display(
			boolean delete,
			int payroll_year,
			int payroll_period_number,
			LIST *employee_work_period_list )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *format_string;
	char *heading_list_string;
	char *justify_list_string;
	EMPLOYEE_WORK_PERIOD *e;
	char delete_message[ 16 ];
	char sub_title[ 128 ];
	double payroll_tax_percent;

	format_string =
"%s^%.1lf^%.1lf^%.2lf^%.2lf^%.2lf^%.2lf%c^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf\n";

	heading_list_string = "full_name,regular_hours,overtime_hours,gross_pay,net_pay,payroll_tax,tax_percent,federal_withhold,state_withhold,social_employee,social_employer,medicare_employee,medicare_employer,federal_unemployment,state_unemployment";

	justify_list_string = "left,right";

	if ( delete )
		strcpy( delete_message, "Delete " );
	else
		*delete_message = '\0';

	sprintf(	sub_title,
			"%sYear: %d Period: %d",
			delete_message,
			payroll_year,
			payroll_period_number );

	sprintf( sys_string,
		 "html_table.e '^%s' '%s' '^' '%s'",
		 sub_title,
		 heading_list_string,
		 justify_list_string );

	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( employee_work_period_list ) )
	{
		pclose( output_pipe );
		return;
	}

	do {
		e = list_get_pointer( employee_work_period_list );

		if ( !e->gross_pay ) continue;

		payroll_tax_percent =
			calculate_payroll_tax_percent(
				e->gross_pay,
			 	e->payroll_tax_amount );

		fprintf( output_pipe,
			 format_string,
			 e->full_name,
			 e->regular_work_hours,
			 e->overtime_work_hours,
			 e->gross_pay,
			 e->net_pay,
			 e->payroll_tax_amount,
			 payroll_tax_percent,
			 '%',
			 e->federal_tax_withholding_amount,
			 e->state_tax_withholding_amount,
			 e->social_security_employee_tax_amount,
			 e->social_security_employer_tax_amount,
			 e->medicare_employee_tax_amount,
			 e->medicare_employer_tax_amount,
			 e->federal_unemployment_tax_amount,
			 e->state_unemployment_tax_amount );

	} while( list_next( employee_work_period_list ) );

	pclose( output_pipe );

} /* post_employee_work_period_employee_display() */

void post_employee_work_period(
			char *application_name,
			LIST *employee_list,
			int payroll_year,
			int payroll_period_number,
			char *begin_work_date,
			char *end_work_date,
			ENTITY_SELF *self )
{
	PAYROLL_POSTING *payroll_posting;
	EMPLOYEE_TAX_WITHHOLDING_TABLE *employee_tax_withholding_table;

	employee_tax_withholding_table =
		employee_tax_withholding_table_new(
			application_name );

	payroll_posting =
		employee_get_payroll_posting(
			employee_list,
			payroll_year,
			payroll_period_number,
			begin_work_date,
			end_work_date,
			self,
			employee_tax_withholding_table );

	/* -------------------------------------- */
	/* Sets employee_work_period->transaction */
	/* -------------------------------------- */
	employee_work_period_set_transaction(
		payroll_posting->employee_work_period_list );

	post_employee_work_period_insert(
		application_name,
		payroll_posting->employee_work_period_list );

	printf( "<h3>Process complete.</h3>\n" );

} /* post_employee_work_period() */

double calculate_payroll_tax_percent(
				double gross_pay,
			 	double payroll_tax_amount )
{
	if ( timlib_double_virtually_same(
		gross_pay, 0.0 ) )
	{
		return 0.0;
	}

	return ( payroll_tax_amount / gross_pay ) * 100.0;

} /* calculate_payroll_tax_percent() */

void post_employee_work_period_journal_display(
			char *application_name,
			LIST *employee_work_period_list )
{
	char *heading;
	char *justify;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	double salary_wage_expense;
	EMPLOYEE_WORK_PERIOD *e;
	double total_debit;
	double total_credit;
	char *salary_wage_expense_account = {0};
	char *payroll_expense_account = {0};
	char *payroll_payable_account = {0};
	char *federal_withholding_payable_account = {0};
	char *state_withholding_payable_account = {0};
	char *social_security_payable_account = {0};
	char *medicare_payable_account = {0};
	char *retirement_plan_payable_account = {0};
	char *health_insurance_payable_account = {0};
	char *union_dues_payable_account = {0};
	char *federal_unemployment_tax_payable_account = {0};
	char *state_unemployment_tax_payable_account = {0};

	heading = "Employee,Account,Debit,Credit";
	justify = "left,left,right,right";

	sprintf(sys_string,
		"html_table.e '' %s '^' %s",
		heading,
		justify );

	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( employee_work_period_list ) )
	{
		pclose( output_pipe );
		return;
	}

	ledger_get_payroll_account_names(
		&salary_wage_expense_account,
		&payroll_expense_account,
		&payroll_payable_account,
		&federal_withholding_payable_account,
		&state_withholding_payable_account,
		&social_security_payable_account,
		&medicare_payable_account,
		&retirement_plan_payable_account,
		&health_insurance_payable_account,
		&union_dues_payable_account,
		&federal_unemployment_tax_payable_account,
		&state_unemployment_tax_payable_account,
		application_name,
		(char *)0 /* fund_name */ );

	do {
		e = list_get_pointer( employee_work_period_list );

		if ( !e->gross_pay ) continue;

		/* Debit */
		/* ----- */
		salary_wage_expense =
			e->gross_pay +
			(double)e->
				retirement_contribution_plan_employer_amount +
			(double)e->health_insurance_employer_amount;

		fprintf(output_pipe,
		 	"%s^%s^%.2lf^\n",
			salary_wage_expense_account,
		 	e->full_name,
		 	salary_wage_expense );

		total_debit = salary_wage_expense;

		fprintf(output_pipe,
		 	"^%s^%.2lf^\n",
			payroll_expense_account,
		 	e->payroll_tax_amount );

		total_debit += e->payroll_tax_amount;

		/* Credit */
		/* ------ */
		fprintf(output_pipe,
			"^%s^^%.2lf\n",
			payroll_payable_account,
			e->net_pay );

		total_credit = e->net_pay;

		fprintf(output_pipe,
		 	"^%s^^%.2lf\n",
			federal_withholding_payable_account,
		 	e->federal_tax_withholding_amount );

		total_credit += e->federal_tax_withholding_amount;

		if ( e->state_tax_withholding_amount )
		{
			if ( !state_withholding_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: state_withholding_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			fprintf(output_pipe,
		 		"^%s^^%.2lf\n",
				state_withholding_payable_account,
		 		e->state_tax_withholding_amount );

			total_credit += e->state_tax_withholding_amount;
		}

		fprintf(output_pipe,
		 	"^%s^^%.2lf\n",
			social_security_payable_account,
		 	e->social_security_employee_tax_amount +
		 	e->social_security_employer_tax_amount );

		total_credit +=
			(e->social_security_employee_tax_amount +
			 e->social_security_employer_tax_amount );

		fprintf(output_pipe,
		 	"^%s^^%.2lf\n",
			medicare_payable_account,
		 	e->medicare_employee_tax_amount +
		 	e->medicare_employer_tax_amount );

		total_credit +=
			(e->medicare_employee_tax_amount +
			 e->medicare_employer_tax_amount );

		if ( e->retirement_contribution_plan_employee_amount
		||   e->retirement_contribution_plan_employer_amount )
		{
			if ( !retirement_plan_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: retirement_plan_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			fprintf(
			      output_pipe,
		 	      "^%s^^%d\n",
			      retirement_plan_payable_account,
			      e->retirement_contribution_plan_employee_amount +
			      e->retirement_contribution_plan_employer_amount );

			total_credit +=
			     (e->retirement_contribution_plan_employee_amount +
			      e->retirement_contribution_plan_employer_amount );

		}

		if ( e->health_insurance_employee_amount
		||   e->health_insurance_employer_amount )
		{
			if ( !health_insurance_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: health_insurance_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			fprintf(
			      output_pipe,
		 	      "^%s^^%d\n",
			      health_insurance_payable_account,
			      e->health_insurance_employee_amount +
			      e->health_insurance_employer_amount );

			total_credit +=
			     (e->health_insurance_employee_amount +
			      e->health_insurance_employer_amount );
		}

		if ( e->union_dues_amount )
		{
			if ( !union_dues_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: union_dues_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			fprintf(output_pipe,
		 		"^%s^^%d\n",
				union_dues_payable_account,
				e->union_dues_amount );

			total_credit += e->union_dues_amount;
		}

		fprintf(output_pipe,
		 	"^%s^^%.2lf\n",
			federal_unemployment_tax_payable_account,
		 	e->federal_unemployment_tax_amount );

		total_credit += e->federal_unemployment_tax_amount;

		if ( e->state_unemployment_tax_amount )
		{
			if ( !state_unemployment_tax_payable_account )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: state_unemployment_tax_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			fprintf(output_pipe,
		 		"^%s^^%.2lf\n",
				state_unemployment_tax_payable_account,
		 		e->state_unemployment_tax_amount );

			total_credit += e->state_unemployment_tax_amount;
		}

		fprintf(output_pipe,
		 	"^Total^%.2lf^%.2lf\n",
		 	total_debit,
			total_credit );

	} while( list_next( employee_work_period_list ) );

	pclose( output_pipe );

} /* post_employee_work_period_journal_display() */

void post_employee_work_period_insert(
			char *application_name,
			LIST *employee_work_period_list )
{
	FILE *transaction_output_pipe;
	FILE *debit_account_pipe = {0};
	FILE *credit_account_pipe = {0};
	double salary_wage_expense;
	EMPLOYEE_WORK_PERIOD *e;
	char *salary_wage_expense_account = {0};
	char *payroll_expense_account = {0};
	char *payroll_payable_account = {0};
	char *federal_withholding_payable_account = {0};
	char *state_withholding_payable_account = {0};
	char *social_security_payable_account = {0};
	char *medicare_payable_account = {0};
	char *retirement_plan_payable_account = {0};
	char *health_insurance_payable_account = {0};
	char *union_dues_payable_account = {0};
	char *federal_unemployment_tax_payable_account = {0};
	char *state_unemployment_tax_payable_account = {0};
	char *propagate_transaction_date_time = {0};

	if ( !list_rewind( employee_work_period_list ) ) return;

	transaction_output_pipe =
		ledger_transaction_insert_open_stream(
			application_name );

	ledger_journal_insert_open_stream(
		&debit_account_pipe,
		&credit_account_pipe,
		application_name );

	if ( !transaction_output_pipe
	||   !debit_account_pipe
	||   !credit_account_pipe )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: did open an output pipe.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	ledger_get_payroll_account_names(
		&salary_wage_expense_account,
		&payroll_expense_account,
		&payroll_payable_account,
		&federal_withholding_payable_account,
		&state_withholding_payable_account,
		&social_security_payable_account,
		&medicare_payable_account,
		&retirement_plan_payable_account,
		&health_insurance_payable_account,
		&union_dues_payable_account,
		&federal_unemployment_tax_payable_account,
		&state_unemployment_tax_payable_account,
		application_name,
		(char *)0 /* fund_name */ );

	do {
		e = list_get_pointer( employee_work_period_list );

		if ( !e->gross_pay ) continue;

		ledger_transaction_insert_stream(
			transaction_output_pipe,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
			e->net_pay /* transaction_amount */,
			e->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */ );

		/* Debit */
		/* ----- */
		salary_wage_expense =
			e->gross_pay +
			(double)e->
				retirement_contribution_plan_employer_amount +
			(double)e->health_insurance_employer_amount;

		ledger_journal_insert_stream(
			debit_account_pipe,
			(FILE *)0 /* credit_account_pipe */,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
			salary_wage_expense,
			salary_wage_expense_account /* debit_account_name */,
			(char *)0 /* credit_account_name */ );

		ledger_journal_insert_stream(
			debit_account_pipe,
			(FILE *)0 /* credit_account_pipe */,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
			e->payroll_tax_amount,
			payroll_expense_account /* debit_account_name */,
			(char *)0 /* credit_account_name */ );

		/* Credit */
		/* ------ */
		ledger_journal_insert_stream(
			(FILE *)0 /* debit_account_pipe */,
			credit_account_pipe,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
			e->net_pay,
			(char *)0 /* debit_account_name */,
			payroll_payable_account /* credit_account_name */ );

		ledger_journal_insert_stream(
			(FILE *)0 /* debit_account_pipe */,
			credit_account_pipe,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
			e->federal_tax_withholding_amount,
			(char *)0 /* debit_account_name */,
			federal_withholding_payable_account
				/* credit_account_name */ );

		if ( e->state_tax_withholding_amount )
		{
			ledger_journal_insert_stream(
				(FILE *)0 /* debit_account_pipe */,
				credit_account_pipe,
				e->full_name,
				e->street_address,
				e->transaction->transaction_date_time,
				e->state_tax_withholding_amount,
				(char *)0 /* debit_account_name */,
				state_withholding_payable_account
					/* credit_account_name */ );
		}

		ledger_journal_insert_stream(
			(FILE *)0 /* debit_account_pipe */,
			credit_account_pipe,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
		 	e->social_security_employee_tax_amount +
		 	e->social_security_employer_tax_amount,
			(char *)0 /* debit_account_name */,
			social_security_payable_account
				/* credit_account_name */ );

		ledger_journal_insert_stream(
			(FILE *)0 /* debit_account_pipe */,
			credit_account_pipe,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
		 	e->medicare_employee_tax_amount +
		 	e->medicare_employer_tax_amount,
			(char *)0 /* debit_account_name */,
			medicare_payable_account /* credit_account_name */ );

		if ( e->retirement_contribution_plan_employee_amount
		||   e->retirement_contribution_plan_employer_amount )
		{
			if ( !retirement_plan_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: retirement_plan_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			ledger_journal_insert_stream(
			      (FILE *)0 /* debit_account_pipe */,
			      credit_account_pipe,
			      e->full_name,
			      e->street_address,
			      e->transaction->transaction_date_time,
			      e->retirement_contribution_plan_employee_amount +
			      e->retirement_contribution_plan_employer_amount,
			      (char *)0 /* debit_account_name */,
			      retirement_plan_payable_account
					/* credit_account_name */ );
		}

		if ( e->health_insurance_employee_amount
		||   e->health_insurance_employer_amount )
		{
			if ( !health_insurance_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: health_insurance_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			ledger_journal_insert_stream(
			      (FILE *)0 /* debit_account_pipe */,
			      credit_account_pipe,
			      e->full_name,
			      e->street_address,
			      e->transaction->transaction_date_time,
			      e->health_insurance_employee_amount +
			      e->health_insurance_employer_amount,
			      (char *)0 /* debit_account_name */,
			      health_insurance_payable_account
					/* credit_account_name */ );
		}

		if ( e->union_dues_amount )
		{
			if ( !union_dues_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: union_dues_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			ledger_journal_insert_stream(
				(FILE *)0 /* debit_account_pipe */,
				credit_account_pipe,
				e->full_name,
				e->street_address,
				e->transaction->transaction_date_time,
				e->union_dues_amount,
				(char *)0 /* debit_account_name */,
				union_dues_payable_account
					/* credit_account_name */ );
		}

		ledger_journal_insert_stream(
			(FILE *)0 /* debit_account_pipe */,
			credit_account_pipe,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
		 	e->federal_unemployment_tax_amount,
			(char *)0 /* debit_account_name */,
			federal_unemployment_tax_payable_account
				/* credit_account_name */ );

		if ( e->state_unemployment_tax_amount )
		{
			if ( !state_unemployment_tax_payable_account )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: state_unemployment_tax_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			ledger_journal_insert_stream(
				(FILE *)0 /* debit_account_pipe */,
				credit_account_pipe,
				e->full_name,
				e->street_address,
				e->transaction->transaction_date_time,
		 		e->state_unemployment_tax_amount,
				(char *)0 /* debit_account_name */,
				state_unemployment_tax_payable_account
					/* credit_account_name */ );
		}

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
				e->transaction->transaction_date_time;
		}

	} while( list_next( employee_work_period_list ) );

	pclose( transaction_output_pipe );
	pclose( debit_account_pipe );
	pclose( credit_account_pipe );

	post_employee_work_period_propagate(
		application_name,
		salary_wage_expense_account,
		payroll_expense_account,
		payroll_payable_account,
		federal_withholding_payable_account,
		state_withholding_payable_account,
		social_security_payable_account,
		medicare_payable_account,
		retirement_plan_payable_account,
		health_insurance_payable_account,
		union_dues_payable_account,
		federal_unemployment_tax_payable_account,
		state_unemployment_tax_payable_account,
		propagate_transaction_date_time );

} /* post_employee_work_period_insert() */

void post_employee_work_period_propagate(
			char *application_name,
			char *salary_wage_expense_account,
			char *payroll_expense_account,
			char *payroll_payable_account,
			char *federal_withholding_payable_account,
			char *state_withholding_payable_account,
			char *social_security_payable_account,
			char *medicare_payable_account,
			char *retirement_plan_payable_account,
			char *health_insurance_payable_account,
			char *union_dues_payable_account,
			char *federal_unemployment_tax_payable_account,
			char *state_unemployment_tax_payable_account,
			char *propagate_transaction_date_time )
{
	if ( salary_wage_expense_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			salary_wage_expense_account );
	}

	if ( payroll_expense_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			payroll_expense_account );
	}

	if ( payroll_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			payroll_payable_account );
	}

	if ( federal_withholding_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			federal_withholding_payable_account );
	}

	if ( state_withholding_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			state_withholding_payable_account );
	}

	if ( social_security_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			social_security_payable_account );
	}

	if ( medicare_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			medicare_payable_account );
	}

	if ( retirement_plan_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			retirement_plan_payable_account );
	}

	if ( health_insurance_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			health_insurance_payable_account );
	}

	if ( union_dues_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			union_dues_payable_account );
	}

	if ( federal_unemployment_tax_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			federal_unemployment_tax_payable_account );
	}

	if ( state_unemployment_tax_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			state_unemployment_tax_payable_account );
	}

} /* post_employee_work_period_propagate() */

void post_employee_work_period_delete(
			char *application_name,
			int payroll_year,
			int payroll_period_number,
			boolean execute,
			ENTITY_SELF *self )
{
} /* post_employee_work_period_delete() */

