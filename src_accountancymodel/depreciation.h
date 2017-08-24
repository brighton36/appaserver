/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/depreciation.h			*/
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

typedef struct
{
	LIST *entity_list;

} DEPRECIATE_FIXED_ASSET;

typedef struct
{
	char *asset_name;
	char *serial_number;
	char *estimated_purchase_date;
	double extension;
	int estimated_useful_life_years;
	int estimated_useful_life_units;
	int estimated_residual_value;
	int declining_balance_n;
	char *depreciation_method;
	double accumulated_depreciation;
	double database_accumulated_depreciation;
	double depreciation_amount;
	double database_depreciation_amount;
} DEPRECIATE_PRIOR_FIXED_ASSET;

typedef struct
{
	char *self_full_name;
	char *self_street_address;
	char *depreciation_date;
	char *transaction_date_time;
	char *database_transaction_date_time;
	TRANSACTION *transaction;
	LIST *depreciate_prior_fixed_asset_list;
} DEPRECIATE_PRIOR_FIXED_ASSET_DEPRECIATION;

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

void depreciation_list_update_and_transaction_propagate(
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

char *deprecation_get_prior_depreciation_date(
			LIST *depreciation_list );

DEPRECIATE_FIXED_ASSET *depreciation_fixed_asset_depreciation_new(
			char *application_name,
			char *fund_name,
			char *depreciation_date );

LIST *depreciation_fixed_asset_get_entity_list(
			char *application_name,
			char *fund_name,
			char *depreciation_date );

LIST *depreciation_get_depreciable_fixed_purchase_record_list(
			char *application_name,
			char *fund_name );

LIST *depreciation_get_depreciable_fixed_asset_purchase_list(
			char *application_name,
			char *full_name,
			char *street_address,
			char *fund_name,
			char *depreciation_date );

void depreciation_set_entity_list_transaction(
			LIST *entity_list );

void depreciation_fetch_purchase_fixed_asset_depreciation_list(
			LIST *depreciable_fixed_asset_purchase_list,
			char *application_name );

char *depreciation_fetch_prior_depreciation_date(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date );

void depreciation_fixed_asset_set_depreciation(
			LIST *entity_list,
			char *depreciation_date );

void depreciation_fixed_asset_entity_set_depreciation(
			double *entity_depreciation_amount,
			LIST *depreciable_fixed_asset_purchase_list,
			char *depreciation_date );

void depreciation_fixed_asset_depreciation_table_display(
			char *process_name,
			LIST *entity_list );

void depreciation_fixed_asset_depreciation_tree_display(
			LIST *entity_list );

void depreciation_fixed_asset_purchase_list_tree_display(
			LIST *depreciable_fixed_asset_purchase_list );

void depreciation_fixed_asset_purchase_list_table_display(
			FILE *output_pipe,
			char *full_name,
			char *street_address,
			double depreciation_amount,
			LIST *depreciable_fixed_asset_purchase_list );

boolean depreciation_date_exists(
			char *application_name,
			char *fund_name,
			char *depreciation_date );

void depreciation_fixed_asset_execute(
			LIST *entity_list,
			char *application_name,
			char *fund_name,
			char *depreciation_date );

void depreciation_fixed_asset_insert_depreciation(
			FILE *output_pipe,
			char *full_name,
			char *street_address,
			LIST *depreciable_fixed_asset_purchase_list,
			char *depreciation_date,
			char *transaction_date_time );

void depreciation_fixed_asset_insert_depreciation_entity_list(
			LIST *entity_list,
			char *depreciation_date );

void depreciation_fixed_asset_insert_transaction_entity_list(
			char *application_name,
			LIST *entity_list );

void depreciation_fixed_asset_insert_ledger_entity_list(
			char *application_name,
			char *fund_name,
			LIST *entity_list );

char *depreciation_fetch_max_depreciation_date(
			char *application_name,
			char *fund_name );

void depreciation_fixed_asset_set_transaction(
			LIST *entity_list );

boolean depreciation_date_prior_exists(
			char *application_name,
			char *depreciation_date );

char *depreciation_fetch_max_prior_depreciation_date(
			char *application_name );

LIST *depreciate_prior_fixed_asset_get_list(
			char *application_name );

DEPRECIATE_PRIOR_FIXED_ASSET *depreciate_prior_fixed_asset_new(
			void );

DEPRECIATE_PRIOR_FIXED_ASSET *depreciate_prior_fixed_asset_parse(
			char *input_buffer );

char *depreciate_prior_fixed_asset_get_select(
			void );

DEPRECIATE_PRIOR_FIXED_ASSET_DEPRECIATION *
	depreciate_prior_fixed_asset_depreciation_new(
			char *application_name,
			char *self_full_name,
			char *self_street_address );

void depreciation_prior_fixed_asset_table_display(
			char *process_name,
			LIST *depreciate_prior_fixed_asset_list );

#endif
