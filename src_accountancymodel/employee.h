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

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *begin_work_date_time;
	char *end_work_date_time;
	int employee_work_hours;
	int database_employee_work_hours;
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
		char *application_name,
		char *full_name,
		char *street_address,
		char *begin_work_date );

#endif

