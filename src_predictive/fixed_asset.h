/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset.h			*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks fixed_asset ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FIXED_ASSET_H
#define FIXED_ASSET_H

#include "list.h"
#include "boolean.h"
#include "inventory.h"
#include "ledger.h"
#include "depreciation.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */

typedef struct
{
	char *asset_name;
	char *serial_number;
	char *account_name;
	char *service_placement_date;
	double extension;
	int estimated_useful_life_years;
	int estimated_useful_life_units;
	int estimated_residual_value;
	int declining_balance_n;
	char *depreciation_method;
	double tax_cost_basis;
	char *tax_recovery_period;
	char *disposal_date;
	double finance_accumulated_depreciation;
	double database_finance_accumulated_depreciation;
	double tax_accumulated_depreciation;
	double database_tax_accumulated_depreciation;
	char *prior_depreciation_date;
	LIST *depreciation_list;
	LIST *tax_recovery_list;
} FIXED_ASSET;

/* Operations */
/* ---------- */
FIXED_ASSET *fixed_asset_new(
					void );

FIXED_ASSET *fixed_asset_parse(
					char *input_buffer );

FIXED_ASSET *fixed_asset_purchase_fetch(
					char *application_name,
					char *asset_name,
					char *serial_number );

FIXED_ASSET *fixed_asset_prior_fetch(
					char *application_name,
					char *asset_name,
					char *serial_number );

FIXED_ASSET *fixed_asset_list_seek(
				LIST *fixed_asset_purchase_list,
				char *asset_name,
				char *serial_number );

char *fixed_asset_get_select(	char *folder_name );

void fixed_asset_update_stream(
				FILE *update_pipe,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *asset_name,
				char *serial_number,
				double finance_accumulated_depreciation );

FILE *fixed_asset_get_update_pipe(
				char *application_name );

void fixed_asset_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			double finance_accumulated_depreciation,
			double database_finance_accumulated_depreciation );

void fixed_asset_depreciation_update_and_transaction_propagate(
				FIXED_ASSET *fixed_asset,
				char *service_placement_date,
				char *application_name,
				char *fund_name );

void fixed_asset_depreciation_delete(
				FIXED_ASSET *fixed_asset,
				char *application_name,
				char *fund_name );

void fixed_asset_list_depreciation_method_update(
				LIST *fixed_asset_purchase_list,
				char *service_placement_date,
				char *application_name,
				char *fund_name );

void fixed_asset_list_depreciation_delete(
				LIST *fixed_asset_list,
				char *application_name,
				char *fund_name );

LIST *fixed_asset_get_account_name_list(
				char *application_name );

LIST *fixed_asset_account_journal_ledger_refresh(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				LIST *purchase_asset_account_list );

void fixed_asset_depreciation_update_and_transaction_propagate(
				FIXED_ASSET *fixed_asset,
				char *service_placement_date,
				char *application_name,
				char *fund_name );

char *fixed_asset_get_account_name(
				char *application_name,
				char *asset_name );

#endif

