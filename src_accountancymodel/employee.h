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
#define DEFAULT_FEDERAL_MARITAL_STATUS	"single"
#define DEFAULT_STATE_MARITAL_STATUS	\
	"single_or_married_with_multiple_incomes"

/* See also $APPASERVER_HOME/utility/payroll_period.c */
/* -------------------------------------------------- */
#define PAYROLL_PERIOD_NUMBER_LABEL	".payroll_period_number"
#define PAYROLL_BEGIN_DATE_LABEL	".payroll_begin_date"
#define PAYROLL_END_DATE_LABEL		".payroll_end_date"

/* Structures */
/* ---------- */
typedef struct
{
	int income_over;
	int income_not_over;
	double tax_fixed_amount;
	double tax_percentage_amount;
} EMPLOYEE_INCOME_TAX_WITHHOLDING;

typedef struct
{
	char *marital_status;
	LIST *income_tax_withholding_list;
} EMPLOYEE_MARITAL_STATUS_WITHHOLDING;

typedef struct
{
	char *state_marital_status;
	int state_withholding_allowances;
	double state_standard_deduction_amount;
} EMPLOYEE_STATE_STANDARD_DEDUCTION;

typedef struct
{
	LIST *federal_marital_status_list;
	LIST *state_marital_status_list;
	LIST *state_standard_deduction_list;
} EMPLOYEE_TAX_WITHHOLDING_TABLE;

typedef struct
{
	char *full_name;
	char *street_address;
	int payroll_year;
	int payroll_period_number;
	char *begin_work_date;
	char *end_work_date;
	double employee_work_hours;
	double commission_sum_extension;
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
	double commission_sum_extension;
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
	double gross_pay_year_to_date;
	double database_gross_pay_year_to_date;
	char *federal_marital_status;
	int federal_withholding_allowances;
	int federal_withholding_additional_period_amount;
	char *state_marital_status;
	int state_withholding_allowances;
	int state_itemized_deduction_allowances;
	int retirement_contribution_plan_employee_period_amount;
	int retirement_contribution_plan_employer_period_amount;
	int health_insurance_employee_period_amount;
	int health_insurance_employer_period_amount;
	int union_dues_period_amount;
	char *terminated_date;
	LIST *employee_work_day_list;
	LIST *employee_work_period_list;
	LIST *customer_sale_list;
} EMPLOYEE;

/* Operations */
/* ---------- */
LIST *employee_fetch_work_day_list(	char *application_name,
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

LIST *employee_fetch_list(		char *application_name,
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

LIST *employee_fetch_work_period_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *begin_work_date,
					char *end_work_date );

PAYROLL_POSTING *employee_get_payroll_posting(
				LIST *employee_list,
				char *application_name,
				int payroll_year,
				int payroll_period_number,
				char *begin_work_date,
				char *end_work_date,
				ENTITY_SELF *self,
				EMPLOYEE_TAX_WITHHOLDING_TABLE *
					employee_tax_withholding_table );

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
				employee_tax_withholding_table );

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
				employee_tax_withholding_table );

char *employee_update_get_sys_string(
				char *application_name );

void employee_update(		char *application_name,
				char *full_name,
				char *street_address,
				double gross_pay_year_to_date,
				double database_gross_pay_year_to_date );

double employee_calculate_employee_work_hours(
				LIST *employee_work_day_list );

double employee_calculate_commission_sum_extension(
				LIST *customer_sale_list );

double employee_calculate_federal_taxable_income(
				double gross_pay,
				int withholding_allowances,
				double withholding_allowance_period_value );

double employee_calculate_state_taxable_income(
				double gross_pay,
				double state_standard_deduction_amount,
				double state_itemized_deduction_amount );

LIST *employee_fetch_federal_income_tax_withholding_list(
				char *application_name,
				char *status /* single or married */ );

LIST *employee_fetch_state_income_tax_withholding_list(
				char *application_name,
				char *status /* single or married */ );

EMPLOYEE_INCOME_TAX_WITHHOLDING *employee_income_tax_withholding_new(
				int income_over );

EMPLOYEE_STATE_STANDARD_DEDUCTION *employee_state_standard_deduction_new(
				char *state_marital_status,
				int state_withholding_allowances,
				double state_standard_deduction_amount );

EMPLOYEE_MARITAL_STATUS_WITHHOLDING *employee_marital_status_withholding_new(
				char *marital_status );

EMPLOYEE_TAX_WITHHOLDING_TABLE *employee_tax_withholding_table_new(
				char *application_name );

double employee_get_federal_tax_withholding_amount(
				double amount_subject_to_withholding,
				LIST *tax_withholding_list );

double employee_calculate_computed_tax(
				double taxable_income,
				char *marital_status_string,
				LIST *marital_status_list );

double employee_calculate_federal_tax_withholding_amount(
			char *federal_marital_status,
			int federal_withholding_allowances,
			double federal_withholding_allowance_period_value,
			double gross_pay,
			LIST *federal_marital_status_list );

double employee_calculate_state_tax_withholding_amount(
			char *state_marital_status,
			int state_withholding_allowances,
			int state_itemized_deduction_allowances,
			double state_withholding_allowance_period_value,
			double state_itemized_allowance_period_value,
			double gross_pay,
			LIST *state_marital_status_list,
			LIST *state_standard_deduction_list );

LIST *employee_fetch_marital_status_string_list(
				char *application_name,
				char *federal_or_state );

LIST *employee_fetch_marital_status_list(
				char *application_name,
				LIST *marital_status_string_list,
				char *federal_or_state );

LIST *employee_fetch_income_tax_withholding_list(
				char *application_name,
				char *federal_or_state,
				char *marital_status_string );

LIST *employee_fetch_state_standard_deduction_list(
				char *application_name );

double employee_get_state_standard_deduction_amount(
				LIST *state_standard_deduction_list,
				char *state_marital_status,
				int state_withholding_allowances );

#endif

