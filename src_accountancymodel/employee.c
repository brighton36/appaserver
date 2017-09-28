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
#include "date.h"
#include "employee.h"

LIST *employee_get_work_period_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date )
{
	EMPLOYEE_WORK_PERIOD *employee_work_period;
	LIST *employee_work_period_list;
	char *select;
	char sys_string[ 1024 ];
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
		 (begin_work_date)
			? begin_work_date
			: "1900-01-01" );

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

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		if ( *piece_buffer )
			employee_work_period->end_work_date =
				strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );

		if ( *piece_buffer )
			employee_work_period->employee_work_hours =
				atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );

		if ( *piece_buffer )
			employee_work_period->gross_pay =
				atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );

		if ( *piece_buffer )
			employee_work_period->
				federal_tax_withholding_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );

		if ( *piece_buffer )
			employee_work_period->
				state_tax_withholding_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );

		if ( *piece_buffer )
			employee_work_period->
				social_security_employee_tax_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );

		if ( *piece_buffer )
			employee_work_period->
				social_security_employer_tax_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );

		if ( *piece_buffer )
			employee_work_period->
				medicare_employee_tax_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );

		if ( *piece_buffer )
			employee_work_period->
				medicare_employer_tax_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 10 );

		if ( *piece_buffer )
			employee_work_period->
				retirement_contribution_plan_employee_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 11 );

		if ( *piece_buffer )
			employee_work_period->
				retirement_contribution_plan_employer_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 12 );

		if ( *piece_buffer )
			employee_work_period->
				health_insurance_employee_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 13 );

		if ( *piece_buffer )
			employee_work_period->
				health_insurance_employer_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 14 );

		if ( *piece_buffer )
			employee_work_period->
				federal_unemployment_tax_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 15 );

		if ( *piece_buffer )
			employee_work_period->
				state_unemployment_tax_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 16 );

		if ( *piece_buffer )
			employee_work_period->
				union_dues_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 17 );

		if ( *piece_buffer )
		{
			employee_work_period->
				transaction_date_time =
					strdup( piece_buffer );
		}
		else
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty transaction_date_time.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );

			pclose( input_pipe );
			exit( 1 );
		}

		employee_work_period->transaction =
			ledger_transaction_with_load_new(
				application_name,
				full_name,
				street_address,
				employee_work_period->
					transaction_date_time );

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
	char sys_string[ 1024 ];
	char where[ 512 ];
	char input_buffer[ 512 ];
	char piece_buffer[ 128 ];
	char buffer[ 256 ];
	FILE *input_pipe;

	select =
"substr(begin_work_date_time,1,16),substr(end_work_date_time,1,16),employee_work_hours";

	sprintf(where,
	 	"full_name = '%s' and			"
	 	"street_address = '%s' and		"
	 	"begin_work_date_time >= '%s'		",
	 	escape_character(	buffer,
					full_name,
					'\'' ),
	 	street_address,
	 	(begin_work_date)
			? begin_work_date
			: "1999-01-01" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=employee_work_day	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	employee_work_day_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );

		employee_work_day =
			employee_work_day_new(
				strdup( piece_buffer )
					/* begin_work_date_time */ );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		if ( *piece_buffer )
			employee_work_day->end_work_date_time =
				strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );

		if ( *piece_buffer )
		{
			employee_work_day->database_employee_work_hours =
				atof( piece_buffer );
		}

		if ( employee_work_day->end_work_date_time )
		{
			DATE *begin_date_time;
			DATE *end_date_time;

			begin_date_time =
				date_yyyy_mm_dd_hm_new(
					employee_work_day->
						begin_work_date_time );

			if ( !begin_date_time )
			{
				fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot strip seconds off (%s).\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 employee_work_day->
					 	begin_work_date_time );

				pclose( input_pipe );
				exit( 1 );
			}

			end_date_time =
				date_yyyy_mm_dd_hm_new(
					employee_work_day->
						end_work_date_time );

			if ( !end_date_time )
			{
				fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot strip seconds off (%s).\n",
				 	__FILE__,
				 	__FUNCTION__,
				 	__LINE__,
				 	employee_work_day->
					 	end_work_date_time );

				pclose( input_pipe );
				exit( 1 );
			}

			employee_work_day->employee_work_hours =
				(double)date_subtract_minutes(
					end_date_time /* later_date */,
					begin_date_time /* earlier_date */ )
				/ 60.0;

		} /* if ( employee_work_day->end_work_date_time ) */

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

EMPLOYEE *employee_with_load_new(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date )
{
	EMPLOYEE *e;

	e = employee_new( full_name, street_address );

	if ( !employee_load(
		&e->hourly_wage,
		&e->period_salary,
		&e->commission_sum_extension_percent,
		&e->marital_status,
		&e->withholding_allowances,
		&e->withholding_additional_amount,
		&e->retirement_contribution_plan_employee_period_amount,
		&e->retirement_contribution_plan_employer_period_amount,
		&e->health_insurance_employee_period_amount,
		&e->health_insurance_employer_period_amount,
		&e->union_dues_period_amount,
		&e->employee_work_day_list,
		&e->employee_work_period_list,
		application_name,
		full_name,
		street_address,
		begin_work_date ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load %s/%s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address );
		exit( 1 );
	}

	return e;

} /* employee_with_load_new() */

boolean employee_load(
		double *hourly_wage,
		double *period_salary,
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
"hourly_wage,period_salary,commission_sum_extension_percent,marital_status,withholding_allowances,withholding_additional_amount,retirement_contribution_plan_employee_period_amount,retirement_contribution_plan_employer_period_amount,health_insurance_employee_period_amount,health_insurance_employer_period_amount,union_dues_period_amount";

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

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );

	if ( *piece_buffer )
		*hourly_wage = atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 0 );

	if ( *piece_buffer )
		*hourly_wage = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 1 );

	if ( *piece_buffer )
		*period_salary = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 2 );

	if ( *piece_buffer )
		*commission_sum_extension_percent = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 3 );

	if ( *piece_buffer )
		*marital_status = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 4 );

	if ( *piece_buffer )
		*withholding_allowances = atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 5 );

	if ( *piece_buffer )
		*withholding_additional_amount = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 6 );

	if ( *piece_buffer )
		*retirement_contribution_plan_employee_period_amount =
			atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 7 );

	if ( *piece_buffer )
		*retirement_contribution_plan_employer_period_amount =
			atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 8 );

	if ( *piece_buffer )
		*health_insurance_employee_period_amount =
			atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 9 );

	if ( *piece_buffer )
		*health_insurance_employer_period_amount =
			atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 10 );

	if ( *piece_buffer )
		*union_dues_period_amount =
			atof( buffer );

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

void employee_work_day_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *begin_work_date_time,
			double employee_work_hours,
			double database_employee_work_hours )
{
	char *sys_string;
	FILE *output_pipe;

	if ( double_virtually_same(
			employee_work_hours,
			database_employee_work_hours ) )
	{
		return;
	}

	sys_string =
		employee_work_day_update_get_sys_string(
			application_name );

	output_pipe = popen( sys_string, "w" );

	fprintf(output_pipe,
		"%s^%s^%s^employee_work_hours^%.2lf\n",
		full_name,
		street_address,
		begin_work_date_time,
		employee_work_hours );

	pclose( output_pipe );

} /* employee_work_day_update() */

char *employee_work_day_update_get_sys_string(
				char *application_name )
{
	static char sys_string[ 256 ];
	char *table_name;
	char *key;

	table_name =
		get_table_name(
			application_name,
			"employee_work_day" );

	key =
"full_name,street_address,begin_work_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	return sys_string;

} /* employee_work_day_update_get_sys_string() */

EMPLOYEE_WORK_DAY *employee_work_day_seek(
				LIST *employee_work_day_list,
				char *begin_work_date_time )
{
	EMPLOYEE_WORK_DAY *employee_work_day;

	if ( !list_rewind( employee_work_day_list ) )
		return (EMPLOYEE_WORK_DAY *)0;

	do {
		employee_work_day = list_get( employee_work_day_list );

		if ( timlib_strcmp(	employee_work_day->begin_work_date_time,
					begin_work_date_time ) == 0 )
		{
			return employee_work_day;
		}

	} while( list_next( employee_work_day_list ) );

	return (EMPLOYEE_WORK_DAY *)0;

} /* employee_work_day_seek() */

LIST *employee_get_list(	char *application_name,
				char *begin_work_date )
{
	LIST *employee_list;
	EMPLOYEE *employee;
	char sys_string[ 1024 ];
	char *where;
	char *select;
	FILE *input_pipe;
	char full_name[ 128 ];
	char street_address[ 128 ];
	char input_buffer[ 256 ];

	employee_list = list_new();

	select = "full_name,street_address";
	where = "terminated_date is not null";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=employee		"
		 "			where=\"%s\"		"
		 "			order=select		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( timlib_get_line( input_buffer, input_pipe, 256 ) )
	{
		piece( full_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( street_address, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		employee =
			employee_with_load_new(
				application_name,
				strdup( full_name ),
				strdup( street_address ),
				begin_work_date );

		list_append_pointer( employee_list, employee );
	}

	pclose( input_pipe );

	return employee_list;

} /* employee_get_list() */

char *employee_get_begin_work_date( char *application_name )
{
	return (char *)0;

} /* employee_get_begin_work_date() */

