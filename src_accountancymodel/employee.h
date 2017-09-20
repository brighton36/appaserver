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

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *begin_work_date;
	char *end_work_date;
	double employee_work_hours;
	double gross_pay;
	double federal_tax_withholding_amount;
	double state_tax_withholding_amount;
	double social_security_employee_tax_amount;
	double social_security_employer_tax_amount;
	double medicare_employee_tax_amout;
	double medicare_employer_tax_amount;
	double retirement_contribution_plan_employee_amount;
	double retirement_contribution_plan_employer_amount;
	double health_insurance_employee_amount;
	double health_insurance_employer_amount;
	double federal_unemployment_tax_amount;
	double state_unemployment_tax_amount;
	double union_dues_amount;
	char *transaction_date_time;
	TRANSACTION *transaction;
} EMPLOYEE_WORK_PERIOD;

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
	double annual_salary;
	double commission_sum_extension_percent;
	char *marital_status;
	int withholding_allowances;
	double withholding_additional_amount;
	double retirement_contribution_plan_employee_period_amount;
	double retirement_contribution_plan_employer_period_amount;
	double health_insurance_employee_period_amount;
	double health_insurance_employer_period_amount;
	double union_dues_period_amount;
	LIST *employee_work_day_list;
	LIST *employee_work_period_list;
} EMPLOYEE;

/* Operations */
/* ---------- */
LIST *employee_get_work_day_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date );

EMPLOYEE *employee_new(			char *full_name,
					char *street_address );

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
		char *begin_work_date );

EMPLOYEE_WORK_DAY *employee_work_day_new(
					char *begin_work_date_time );

EMPLOYEE_WORK_PERIOD *employee_work_period_new(
					char *begin_work_date );

LIST *employee_get_work_period_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date );

#endif

