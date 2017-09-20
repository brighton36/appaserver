/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/employee.c			*/
/* -------------------------------------------------------------------- */
/* This is the appaserver employee ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */


#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "folder.h"
#include "employee.h"

LIST *employee_get_work_period_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date )
{
	EMPLOYEE_WORK_PERIOD *employee_work_period;
	LIST *employee_work_period_list;
	char *select;
	char sys_string[ 512 ];
	char where[ 256 ];
	char input_buffer[ 512 ];
	char piece_buffer[ 128 ];
	char buffer[ 256 ];
	FILE *input_pipe;

	select =
"begin_work_date,end_work_date,employee_work_hours,gross_pay,federal_tax_withholding_amount,state_tax_withholding_amount,social_security_employee_tax_amount,social_security_employer_tax_amount,medicare_employee_tax_amout,medicare_employer_tax_amount,retirement_contribution_plan_employee_amount,retirement_contribution_plan_employer_amount,health_insurance_employee_amount,health_insurance_employer_amount,federal_unemployment_tax_amount,state_unemployment_tax_amount,union_dues_amount,transaction_date_time";

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "begin_work_date >= '%s'		",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 begin_work_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=employee_work_period	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	employee_work_period_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		employee_work_period =
			employee_work_period_new(
				strdup( piece_buffer )
					/* begin_work_date */ );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		if ( *piece_buffer )
			employee_work_period->end_work_date =
				strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		if ( *piece_buffer )
			employee_work_period->employee_work_hours =
				atof( piece_buffer );

		list_append_pointer(
			employee_work_period_list,
			employee_work_period );
	}

	pclose( input_pipe );
	return employee_work_period_list;

} /* employee_get_work_period_list() */

LIST *employee_get_work_day_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date )
{
	EMPLOYEE_WORK_DAY *employee_work_day;
	LIST *employee_work_day_list;
	char *select;
	char sys_string[ 512 ];
	char where[ 256 ];
	char input_buffer[ 512 ];
	char piece_buffer[ 128 ];
	char buffer[ 256 ];
	FILE *input_pipe;

	select =
"begin_work_date_time,end_work_date_time,employee_work_hours";

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "begin_work_date_time >= '%s'		",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 begin_work_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=employee_work_day	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	employee_work_day_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		employee_work_day =
			employee_work_day_new(
				strdup( piece_buffer )
					/* begin_work_date_time */ );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		if ( *piece_buffer )
			employee_work_day->end_work_date_time =
				strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		if ( *piece_buffer )
		{
			employee_work_day->employee_work_hours =
			employee_work_day->database_employee_work_hours =
				atof( piece_buffer );
		}

		list_append_pointer(
			employee_work_day_list,
			employee_work_day );
	}

	pclose( input_pipe );
	return employee_work_day_list;

} /* employee_get_work_day_list() */

EMPLOYEE_WORK_PERIOD *employee_work_period_new( char *begin_work_date )
{
	EMPLOYEE_WORK_PERIOD *e;

	if ( ! ( e = calloc( 1, sizeof( EMPLOYEE_WORK_PERIOD ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	e->begin_work_date = begin_work_date;
	return e;

} /* employee_work_period_new() */

EMPLOYEE_WORK_DAY *employee_work_day_new( char *begin_work_date_time )
{
	EMPLOYEE_WORK_DAY *e;

	if ( ! ( e = calloc( 1, sizeof( EMPLOYEE_WORK_DAY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	e->begin_work_date_time = begin_work_date_time;
	return e;

} /* employee_work_day_new() */

EMPLOYEE *employee_new(			char *full_name,
					char *street_address )
{
	EMPLOYEE *e;

	if ( ! ( e = calloc( 1, sizeof( EMPLOYEE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	e->full_name = full_name;
	e->street_address = street_address;
	return e;

} /* employee_new() */

boolean employee_load(
		double *hourly_wage,
		double *annual_salary,
		double *commission_sum_extension_percent,
		char **marital_status,
		int *withholding_allowances,
		double *withholding_additional_amount,
		double *retirement_contribution_plan_employee_period_amount,
		double *retirement_contribution_plan_employer_period_amount,
		double *health_insurance_employee_period_amount,
		double *health_insurance_employer_period_amount,
		double *union_dues_period_amount,
		LIST **employee_work_day_list,
		LIST **employee_work_period_list,
		char *application_name,
		char *full_name,
		char *street_address,
		char *begin_work_date )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *select;
	char *results;
	char buffer[ 512 ];
	char piece_buffer[ 128 ];

	select =
"hourly_wage,annual_salary,commission_sum_extension_percent,marital_status,withholding_allowances,withholding_additional_amount,retirement_contribution_plan_employee_period_amount,retirement_contribution_plan_employer_period_amount,health_insurance_employee_period_amount,health_insurance_employer_period_amount,union_dues_period_amount";

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' 		",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=employee			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece( buffer, FOLDER_DATA_DELIMITER, results, 0 );
	if ( *piece_buffer )
		*hourly_wage = atoi( buffer );

	free( results );

	*employee_work_day_list =
		employee_get_work_day_list(
			application_name,
			full_name,
			street_address,
			begin_work_date );

	*employee_work_period_list =
		employee_get_work_period_list(
			application_name,
			full_name,
			street_address,
			begin_work_date );

	return 1;

} /* employee_load() */

