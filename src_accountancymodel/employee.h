/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/employee.h			*/
/* -------------------------------------------------------------------- */
/* This is the appaserver employee ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "list.h"
#include "ledger.h"
#include "entity.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* See also $APPASERVER_HOME/utility/payroll_period.c */
/* -------------------------------------------------- */
#define PAYROLL_PERIOD_NUMBER_LABEL	".payroll_period_number"
#define PAYROLL_BEGIN_DATE_LABEL	".payroll_begin_date"
#define PAYROLL_END_DATE_LABEL		".payroll_end_date"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	int payroll_year;
	int payroll_period_number;
	char *begin_work_date;
	char *end_work_date;
	double employee_work_hours;
	double gross_pay;
	double federal_tax_withholding_amount;
	double state_tax_withholding_amount;
	double social_security_employee_tax_amount;
	double social_security_employer_tax_amount;
	double medicare_employee_tax_amount;
	double medicare_employer_tax_amount;
	int retirement_contribution_plan_employee_amount;
	int retirement_contribution_plan_employer_amount;
	int health_insurance_employee_amount;
	int health_insurance_employer_amount;
	double federal_unemployment_tax_amount;
	double state_unemployment_tax_amount;
	int union_dues_amount;
	char *transaction_date_time;
	TRANSACTION *transaction;
} EMPLOYEE_WORK_PERIOD;

typedef struct
{
	int payroll_year;
	int payroll_period_number;
	char *begin_work_date;
	char *end_work_date;
	double employee_work_hours;
	double gross_pay;
	double federal_tax_withholding_amount;
	double state_tax_withholding_amount;
	double social_security_employee_tax_amount;
	double social_security_employer_tax_amount;
	double medicare_employee_tax_amount;
	double medicare_employer_tax_amount;
	int retirement_contribution_plan_employee_amount;
	int retirement_contribution_plan_employer_amount;
	int health_insurance_employee_amount;
	int health_insurance_employer_amount;
	double federal_unemployment_tax_amount;
	double state_unemployment_tax_amount;
	int union_dues_amount;
	LIST *employee_work_period_list;
} PAYROLL_POSTING;

typedef struct
{
	char *begin_work_date_time;
	char *end_work_date_time;
	double employee_work_hours;
	double database_employee_work_hours;
} EMPLOYEE_WORK_DAY;

typedef struct
{
	char *full_name;
	char *street_address;
	double hourly_wage;
	double period_salary;
	double commission_sum_extension_percent;
	char *marital_status;
	int withholding_allowances;
	int withholding_additional_period_amount;
	int retirement_contribution_plan_employee_period_amount;
	int retirement_contribution_plan_employer_period_amount;
	int health_insurance_employee_period_amount;
	int health_insurance_employer_period_amount;
	int union_dues_period_amount;
	char *terminated_date;
	LIST *employee_work_day_list;
	LIST *employee_work_period_list;
} EMPLOYEE;

/* Operations */
/* ---------- */
LIST *employee_get_work_day_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date,
					char *end_work_date );

EMPLOYEE *employee_new(			char *full_name,
					char *street_address );

boolean employee_load(
		double *hourly_wage,
		double *period_salary,
		double *commission_sum_extension_percent,
		char **marital_status,
		int *withholding_allowances,
		int *withholding_additional_period_amount,
		int *retirement_contribution_plan_employee_period_amount,
		int *retirement_contribution_plan_employer_period_amount,
		int *health_insurance_employee_period_amount,
		int *health_insurance_employer_period_amount,
		int *union_dues_period_amount,
		LIST **employee_work_day_list,
		LIST **employee_work_period_list,
		char *application_name,
		char *full_name,
		char *street_address,
		char *begin_work_date,
		char *end_work_date );

EMPLOYEE_WORK_DAY *employee_work_day_new(
					char *begin_work_date_time );

EMPLOYEE_WORK_PERIOD *employee_work_period_new(
					char *full_name,
					char *street_address,
					int payroll_year,
					int payroll_period_number );

PAYROLL_POSTING *employee_payroll_posting_new(
					int payroll_year,
					int payroll_period_number,
					char *begin_work_date,
					char *end_work_date );

LIST *employee_get_payroll_posting_work_period_list(
					char *application_name,
					int payroll_year,
					int payroll_period_number );

LIST *employee_get_employee_work_period_list(
					char *application_name,
					char *full_name,
					char *street_address );

char *employee_work_day_update_get_sys_string(
					char *application_name );

void employee_work_day_update(		char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date_time,
					double employee_work_hours,
					double database_employee_work_hours );

EMPLOYEE *employee_with_load_new(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date,
					char *end_work_date );

EMPLOYEE_WORK_DAY *employee_work_day_seek(
					LIST *employee_work_day_list,
					char *begin_work_date_time );

LIST *employee_get_list(		char *application_name,
					char *begin_work_date,
					char *end_work_date );

boolean employee_get_prior_period(
					char **begin_work_date,
					char **end_work_date,
					int *payroll_year,
					int *period_number,
					enum payroll_pay_period );

boolean employee_get_payroll_begin_end_work_dates(
					char **payroll_begin_work_date,
					char **payroll_end_work_date,
					char *payroll_pay_period_string,
					char *include_date );

LIST *employee_get_work_period_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date,
					char *end_work_date );

PAYROLL_POSTING *employee_get_payroll_posting(
					LIST *employee_list,
					int payroll_year,
					int payroll_period_number,
					char *begin_work_date,
					char *end_work_date );

LIST *employee_posting_calculate_work_period_list(
			double *employee_work_hours,
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
			int payroll_year,
			int payroll_period_number );

EMPLOYEE_WORK_PERIOD *employee_get_work_period(
			double *employee_work_hours,
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
			LIST *employee_work_day_list,
			double hourly_wage,
			double period_salary,
			double commission_sum_extension_percent,
			char *marital_status,
			int withholding_allowances,
			int withholding_additional_period_amount,
			int retirement_contribution_plan_employee_period_amount,
			int retirement_contribution_plan_employer_period_amount,
			int health_insurance_employee_period_amount,
			int health_insurance_employer_period_amount,
			int union_dues_period_amount,
			char *full_name,
			char *street_address,
			int payroll_year,
			int payroll_period_number );

#endif

