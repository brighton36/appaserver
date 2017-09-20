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

LIST *employee_get_work_day_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date )
{
	return (LIST *)0;

} /* employee_get_work_day_list() */

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

	return 1;

} /* employee_load() */

