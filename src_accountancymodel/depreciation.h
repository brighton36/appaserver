/* -------------------------------------------------------------------- */
/* src_accountancymodel/depreciation.h					*/
/* -------------------------------------------------------------------- */
/* This is the appaserver depreciation ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DEPRECIATION_H
#define DEPRECIATION_H

#include "ledger.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define DEPRECIATION_MEMO		"Depreciation"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *asset_name;
	char *serial_number;
	int units_produced;
	char *depreciation_date;
	double depreciation_amount;
	double database_depreciation_amount;
	char *transaction_date_time;
	char *database_transaction_date_time;
	TRANSACTION *transaction;
} DEPRECIATION;

/* Operations */
/* ---------- */
DEPRECIATION *depreciation_calloc( void );

DEPRECIATION *depreciation_fetch(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date );

void depreciation_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date,
			double depreciation_amount,
			double database_depreciation_amount,
			char *transaction_date_time,
			char *database_transaction_date_time );

DEPRECIATION *depreciation_parse(
			char *application_name,
			char *input_buffer );

char *depreciation_get_select(
			void );

char *depreciation_get_update_sys_string(
			char *application_name );

double depreciation_get_amount(
			char *depreciation_method,
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double accumulated_depreciation,
			char *arrived_date_string,
			int units_produced );

double depreciation_sum_of_years_digits_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double accumulated_depreciation,
			char *arrived_date_string );

double depreciation_straight_line_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double accumulated_depreciation );

LIST *depreciation_journal_ledger_refresh(
			char *application_name,
			char *fund_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double depreciation_amount );

double depreciation_get_fraction_of_year(
			char *prior_depreciation_date_string,
			char *depreciation_date_string );

double depreciation_n_declining_balance_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double accumulated_depreciation,
			int n );

LIST *depreciation_fetch_list(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number );

/* Returns new accumulated_depreciation */
/* ------------------------------------ */
double depreciation_list_set(
			LIST *depreciation_list,
			char *depreciation_method,
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			char *arrived_date_string );

void depreciation_list_update(
			LIST *depreciation_list,
			char *application_name,
			char *fund_name );

void depreciation_list_delete(
			LIST *depreciation_list,
			char *application_name,
			char *fund_name );

void depreciation_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date );

DEPRECIATION *depreciation_list_seek(
			LIST *depreciation_list,
			char *depreciation_date );

double depreciation_units_of_production_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_units,
			int units_produced,
			double accumulated_depreciation );

#endif

