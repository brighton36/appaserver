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
#include "html_table.h"
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
			LIST *employee_list );

void post_employee_work_period_display(
			char *application_name,
			boolean delete,
			LIST *employee_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	boolean delete;
	int period_year;
	int period_number;
	char *begin_work_date;
	char *end_work_date;
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
"Usage: %s application process delete_yn period_year period_number withhtml_yn execute_yn\n",
			 argv[ 0 ] );
		fprintf( stderr,
"Note: if period_year and period_number are missing, then the prior period will be posted.\n" );
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
	delete = (*argv[ 3 ] == 'y');
	period_year = atoi( argv[ 4 ] );
	period_number = atoi( argv[ 5 ] );
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

	self = entity_self_load( application_name );

	if (	!*begin_work_date
	||	strcmp( begin_work_date, "begin_work_date" ) == 0 )
	{
		begin_work_date =
			employee_get_prior_period_begin_end_work_dates(
				&end_work_date,
				self->payroll_pay_period );
	}
	else
	{
		/* employee_get_list() select all records. */
		/* --------------------------------------- */
		begin_work_date = (char *)0;
		end_work_date = (char *)0;
	}

fprintf( stderr, "%s/%s()/%d: got begin_work_date = %s\n",
__FILE__,
__FUNCTION__,
__LINE__,
begin_work_date );

	self->employee_list =
		employee_get_list(
			application_name,
			begin_work_date );

	if ( !execute )
	{
		post_employee_work_period_display(
			application_name,
			delete,
			self->employee_list );
	}
	else
	{
		post_employee_work_period(
			application_name,
			delete,
			with_html,
			self->employee_list );

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
			LIST *employee_list )
{

} /* post_employee_work_period_display() */

void post_employee_work_period(
			char *application_name,
			boolean delete,
			boolean with_html,
			LIST *employee_list )
{
	if ( with_html )
	{
		post_employee_work_period_display(
			application_name,
			delete,
			employee_list );
	}

} /* post_employee_work_period() */

