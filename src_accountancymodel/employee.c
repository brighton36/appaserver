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
#include "column.h"
#include "employee.h"
#include "customer.h"

LIST *employee_fetch_work_period_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date,
					char *end_work_date )
{
	EMPLOYEE_WORK_PERIOD *employee_work_period;
	LIST *employee_work_period_list;
	char *select;
	char sys_string[ 1024 ];
	char where[ 256 ];
	char input_buffer[ 512 ];
	char piece_buffer[ 128 ];
	int payroll_year;
	int payroll_period_number;
	char buffer[ 256 ];
	FILE *input_pipe;

	select =
"payroll_year,payroll_period_number,begin_work_date,end_work_date,employee_work_hours,commission_sum_extension,gross_pay,federal_tax_withholding_amount,state_tax_withholding_amount,social_security_employee_tax_amount,social_security_employer_tax_amount,medicare_employee_tax_amout,medicare_employer_tax_amount,retirement_contribution_plan_employee_amount,retirement_contribution_plan_employer_amount,health_insurance_employee_amount,health_insurance_employer_amount,federal_unemployment_tax_amount,state_unemployment_tax_amount,union_dues_amount,transaction_date_time";

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "begin_work_date >= '%s' and		"
		 "end_work_date <= '%s'			",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 (begin_work_date)
			? begin_work_date
			: "1900-01-01",
		 (end_work_date)
			? end_work_date
			: "2999-12-31" );

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

		payroll_year = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		payroll_period_number = atoi( piece_buffer );

		employee_work_period =
			employee_work_period_new(
				full_name,
				street_address,
				payroll_year,
				payroll_period_number );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		if ( *piece_buffer )
			employee_work_period->begin_work_date =
				strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		if ( *piece_buffer )
			employee_work_period->end_work_date =
				strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
		if ( *piece_buffer )
			employee_work_period->employee_work_hours =
				atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
		if ( *piece_buffer )
			employee_work_period->commission_sum_extension =
				atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
		if ( *piece_buffer )
			employee_work_period->gross_pay =
				atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
		if ( *piece_buffer )
			employee_work_period->
				federal_tax_withholding_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
		if ( *piece_buffer )
			employee_work_period->
				state_tax_withholding_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
		if ( *piece_buffer )
			employee_work_period->
				social_security_employee_tax_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 10 );
		if ( *piece_buffer )
			employee_work_period->
				social_security_employer_tax_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 11 );
		if ( *piece_buffer )
			employee_work_period->
				medicare_employee_tax_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 12 );
		if ( *piece_buffer )
			employee_work_period->
				medicare_employer_tax_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 13 );
		if ( *piece_buffer )
			employee_work_period->
				retirement_contribution_plan_employee_amount =
					atoi( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 14 );
		if ( *piece_buffer )
			employee_work_period->
				retirement_contribution_plan_employer_amount =
					atoi( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 15 );
		if ( *piece_buffer )
			employee_work_period->
				health_insurance_employee_amount =
					atoi( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 16 );
		if ( *piece_buffer )
			employee_work_period->
				health_insurance_employer_amount =
					atoi( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 17 );
		if ( *piece_buffer )
			employee_work_period->
				federal_unemployment_tax_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 18 );
		if ( *piece_buffer )
			employee_work_period->
				state_unemployment_tax_amount =
					atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 19 );
		if ( *piece_buffer )
			employee_work_period->
				union_dues_amount =
					atoi( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 20 );
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

} /* employee_fetch_work_period_list() */

LIST *employee_fetch_work_day_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date,
					char *end_work_date )
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
	char end_work_date_time[ 32 ];

	sprintf( end_work_date_time,
		 "%s 23:59:59",
		 (end_work_date)
			? end_work_date
			: "2999-12-31" );

	select =
"substr(begin_work_date_time,1,16),substr(end_work_date_time,1,16),employee_work_hours";

	sprintf(where,
	 	"full_name = '%s' and				"
	 	"street_address = '%s' and			"
	 	"begin_work_date_time between '%s' and '%s'	",
	 	escape_character(	buffer,
					full_name,
					'\'' ),
	 	street_address,
	 	(begin_work_date)
			? begin_work_date
			: "1999-01-01",
		end_work_date_time );

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
						begin_work_date_time,
					date_get_utc_offset() );

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
						end_work_date_time,
					date_get_utc_offset() );

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

} /* employee_fetch_work_day_list() */

EMPLOYEE_WORK_PERIOD *employee_work_period_new(
					char *full_name,
					char *street_address,
					int payroll_year,
					int payroll_period_number )
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

	e->full_name = full_name;
	e->street_address = street_address;
	e->payroll_year = payroll_year;
	e->payroll_period_number = payroll_period_number;
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
					char *begin_work_date,
					char *end_work_date )
{
	EMPLOYEE *e;

	e = employee_new( full_name, street_address );

	if ( !employee_load(
		&e->hourly_wage,
		&e->period_salary,
		&e->commission_sum_extension_percent,
		&e->gross_pay_year_to_date,
		&e->database_gross_pay_year_to_date,
		&e->federal_marital_status,
		&e->federal_withholding_allowances,
		&e->federal_withholding_additional_period_amount,
		&e->state_marital_status,
		&e->state_withholding_allowances,
		&e->state_itemized_deduction_allowances,
		&e->retirement_contribution_plan_employee_period_amount,
		&e->retirement_contribution_plan_employer_period_amount,
		&e->health_insurance_employee_period_amount,
		&e->health_insurance_employer_period_amount,
		&e->union_dues_period_amount,
		&e->employee_work_day_list,
		&e->employee_work_period_list,
		&e->customer_sale_list,
		application_name,
		full_name,
		street_address,
		begin_work_date,
		end_work_date ) )
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
		double *gross_pay_year_to_date,
		double *database_gross_pay_year_to_date,
		char **federal_marital_status,
		int *federal_withholding_allowances,
		int *federal_withholding_additional_period_amount,
		char **state_marital_status,
		int *state_withholding_allowances,
		int *state_itemized_deduction_allowances,
		int *retirement_contribution_plan_employee_period_amount,
		int *retirement_contribution_plan_employer_period_amount,
		int *health_insurance_employee_period_amount,
		int *health_insurance_employer_period_amount,
		int *union_dues_period_amount,
		LIST **employee_work_day_list,
		LIST **employee_work_period_list,
		LIST **customer_sale_list,
		char *application_name,
		char *full_name,
		char *street_address,
		char *begin_work_date,
		char *end_work_date )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *select;
	char *results;
	char buffer[ 512 ];
	char piece_buffer[ 128 ];
	char *representative_full_name_attribute = "mechanic_full_name";
	char *representative_street_name_attribute = "mechanic_street_address";

	select =
"hourly_wage,period_salary,commission_sum_extension_percent,gross_pay_year_to_date,federal_marital_status,federal_withholding_allowances,federal_withholding_additional_period_amount,state_marital_status,state_withholding_allowances,state_itemized_deduction_allowances,retirement_contribution_plan_employee_period_amount,retirement_contribution_plan_employer_period_amount,health_insurance_employee_period_amount,health_insurance_employer_period_amount,union_dues_period_amount";

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
		*gross_pay_year_to_date =
		*database_gross_pay_year_to_date = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 4 );
	if ( !*buffer )
	{
		*federal_marital_status = DEFAULT_FEDERAL_MARITAL_STATUS;
	}
	else
	{
		*federal_marital_status = strdup( buffer );
	}

	piece( buffer, FOLDER_DATA_DELIMITER, results, 5 );
	if ( *piece_buffer )
		*federal_withholding_allowances = atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 6 );
	if ( *piece_buffer )
		*federal_withholding_additional_period_amount = atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 7 );
	if ( !*buffer )
	{
		*state_marital_status = DEFAULT_STATE_MARITAL_STATUS;
	}
	else
	{
		*state_marital_status = strdup( buffer );
	}

	piece( buffer, FOLDER_DATA_DELIMITER, results, 8 );
	if ( *piece_buffer )
		*state_withholding_allowances = atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 9 );
	if ( *piece_buffer )
		*state_itemized_deduction_allowances = atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 10 );
	if ( *piece_buffer )
		*retirement_contribution_plan_employee_period_amount =
			atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 11 );
	if ( *piece_buffer )
		*retirement_contribution_plan_employer_period_amount =
			atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 12 );
	if ( *piece_buffer )
		*health_insurance_employee_period_amount =
			atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 13 );
	if ( *piece_buffer )
		*health_insurance_employer_period_amount =
			atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 14 );
	if ( *piece_buffer )
		*union_dues_period_amount =
			atoi( buffer );

	free( results );

	*employee_work_day_list =
		employee_fetch_work_day_list(
			application_name,
			full_name,
			street_address,
			begin_work_date,
			end_work_date );

	*employee_work_period_list =
		employee_fetch_work_period_list(
			application_name,
			full_name,
			street_address,
			begin_work_date,
			end_work_date );

	*customer_sale_list =
		customer_sale_fetch_commission_list(
			application_name,
			begin_work_date,
			end_work_date,
			full_name /* sales_representative_full_name */,
			street_address
				/* sales_representative_street_address */,
			representative_full_name_attribute,
			representative_street_name_attribute );

	return 1;

} /* employee_load() */

void employee_update(	char *application_name,
			char *full_name,
			char *street_address,
			double gross_pay_year_to_date,
			double database_gross_pay_year_to_date )
{
	char *sys_string;
	FILE *output_pipe;

	if ( timlib_dollar_virtually_same(
			gross_pay_year_to_date,
			database_gross_pay_year_to_date ) )
	{
		return;
	}

	sys_string =
		employee_update_get_sys_string(
			application_name );

	output_pipe = popen( sys_string, "w" );

	fprintf(output_pipe,
		"%s^%s^gross_pay_year_to_date^%.2lf\n",
		full_name,
		street_address,
		gross_pay_year_to_date );

	pclose( output_pipe );

} /* employee_update() */

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

char *employee_update_get_sys_string(
				char *application_name )
{
	static char sys_string[ 256 ];
	char *table_name;
	char *key;

	table_name =
		get_table_name(
			application_name,
			"employee" );

	key =
"full_name,street_address";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	return sys_string;

} /* employee_update_get_sys_string() */

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

LIST *employee_fetch_list(	char *application_name,
				char *begin_work_date,
				char *end_work_date )
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
				begin_work_date,
				end_work_date );

		list_append_pointer( employee_list, employee );
	}

	pclose( input_pipe );

	return employee_list;

} /* employee_fetch_list() */

boolean employee_get_payroll_begin_end_work_dates(
					char **payroll_begin_work_date,
					char **payroll_end_work_date,
					char *payroll_pay_period_string,
					char *include_date )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 128 ];
	char column_buffer[ 128 ];
	int results = 0;

	sprintf( sys_string,
		 "payroll_period.e"
		 " period=%s"
		 " date=%s",
		 payroll_pay_period_string,
		 include_date );

	input_pipe = popen( sys_string, "r" );

	while( timlib_get_line( input_buffer, input_pipe, 128 ) )
	{
		if ( timlib_strncmp(	input_buffer,
					PAYROLL_BEGIN_DATE_LABEL ) == 0 )
		{
			column( column_buffer, 1, input_buffer );
			*payroll_begin_work_date = strdup( column_buffer );
			results++;
			continue;
		}

		if ( timlib_strncmp(	input_buffer,
					PAYROLL_END_DATE_LABEL ) == 0 )
		{
			column( column_buffer, 1, input_buffer );
			*payroll_end_work_date = strdup( column_buffer );
			results++;
			continue;
		}
	}

	pclose( input_pipe );

	return (boolean)(results == 2);

} /* employee_get_payroll_begin_end_work_dates() */

boolean employee_get_prior_period(
				char **begin_work_date,
				char **end_work_date,
				int *payroll_year,
				int *payroll_period_number,
				enum payroll_pay_period
					payroll_pay_period )
{
	char *payroll_pay_period_string;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 128 ];
	char column_buffer[ 128 ];
	int results = 0;

	payroll_pay_period_string =
		entity_get_payroll_pay_period_string(
			payroll_pay_period );

	sprintf( sys_string,
		 "payroll_period.e"
		 " period=%s"
		 " prior=yes",
		 payroll_pay_period_string );

	input_pipe = popen( sys_string, "r" );

	while( timlib_get_line( input_buffer, input_pipe, 128 ) )
	{
		if ( timlib_strncmp(	input_buffer,
					PAYROLL_BEGIN_DATE_LABEL ) == 0 )
		{
			column( column_buffer, 1, input_buffer );
			*begin_work_date = strdup( column_buffer );
			results++;
			continue;
		}

		if ( timlib_strncmp(	input_buffer,
					PAYROLL_END_DATE_LABEL ) == 0 )
		{
			column( column_buffer, 1, input_buffer );
			*end_work_date = strdup( column_buffer );
			*payroll_year = atoi( column_buffer );
			results++;
			continue;
		}

		if ( timlib_strncmp(	input_buffer,
					PAYROLL_PERIOD_NUMBER_LABEL ) == 0 )
		{
			column( column_buffer, 1, input_buffer );
			*payroll_period_number = atoi( column_buffer );
			results++;
			continue;
		}
	}

	pclose( input_pipe );

	return (boolean)(results == 3);

} /* employee_get_prior_period() */

PAYROLL_POSTING *employee_payroll_posting_new(
					int payroll_year,
					int payroll_period_number,
					char *begin_work_date,
					char *end_work_date )
{
	PAYROLL_POSTING *e;

	if ( ! ( e = calloc( 1, sizeof( PAYROLL_POSTING ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	e->payroll_year = payroll_year;
	e->payroll_period_number = payroll_period_number;
	e->begin_work_date = begin_work_date;
	e->end_work_date = end_work_date;

	return e;

} /* employee_payroll_posting_new() */

PAYROLL_POSTING *employee_get_payroll_posting(
				LIST *employee_list,
				char *application_name,
				int payroll_year,
				int payroll_period_number,
				char *begin_work_date,
				char *end_work_date,
				ENTITY_SELF *self,
				EMPLOYEE_TAX_WITHHOLDING_TABLE *
					employee_tax_withholding_table )
{
	PAYROLL_POSTING *payroll_posting;

	payroll_posting =
		employee_payroll_posting_new(
			payroll_year,
			payroll_period_number,
			begin_work_date,
			end_work_date );

	payroll_posting->employee_work_period_list =
		employee_posting_calculate_work_period_list(
			&payroll_posting->employee_work_hours,
			&payroll_posting->commission_sum_extension,
			&payroll_posting->gross_pay,
			&payroll_posting->federal_tax_withholding_amount,
			&payroll_posting->state_tax_withholding_amount,
			&payroll_posting->social_security_employee_tax_amount,
			&payroll_posting->social_security_employer_tax_amount,
			&payroll_posting->medicare_employee_tax_amount,
			&payroll_posting->medicare_employer_tax_amount,
			&payroll_posting->
				retirement_contribution_plan_employee_amount,
			&payroll_posting->
				retirement_contribution_plan_employer_amount,
			&payroll_posting->health_insurance_employee_amount,
			&payroll_posting->health_insurance_employer_amount,
			&payroll_posting->federal_unemployment_tax_amount,
			&payroll_posting->state_unemployment_tax_amount,
			&payroll_posting->union_dues_amount,
			employee_list,
			application_name,
			payroll_year,
			payroll_period_number,
			begin_work_date,
			end_work_date,
			self,
			employee_tax_withholding_table );

	return payroll_posting;

} /* employee_get_payroll_posting() */

LIST *employee_posting_calculate_work_period_list(
			double *employee_work_hours,
			double *commission_sum_extension,
			double *gross_pay,
			double *federal_tax_withholding_amount,
			double *state_tax_withholding_amount,
			double *social_security_employee_tax_amount,
			double *social_security_employer_tax_amount,
			double *medicare_employee_tax_amount,
			double *medicare_employer_tax_amount,
			int *retirement_contribution_plan_employee_amount,
			int *retirement_contribution_plan_employer_amount,
			int *health_insurance_employee_amount,
			int *health_insurance_employer_amount,
			double *federal_unemployment_tax_amount,
			double *state_unemployment_tax_amount,
			int *union_dues_amount,
			LIST *employee_list,
			char *application_name,
			int payroll_year,
			int payroll_period_number,
			char *begin_work_date,
			char *end_work_date,
			ENTITY_SELF *self,
			EMPLOYEE_TAX_WITHHOLDING_TABLE *
				employee_tax_withholding_table )
{
	LIST *employee_work_period_list;
	EMPLOYEE *employee;
	EMPLOYEE_WORK_PERIOD *employee_work_period;

	if ( !list_rewind( employee_list ) ) return (LIST *)0;

	employee_work_period_list = list_new();

	do {
		employee = list_get_pointer( employee_list );

		employee_work_period =
			employee_get_work_period(
			  application_name,
			  employee->employee_work_day_list,
			  employee->customer_sale_list,
			  employee->hourly_wage,
			  employee->period_salary,
			  employee->commission_sum_extension_percent,
			  employee->federal_marital_status,
			  employee->federal_withholding_allowances,
			  employee->
				federal_withholding_additional_period_amount,
			  employee->state_marital_status,
			  employee->state_withholding_allowances,
			  employee->state_itemized_deduction_allowances,
			  employee->
			    retirement_contribution_plan_employee_period_amount,
			  employee->
			    retirement_contribution_plan_employer_period_amount,
			  employee->
			  	health_insurance_employee_period_amount,
			  employee->
			  	health_insurance_employer_period_amount,
			  employee->
			  	union_dues_period_amount,
			  employee->full_name,
			  employee->street_address,
			  payroll_year,
			  payroll_period_number,
			  begin_work_date,
			  end_work_date,
			  self,
			  employee_tax_withholding_table );

		list_append_pointer(
			employee->employee_work_period_list,
			employee_work_period );

		list_append_pointer(
			employee_work_period_list,
			employee_work_period );

		*employee_work_hours +=
			employee_work_period->
				employee_work_hours;

		*commission_sum_extension +=
			employee_work_period->
				commission_sum_extension;

		*gross_pay +=
			employee_work_period->
				gross_pay;

		*federal_tax_withholding_amount +=
			employee_work_period->
				federal_tax_withholding_amount;

		*state_tax_withholding_amount +=
			employee_work_period->
				state_tax_withholding_amount;

		*social_security_employee_tax_amount +=
			employee_work_period->
				social_security_employee_tax_amount;

		*social_security_employer_tax_amount +=
			employee_work_period->
				social_security_employer_tax_amount;

		*medicare_employee_tax_amount +=
			employee_work_period->
				medicare_employee_tax_amount;

		*medicare_employer_tax_amount +=
			employee_work_period->
				medicare_employer_tax_amount;

		*retirement_contribution_plan_employee_amount +=
			employee_work_period->
				retirement_contribution_plan_employee_amount;

		*retirement_contribution_plan_employer_amount +=
			employee_work_period->
				retirement_contribution_plan_employer_amount;

		*health_insurance_employee_amount +=
			employee_work_period->
				health_insurance_employee_amount;

		*health_insurance_employer_amount +=
			employee_work_period->
				health_insurance_employer_amount;

		*federal_unemployment_tax_amount +=
			employee_work_period->
				federal_unemployment_tax_amount;

		*state_unemployment_tax_amount +=
			employee_work_period->
				state_unemployment_tax_amount;

		*union_dues_amount +=
			employee_work_period->
				union_dues_amount;

	} while( list_next( employee_list ) );

	return employee_work_period_list;

} /* employee_posting_calculate_work_period_list() */

double employee_calculate_commission_sum_extension(
			LIST *customer_sale_list )
{
	CUSTOMER_SALE *customer_sale;
	double commission_sum_extension;

	if ( !list_rewind( customer_sale_list ) ) return 0.0;

	commission_sum_extension = 0.0;

	do {
		customer_sale = list_get_pointer( customer_sale_list );

		commission_sum_extension +=
			customer_sale->
				sum_extension;

	} while( list_next( customer_sale_list ) );

	return commission_sum_extension;

} /* employee_calculate_commission_sum_extension() */

double employee_calculate_employee_work_hours(
			LIST *employee_work_day_list )
{
	EMPLOYEE_WORK_DAY *employee_work_day;
	double employee_work_hours;

	if ( !list_rewind( employee_work_day_list ) ) return 0.0;

	employee_work_hours = 0.0;

	do {
		employee_work_day = list_get_pointer( employee_work_day_list );

		employee_work_hours += employee_work_day->employee_work_hours;

	} while( list_next( employee_work_day_list ) );

	return employee_work_hours;

} /* employee_calculate_employee_work_hours() */

EMPLOYEE_STATE_STANDARD_DEDUCTION *employee_state_standard_deduction_new(
				char *state_marital_status,
				int state_withholding_allowances,
				double state_standard_deduction_amount )
{
	EMPLOYEE_STATE_STANDARD_DEDUCTION *e;

	if ( ! ( e = calloc(	1,
				sizeof(
				     EMPLOYEE_STATE_STANDARD_DEDUCTION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	e->state_marital_status = state_marital_status;
	e->state_withholding_allowances = state_withholding_allowances;
	e->state_standard_deduction_amount = state_standard_deduction_amount;
	return e;

} /* employee_state_standard_deduction_new() */

EMPLOYEE_MARITAL_STATUS_WITHHOLDING *employee_marital_status_withholding_new(
				char *marital_status )
{
	EMPLOYEE_MARITAL_STATUS_WITHHOLDING *e;

	if ( ! ( e = calloc(	1,
				sizeof(
				     EMPLOYEE_MARITAL_STATUS_WITHHOLDING ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	e->marital_status = marital_status;
	return e;

} /* employee_marital_status_withholding_new() */

EMPLOYEE_TAX_WITHHOLDING_TABLE *employee_tax_withholding_table_new(
				char *application_name )
{
	EMPLOYEE_TAX_WITHHOLDING_TABLE *e;
	LIST *federal_marital_status_string_list;
	LIST *state_marital_status_string_list;

	if ( ! ( e = calloc( 1, sizeof( EMPLOYEE_TAX_WITHHOLDING_TABLE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	federal_marital_status_string_list =
		employee_fetch_marital_status_string_list(
			application_name,
			"federal" );

	if ( list_length( federal_marital_status_string_list ) )
	{
		e->federal_marital_status_list =
			employee_fetch_marital_status_list(
				application_name,
				federal_marital_status_string_list,
				"federal" );
	}

	state_marital_status_string_list =
		employee_fetch_marital_status_string_list(
			application_name,
			"state" );

	if ( list_length( state_marital_status_string_list ) )
	{
		e->state_marital_status_list =
			employee_fetch_marital_status_list(
				application_name,
				state_marital_status_string_list,
				"state" );
	}

	e->state_standard_deduction_list =
		employee_fetch_state_standard_deduction_list(
			application_name );

	return e;

} /* employee_tax_withholding_table_new() */

EMPLOYEE_INCOME_TAX_WITHHOLDING *employee_income_tax_withholding_new(
					int income_over )
{
	EMPLOYEE_INCOME_TAX_WITHHOLDING *e;

	if ( ! ( e = calloc( 1, sizeof( EMPLOYEE_INCOME_TAX_WITHHOLDING ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	e->income_over = income_over;
	return e;

} /* employee_income_tax_withholding_new() */

LIST *employee_fetch_marital_status_string_list(
				char *application_name,
				char *federal_or_state )
{
	char folder_name[ 128 ];
	char select[ 128 ];
	char sys_string[ 1024 ];

	sprintf( select,
		 "%s_marital_status",
		 federal_or_state );

	sprintf( folder_name,
		 "%s_marital_status",
		 federal_or_state );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		",
		 application_name,
		 select,
		 folder_name );

	return pipe2list( sys_string );

} /* employee_fetch_marital_status_string_list() */

LIST *employee_fetch_state_standard_deduction_list(
				char *application_name )
{
	EMPLOYEE_STATE_STANDARD_DEDUCTION *e;
	char *folder_name;
	char *select;
	LIST *state_standard_deduction_list;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char state_marital_status[ 128 ];
	char state_withholding_allowances[ 128 ];
	char state_standard_deduction_amount[ 128 ];

	select =
"state_marital_status,state_withholding_allowances,state_standard_deduction_amount";

	folder_name = "state_standard_deduction_table";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		",
		 application_name,
		 select,
		 folder_name );

	input_pipe = popen( sys_string, "r" );

	state_standard_deduction_list = list_new();

	while( timlib_get_line( input_buffer, input_pipe, 512 ) )
	{
		piece(	state_marital_status,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	state_withholding_allowances,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	state_standard_deduction_amount,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		e = employee_state_standard_deduction_new(
			strdup( state_marital_status ),
			atoi( state_withholding_allowances ),
			atof( state_standard_deduction_amount ) );

		list_append_pointer( state_standard_deduction_list, e );
	}

	pclose( input_pipe );
	return state_standard_deduction_list;

} /* employee_fetch_state_standard_deduction_list() */

double employee_get_state_standard_deduction_amount(
				LIST *state_standard_deduction_list,
				char *state_marital_status,
				int state_withholding_allowances )
{
	EMPLOYEE_STATE_STANDARD_DEDUCTION *e;

	if ( !state_withholding_allowances ) return 0.0;

	if ( !list_rewind( state_standard_deduction_list ) ) return 0.0;

	do {
		e = list_get_pointer( state_standard_deduction_list );

		if ( strcmp(	state_marital_status,
				e->state_marital_status ) == 0
		&&   state_withholding_allowances ==
		     e->state_withholding_allowances )
		{
			return e->state_standard_deduction_amount;
		}

	} while( list_next( state_standard_deduction_list ) );

	fprintf( stderr,
"Warning in %s/%s()/%d: cannot find (%s/%d) in state_standard_deduction_list.\n",
		 __FILE__,
		 __FUNCTION__,
		 __LINE__,
		 state_marital_status,
		 state_withholding_allowances );

	return 0.0;

} /* employee_get_state_standard_deduction_amount() */

LIST *employee_fetch_marital_status_list(
				char *application_name,
				LIST *marital_status_string_list,
				char *federal_or_state )
{
	char *marital_status_string;
	LIST *marital_status_list;
	EMPLOYEE_MARITAL_STATUS_WITHHOLDING *e;

	if ( !list_rewind( marital_status_string_list ) ) return (LIST *)0;

	marital_status_list = list_new();

	do {
		marital_status_string =
			list_get_pointer(
				marital_status_string_list );

		e = employee_marital_status_withholding_new(
				marital_status_string );

		e->income_tax_withholding_list =
			employee_fetch_income_tax_withholding_list(
				application_name,
				federal_or_state,
				marital_status_string );

	} while( list_next( marital_status_string_list ) );

	return marital_status_list;

} /* employee_fetch_marital_status_list() */

LIST *employee_fetch_income_tax_withholding_list(
				char *application_name,
				char *federal_or_state,
				char *marital_status_string )
{
	char folder_name[ 128 ];
	char *select;
	char where[ 128 ];
	char *order;
	LIST *tax_withholding_list;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char piece_buffer[ 128 ];
	EMPLOYEE_INCOME_TAX_WITHHOLDING *e;

	select =
"income_over,income_not_over,tax_fixed_amount,tax_percentage_amount";

	sprintf( folder_name,
		 "%s_income_tax_withholding",
		 federal_or_state );

	sprintf( where,
		 "%s_marital_status = '%s'",
		 federal_or_state,
		 marital_status_string );

	order = "income_over";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			order=%s		",
		 application_name,
		 select,
		 folder_name,
		 order );

	input_pipe = popen( sys_string, "r" );

	tax_withholding_list = list_new();

	while( timlib_get_line( input_buffer, input_pipe, 512 ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );

		e = employee_income_tax_withholding_new( atoi( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		e->income_not_over = atoi( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		e->tax_fixed_amount = atoi( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		e->tax_percentage_amount = atoi( piece_buffer );

		list_append_pointer( tax_withholding_list, e );
	}

	pclose( input_pipe );
	return tax_withholding_list;

} /* employee_fetch_income_tax_withholding_list() */

double employee_calculate_state_taxable_income(
				double gross_pay,
				double state_standard_deduction_amount,
				double state_itemized_deduction_amount )
{
	double taxable_income;

	taxable_income =
		gross_pay -
		( state_standard_deduction_amount +
		  state_itemized_deduction_amount );

	return taxable_income;

} /* employee_calculate_state_taxable_income() */

double employee_calculate_federal_taxable_income(
				double gross_pay,
				int withholding_allowances,
				double withholding_allowance_period_value )
{
	double taxable_income;

	taxable_income =
		gross_pay -
		( (double)withholding_allowances *
		  withholding_allowance_period_value );

	return taxable_income;

} /* employee_calculate_federal_taxable_income() */

double employee_calculate_computed_tax(
				double taxable_income,
				char *marital_status_string,
				LIST *marital_status_list )
{
	EMPLOYEE_MARITAL_STATUS_WITHHOLDING *marital_status_withholding;
	EMPLOYEE_INCOME_TAX_WITHHOLDING *e;
	LIST *tax_withholding_list;
	double excess_over;
	double withholding_amount = 0.0;

	if ( !list_rewind( marital_status_list ) ) return 0.0;

	do {
		marital_status_withholding =
			list_get_pointer(
				marital_status_list );

		if ( strcmp(	marital_status_withholding->marital_status,
				marital_status_string ) != 0 )
		{
			continue;
		}

		tax_withholding_list =
			marital_status_withholding->
				income_tax_withholding_list;

		e = list_get_first_pointer( tax_withholding_list );
	
		if ( taxable_income <= (double)e->income_over )
			return 0.0;
	
		do {
			e = list_get_pointer( tax_withholding_list );
	
			if ( !e->income_not_over
			|| ( (	taxable_income >
				(double)e->income_over
			&&     taxable_income <=
				(double)e->income_not_over ) ) )
			{
				excess_over =	taxable_income -
						(double)e->income_over;
	
				withholding_amount =
					e->tax_fixed_amount +
					( excess_over *
					( e->tax_percentage_amount / 100.0 ) );
	
				return withholding_amount;
			}
	
		} while( list_next( tax_withholding_list ) );

	} while( list_next( marital_status_list ) );

	return 0.0;

} /* employee_calculate_computed_tax() */

double employee_calculate_federal_tax_withholding_amount(
			char *federal_marital_status,
			int federal_withholding_allowances,
			double federal_withholding_allowance_period_value,
			double gross_pay,
			LIST *federal_marital_status_list )
{
	double taxable_income;
	double federal_tax_withholding_amount;

	taxable_income =
		employee_calculate_federal_taxable_income(
			gross_pay,
			federal_withholding_allowances,
			federal_withholding_allowance_period_value );

	federal_tax_withholding_amount =
		employee_calculate_computed_tax(
			taxable_income,
			federal_marital_status,
			federal_marital_status_list );

	return federal_tax_withholding_amount;

} /* employee_calculate_federal_tax_withholding_amount() */

double employee_calculate_state_tax_withholding_amount(
			char *state_marital_status,
			int state_withholding_allowances,
			int state_itemized_deduction_allowances,
			double state_withholding_allowance_period_value,
			double state_itemized_allowance_period_value,
			double gross_pay,
			LIST *state_marital_status_list,
			LIST *state_standard_deduction_list )
{
	double state_itemized_deduction_amount = 0.0;
	double state_standard_deduction_amount;
	double taxable_income;
	double computed_tax;
	double state_exemption_withholding_amount;
	double state_tax_withholding_amount;

	if ( state_itemized_deduction_allowances )
	{
		state_itemized_deduction_amount =
			(double)state_itemized_deduction_allowances *
			state_itemized_allowance_period_value;
	}

	state_standard_deduction_amount =
		employee_get_state_standard_deduction_amount(
				state_standard_deduction_list,
				state_marital_status,
				state_withholding_allowances );

	taxable_income =
		employee_calculate_state_taxable_income(
				gross_pay,
				state_standard_deduction_amount,
				state_itemized_deduction_amount );

	computed_tax =
		employee_calculate_computed_tax(
			taxable_income,
			state_marital_status,
			state_marital_status_list );

	state_exemption_withholding_amount =
		(double)state_withholding_allowances *
		state_withholding_allowance_period_value;

	state_tax_withholding_amount =
		computed_tax -
		state_exemption_withholding_amount;

	return state_tax_withholding_amount;

} /* employee_calculate_state_tax_withholding_amount() */

EMPLOYEE_WORK_PERIOD *employee_get_work_period(
		char *application_name,
		LIST *employee_work_day_list,
		LIST *customer_sale_list,
		double hourly_wage,
		double period_salary,
		double commission_sum_extension_percent,
		char *federal_marital_status,
		int federal_withholding_allowances,
		int federal_withholding_additional_period_amount,
		char *state_marital_status,
		int state_withholding_allowances,
		int state_itemized_deduction_allowances,
		int retirement_contribution_plan_employee_period_amount,
		int retirement_contribution_plan_employer_period_amount,
		int health_insurance_employee_period_amount,
		int health_insurance_employer_period_amount,
		int union_dues_period_amount,
		char *full_name,
		char *street_address,
		int payroll_year,
		int payroll_period_number,
		char *begin_work_date,
		char *end_work_date,
		ENTITY_SELF *self,
		EMPLOYEE_TAX_WITHHOLDING_TABLE *
			employee_tax_withholding_table )
{
	EMPLOYEE_WORK_PERIOD *employee_work_period;

	employee_work_period =
		employee_work_period_new(
			full_name,
			street_address,
			payroll_year,
			payroll_period_number );

	employee_work_period->begin_work_date = begin_work_date;
	employee_work_period->end_work_date = end_work_date;

	/* employee_work_hours */
	/* ------------------- */
	employee_work_period->employee_work_hours =
		employee_calculate_employee_work_hours(
			employee_work_day_list );

	/* commission_sum_extension */
	/* ------------------------ */
	employee_work_period->commission_sum_extension =
		employee_calculate_commission_sum_extension(
			customer_sale_list );

	/* gross_pay */
	/* --------- */
	if ( hourly_wage )
	{
		employee_work_period->gross_pay =
			employee_work_period->employee_work_hours *
			hourly_wage;
	}

	if ( period_salary )
	{
		employee_work_period->gross_pay += period_salary;
	}

	if ( employee_work_period->commission_sum_extension
	&&   commission_sum_extension_percent )
	{
		double commission_sum_extension_ratio;

		commission_sum_extension_ratio =
			commission_sum_extension_percent / 100.0;

		employee_work_period->gross_pay +=
			employee_work_period->commission_sum_extension *
			commission_sum_extension_ratio;
	}

	/* federal_tax_withholding_amount */
	/* ------------------------------ */
	employee_work_period->federal_tax_withholding_amount =
		employee_calculate_federal_tax_withholding_amount(
			federal_marital_status,
			federal_withholding_allowances,
			self->federal_withholding_allowance_period_value,
			employee_work_period->gross_pay,
			employee_tax_withholding_table->
				federal_marital_status_list );

	/* state_tax_withholding_amount */
	/* ---------------------------- */
	employee_work_period->state_tax_withholding_amount =
		employee_calculate_state_tax_withholding_amount(
			state_marital_status,
			state_withholding_allowances,
			state_itemized_deduction_allowances,
			self->state_withholding_allowance_period_value,
			self->state_itemized_allowance_period_value,
			employee_work_period->gross_pay,
			employee_tax_withholding_table->
				state_marital_status_list,
			employee_tax_withholding_table->
				state_standard_deduction_list );

	return employee_work_period;

} /* employee_get_work_period() */

