/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/depreciation.h			*/
/* -------------------------------------------------------------------- */
/* This is the appaserver depreciation ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DEPRECIATION_H
#define DEPRECIATION_H

#include "ledger.h"
#include "entity.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define DEPRECIATION_MEMO		"Depreciation"

/* Structures */
/* ---------- */
typedef struct
{
	char *undo_fixed_asset_date;
	char *prior_fixed_asset_date;
	char *undo_fixed_prior_date;
	char *prior_fixed_prior_date;
	char *undo_property_date;
	char *prior_property_date;
	char *undo_property_prior_date;
	char *prior_property_prior_date;
} DEPRECIATION_DATE;

typedef struct
{
	LIST *fixed_asset_purchase_list;
	LIST *fixed_asset_prior_list;
	LIST *property_purchase_list;
	LIST *property_prior_list;
} DEPRECIATION_ASSET_LIST;

typedef struct
{
	char *fund_name;
	TRANSACTION *purchase_fixed_asset_transaction;
	TRANSACTION *prior_fixed_asset_transaction;
	TRANSACTION *purchase_property_transaction;
	TRANSACTION *prior_property_transaction;
	double purchase_fixed_asset_depreciation_amount;
	double prior_fixed_asset_depreciation_amount;
	double purchase_property_depreciation_amount;
	double prior_property_depreciation_amount;
	char *depreciation_expense_account;
	char *accumulated_depreciation_account;
	DEPRECIATION_DATE *depreciation_date;
	DEPRECIATION_ASSET_LIST *depreciation_asset_list;
} DEPRECIATION_FUND;

typedef struct
{
	char *depreciation_date;
	int units_produced;
	double depreciation_amount;
	double database_depreciation_amount;
	char *transaction_date_time;
	char *database_transaction_date_time;
	TRANSACTION *transaction;
} DEPRECIATION;

typedef struct
{
	DEPRECIATION_DATE *depreciation_date;
	LIST *depreciation_fund_list;
} DEPRECIATION_STRUCTURE;

/* Operations */
/* ---------- */
DEPRECIATION_STRUCTURE *depreciation_structure_new(
			char *application_name );

DEPRECIATION_ASSET_LIST *depreciation_asset_list_new(
			void );

DEPRECIATION_DATE *depreciation_date_new(
			char *application_name );

DEPRECIATION *depreciation_calloc(
			void );

DEPRECIATION_FUND *depreciation_fund_new(
			char *application_name,
			char *fund_name );

DEPRECIATION *depreciation_new(
			void );

double depreciation_calculate_amount(
			char *depreciation_method,
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation,
			char *service_placement_date,
			int units_produced );

double depreciation_sum_of_years_digits_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation,
			char *service_placement_date );

double depreciation_straight_line_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation );

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
			double finance_accumulated_depreciation,
			int n );

double depreciation_units_of_production_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_units,
			int units_produced,
			double finance_accumulated_depreciation );

boolean depreciation_date_exists(
			char *application_name,
			char *folder_name,
			char *depreciation_date );

char *depreciation_fetch_max_depreciation_date(
			char *application_name,
			char *folder_name );

void depreciation_fixed_asset_list_set(
			LIST *fixed_asset_list,
			double *purchased_fixed_asset_depreciation_amount,
			char *depreciation_date,
			char *prior_depreciation_date );

LIST *depreciation_fetch_fund_list(
			char *application_name );

LIST *fixed_asset_depreciation_purchase_fetch_list(
			char *application_name,
			char *fund_name );

void depreciation_fund_list_table_display(
			char *process_name,
			LIST *depreciation_fund_list );

void depreciation_fixed_asset_list_table_display(
			char *process_name,
			LIST *fixed_asset_list );

boolean depreciation_fund_list_insert(
			LIST *depreciation_fund_list,
			char *application_name,
			char *full_name,
			char *street_address );

boolean depreciation_fixed_asset_list_insert(
			LIST *fixed_asset_list,
			char *folder_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

void depreciation_fund_list_set_transaction(
			LIST *depreciation_fund_list,
			char *full_name,
			char *street_address );

void depreciation_fund_get_transaction(
		TRANSACTION **purchase_transaction,
		TRANSACTION **prior_transaction,
		char *full_name,
		char *street_address,
		char *depreciation_expense_account,
		char *accumulated_depreciation_account,
		double purchased_fixed_asset_depreciation_amount,
		double prior_fixed_asset_depreciation_amount );

#endif
