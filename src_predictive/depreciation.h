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
	char *asset_name;
	char *serial_number;
	char *depreciation_date;
	int units_produced;
	double depreciation_amount;
	double database_depreciation_amount;
	char *transaction_date_time;
	char *database_transaction_date_time;
	TRANSACTION *transaction;
} DEPRECIATION;

/* Operations */
/* ---------- */
DEPRECIATION *depreciation_calloc( void );

DEPRECIATION *depreciation_new(
			char *asset_name,
			char *serial_number );

void depreciation_update(
			char *application_name,
			char *folder_name,
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
			char *application_name,
			char *folder_name );

double depreciation_get_amount(
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

LIST *depreciation_fetch_list(
			char *application_name,
			char *asset_name,
			char *serial_number );

/* Returns new finance_accumulated_depreciation */
/* -------------------------------------------- */
double depreciation_list_set(
			LIST *depreciation_list,
			char *depreciation_method,
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			char *service_placement_date );

void depreciation_list_update_and_transaction_propagate(
			LIST *depreciation_list,
			char *application_name,
			char *folder_name,
			char *fund_name );

void depreciation_list_delete(
			LIST *depreciation_list,
			char *application_name,
			char *fund_name,
			char *folder_name );

void depreciation_delete(
			char *application_name,
			char *folder_name,
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
			double finance_accumulated_depreciation );

char *deprecation_get_prior_depreciation_date(
			LIST *depreciation_list );

LIST *depreciation_fetch_fixed_asset_list(
			char *application_name,
			char *fund_name,
			char *folder_name,
			char *depreciation_date );

/*
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

void depreciation_fixed_asset_set_depreciation(
			LIST *entity_list,
			char *depreciation_date );

void depreciation_fixed_asset_entity_set_depreciation(
			double *entity_depreciation_amount,
			LIST *depreciable_fixed_asset_purchase_list,
			char *depreciation_date );

void depreciation_fixed_asset_table_display(
			char *process_name,
			LIST *fixed_asset_purchased_list );

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
*/

char *depreciation_fetch_prior_depreciation_date(
			char *application_name,
			char *folder_name,
			char *asset_name,
			char *serial_number,
			char *depreciation_date );

boolean depreciation_date_exists(
			char *application_name,
			char *folder_name,
			char *depreciation_date );

void depreciation_fixed_asseth_execute(
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
			char *folder_name );

void depreciation_fixed_asset_set_transaction(
			LIST *entity_list );

boolean depreciation_date_prior_exists(
			char *application_name,
			char *depreciation_date );

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

void depreciation_prior_fixed_asset_set_depreciation(
			double *depreciation_amount,
			LIST *depreciate_prior_fixed_asset_list,
			char *prior_depreciation_date,
			char *depreciation_date );

void depreciation_prior_fixed_asset_insert_depreciation(
			char *full_name,
			char *street_address,
			LIST *depreciate_prior_fixed_asset_list,
			char *depreciation_date,
			char *transaction_date_time );

void depreciation_fixed_asset_list_set_depreciation(
			LIST *fixed_asset_list,
			double *purchased_fixed_asset_depreciation_amount,
			char *depreciation_date,
			char *prior_depreciation_date );

#endif
