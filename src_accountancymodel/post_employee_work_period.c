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
void post_employee_work_period(
			char *application_name,
			boolean delete,
			boolean with_html,
			LIST *employee_list,
			int payroll_year,
			int payroll_period_number,
			char *begin_work_date,
			char *end_work_date,
			ENTITY_SELF *self );

void post_employee_work_period_display(
			char *application_name,
			boolean delete,
			LIST *employee_list,
			int payroll_year,
			int payroll_period_number,
			char *begin_work_date,
			char *end_work_date,
			ENTITY_SELF *self,
			PAYROLL_POSTING *payroll_posting );

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

	self->employee_list =
		employee_fetch_list(
			application_name,
			begin_work_date,
			end_work_date );

	if ( !execute )
	{
		post_employee_work_period_display(
			application_name,
			delete,
			self->employee_list,
			payroll_year,
			payroll_period_number,
			begin_work_date,
			end_work_date,
			self,
			(PAYROLL_POSTING *)0 );
	}
	else
	{
		post_employee_work_period(
			application_name,
			delete,
			with_html,
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
			ENTITY_SELF *self,
			PAYROLL_POSTING *payroll_posting )
{
	EMPLOYEE_TAX_WITHHOLDING_TABLE *employee_tax_withholding_table;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *format_string;
	char *heading_list_string;
	char *justify_list_string;
	EMPLOYEE_WORK_PERIOD *e;
	char sub_title[ 128 ];

	if ( !payroll_posting )
	{
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
	}

	format_string =
"%s^%.1lf^%.1lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf\n";

	heading_list_string = "full_name,regular_hours,overtime_hours,gross_pay,federal_withhold,state_withhold,social_employee,social_employer,medicare_employee,medicare_employer,federal_unemployment,state_unemployment";

	justify_list_string = "left,right";

	sprintf(	sub_title,
			"Year: %d Period: %d",
			payroll_year,
			payroll_period_number );

	sprintf( sys_string,
		 "html_table.e '^%s' '%s' '^' '%s'",
		 sub_title,
		 heading_list_string,
		 justify_list_string );

	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( payroll_posting->employee_work_period_list ) )
	{
		pclose( output_pipe );
		return;
	}

	do {
		e = list_get_pointer(
			payroll_posting->
				employee_work_period_list );

		fprintf( output_pipe,
			 format_string,
			 e->full_name,
			 e->regular_work_hours,
			 e->overtime_work_hours,
			 e->gross_pay,
			 e->federal_tax_withholding_amount,
			 e->state_tax_withholding_amount,
			 e->social_security_employee_tax_amount,
			 e->social_security_employer_tax_amount,
			 e->medicare_employee_tax_amount,
			 e->medicare_employer_tax_amount,
			 e->federal_unemployment_tax_amount,
			 e->state_unemployment_tax_amount );

	} while( list_next( payroll_posting->employee_work_period_list ) );

	pclose( output_pipe );

} /* post_employee_work_period_display() */

void post_employee_work_period(
			char *application_name,
			boolean delete,
			boolean with_html,
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

	if ( with_html )
	{
		post_employee_work_period_display(
			application_name,
			delete,
			employee_list,
			payroll_year,
			payroll_period_number,
			begin_work_date,
			end_work_date,
			self,
			payroll_posting );
	}

} /* post_employee_work_period() */

