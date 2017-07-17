/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/purchase.h			*/
/* -------------------------------------------------------------------- */
/* This is the AccountancyModel purchase ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PURCHASE_H
#define PURCHASE_H

#include "list.h"
#include "boolean.h"
#include "inventory.h"
#include "ledger.h"
#include "entity.h"
#include "depreciation.h"

/* Constants */
/* --------- */
#define PURCHASE_ORDER_MEMO		"Purchase Order"
#define PURCHASE_VENDOR_PAYMENT_MEMO	"Vendor Payment"

#define PURCHASE_GET_AMOUNT_DUE( purchase_amount, sum_vendor_payment )	\
					( purchase_amount -		\
					  sum_vendor_payment )

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *payment_date_time;
	double payment_amount;
	int check_number;
	char *transaction_date_time;
	char *database_transaction_date_time;
	TRANSACTION *transaction;
} VENDOR_PAYMENT;

typedef struct
{
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *asset_name;
	char *asset_account_name;
	char *expense_account_name;
	double extension;
	double accrual_period_years;
	double accumulated_accrual;
	double database_accumulated_accrual;
	LIST *accrual_list;
} PURCHASE_PREPAID_ASSET;

typedef struct
{
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *asset_name;
	char *serial_number;
	char *account_name;
	char *arrived_date_time;
	double extension;
	int estimated_useful_life_years;
	int estimated_useful_life_units;
	int estimated_residual_value;
	int declining_balance_n;
	char *depreciation_method;
	double accumulated_depreciation;
	double database_accumulated_depreciation;
	char *max_depreciation_date;
	LIST *depreciation_list;
} PURCHASE_FIXED_ASSET;

typedef struct
{
	char *account_name;
	double debit_amount;
} PURCHASE_ASSET_ACCOUNT;

typedef struct
{
	char *account_name;
	double extension;
	double database_extension;
} PURCHASE_SERVICE;

typedef struct
{
	char *supply_name;
	int quantity;
	double unit_cost;
	double extension;
	double database_extension;
} PURCHASE_SUPPLY;

typedef struct
{
	char *inventory_name;
	char *serial_number;
	double unit_cost;
} PURCHASE_SPECIFIC_INVENTORY;

typedef struct
{
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *fund_name;
	char *transaction_date_time;
	char *database_transaction_date_time;
	double sum_inventory_extension;
	double sum_specific_inventory_unit_cost;
	double sum_supply_extension;
	double sum_service_extension;
	double sum_fixed_asset_extension;
	double sum_prepaid_asset_extension;
	double sum_extension;
	double database_sum_extension;
	double sales_tax;
	double freight_in;
	double purchase_amount;
	double database_purchase_amount;
	double sum_payment_amount;
	double amount_due;
	double database_amount_due;
	enum title_passage_rule title_passage_rule;
	char *shipped_date;
	char *database_shipped_date;
	char *arrived_date_time;
	char *database_arrived_date_time;
	LIST *inventory_purchase_list;
	LIST *specific_inventory_purchase_list;
	LIST *supply_purchase_list;
	LIST *service_purchase_list;
	LIST *fixed_asset_purchase_list;
	LIST *prepaid_asset_purchase_list;
	LIST *purchase_asset_account_list;
	LIST *vendor_payment_list;
	TRANSACTION *transaction;
	LIST *propagate_account_list;
} PURCHASE_ORDER;

/* Operations */
/* ---------- */
PURCHASE_ORDER *purchase_order_new(	char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time );

LIST *purchase_supply_get_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time );

LIST *purchase_service_get_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time );

LIST *purchase_fixed_asset_get_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time );

PURCHASE_SPECIFIC_INVENTORY *purchase_specific_inventory_new(
					char *inventory_name );

PURCHASE_SUPPLY *purchase_supply_new(	char *supply_name );

PURCHASE_SERVICE *purchase_service_new(	char *account_name );

PURCHASE_PREPAID_ASSET *purchase_prepaid_asset_new(
					void );

PURCHASE_FIXED_ASSET *purchase_fixed_asset_new(
					void );

PURCHASE_ASSET_ACCOUNT *purchase_asset_account_new(
					char *account_name );

LIST *purchase_get_asset_account_list(	char *application_name,
					LIST *fixed_asset_purchase_list,
					LIST *prepaid_asset_purchase_list );

PURCHASE_FIXED_ASSET *purchase_fixed_asset_parse(
					char *input_buffer );

PURCHASE_PREPAID_ASSET *purchase_prepaid_asset_parse(
					char *input_buffer );

PURCHASE_FIXED_ASSET *purchase_fixed_asset_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time,
					char *asset_name,
					char *serial_number,
					char *arrived_date_time );

double purchase_get_sum_inventory_extension(
					LIST *inventory_purchase_list );

double purchase_get_sum_supply_extension(
					LIST *supply_purchase_list );

double purchase_get_sum_service_extension(
					LIST *service_purchase_list );

double purchase_get_sum_fixed_asset_extension(
					LIST *fixed_asset_purchase_list );

double purchase_get_sum_prepaid_asset_extension(
					LIST *prepaid_asset_purchase_list );

boolean purchase_order_load(	double *sum_extension,
				double *database_sum_extension,
				double *sales_tax,
				double *freight_in,
				double *purchase_amount,
				double *database_purchase_amount,
				double *amount_due,
				double *database_amount_due,
				enum title_passage_rule *title_passage_rule,
				char **shipped_date,
				char **database_shipped_date,
				char **arrived_date_time,
				char **database_arrived_date_time,
				char **transaction_date_time,
				char **database_transaction_date_time,
				char **fund_name,
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time );

void purchase_order_update(	char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				double sum_extension,
				double database_sum_extension,
				double purchase_amount,
				double database_purchase_amount,
				double amount_due,
				double database_amount_due,
				char *transaction_date_time,
				char *database_transaction_date_time,
				char *arrived_date_time,
				char *database_arrived_date_time,
				char *shipped_date,
				char *database_shipped_date );

void purchase_order_transaction_date_time_update(
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *transaction_date_time,
				char *application_name );

void purchase_order_arrived_date_time_update(
				PURCHASE_ORDER *purchase_order,
				char *application_name );

void purchase_order_shipped_date_update(
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *shipped_date,
				char *application_name );

double purchase_order_get_purchase_amount(
				double *sum_inventory_extension,
				double *sum_specific_inventory_extension,
				double *sum_supply_extension,
				double *sum_service_extension,
				double *sum_fixed_asset_extension,
				double *sum_prepaid_asset_extension,
				double *sum_extension,
				LIST *inventory_purchase_list,
				LIST *specific_inventory_purchase_list,
				LIST *supply_purchase_list,
				LIST *service_purchase_list,
				LIST *fixed_asset_purchase_list,
				LIST *prepaid_asset_purchase_list,
				double sales_tax,
				double freight_in );

char *purchase_order_get_update_sys_string(
				char *application_name );

char *purchase_get_max_purchase_date_time(
				char *application_name );

/* Returns propagate_account_list */
/* ------------------------------ */
LIST *purchase_order_journal_ledger_refresh(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double sum_specific_inventory_unit_cost,
				double sum_supply_extension,
				double sum_service_extension,
				double sales_tax,
				double freight_in,
				double purchase_amount,
				LIST *inventory_purchase_list,
				LIST *supply_purchase_list,
				LIST *service_purchase_list,
				LIST *purchase_asset_account_list );

char *purchase_get_arrived_purchase_date_time(
				char *application_name,
				char *arrived_date_time );

char *purchase_order_fetch_arrived_date_time(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time );

char *purchase_order_fetch_transaction_date_time(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time );

char *purchase_order_get_select( void );

void purchase_order_parse(	char **full_name,
				char **street_address,
				char **purchase_date_time,
				double *sum_extension,
				double *database_sum_extension,
				double *sales_tax,
				double *freight_in,
				double *purchase_amount,
				double *database_purchase_amount,
				double *amount_due,
				double *database_amount_due,
				enum title_passage_rule *title_passage_rule,
				char **shipped_date,
				char **database_shipped_date,
				char **arrived_date_time,
				char **database_arrived_date_time,
				char **transaction_date_time,
				char **database_transaction_date_time,
				char **fund_name,
				char *results );

PURCHASE_ORDER *purchase_order_calloc(
				void );

PURCHASE_ORDER *purchase_order_seek(
				LIST *purchase_order_list,
				char *purchase_date_time );

PURCHASE_ORDER *purchase_order_transaction_date_time_fetch(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time );

void purchase_order_inventory_delete(
				LIST *inventory_purchase_list,
				char *inventory_name );

char *purchase_order_get_inventory_purchase_join_where(
				void );

LIST *purchase_get_inventory_purchase_list(
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				HASH_TABLE *inventory_purchase_hash_table,
				LIST *inventory_purchase_name_list );

LIST *purchase_get_inventory_purchase_order_list(
				char *application_name,
				char *inventory_name,
				HASH_TABLE *inventory_purchase_hash_table,
				LIST *inventory_purchase_name_list,
				HASH_TABLE *transaction_hash_table,
				HASH_TABLE *journal_ledger_hash_table );

boolean purchase_inventory_is_latest(
				char *application_name,
				char *inventory_name,
				char *purchase_date_time );

void purchase_order_transaction_delete_with_propagate(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time );

double purchase_get_sum_payment_amount(
				LIST *vendor_payment_list );

VENDOR_PAYMENT *purchase_vendor_payment_new(
				char *payment_date_time );

LIST *purchase_get_vendor_payment_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time );

VENDOR_PAYMENT *purchase_vendor_payment_seek(
				LIST *vendor_payment_list,
				char *payment_date_time );

LIST *purchase_vendor_payment_journal_ledger_refresh(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double payment_amount,
				int check_number );

void purchase_vendor_payment_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *payment_date_time,
				char *transaction_date_time,
				char *database_transaction_date_time );

char *purchase_vendor_payment_get_update_sys_string(
				char *application_name );

double purchase_supply_get_extension(
				double unit_cost,
				int quantity );

PURCHASE_SUPPLY *purchase_supply_list_seek(
				LIST *supply_purchase_list,
				char *supply_name );

PURCHASE_PREPAID_ASSET *purchase_prepaid_asset_list_seek(
				LIST *accrual_asset_purchase_list,
				char *asset_name );

PURCHASE_FIXED_ASSET *purchase_fixed_asset_list_seek(
				LIST *fixed_asset_purchase_list,
				char *asset_name,
				char *serial_number );

PURCHASE_SERVICE *purchase_service_list_seek(
				LIST *service_purchase_list,
				char *service_name );

void purchase_supply_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *supply_name,
				double extension,
				double database_extension );

LIST *purchase_inventory_journal_ledger_debit_refresh(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				LIST *inventory_purchase_list );

LIST *purchase_supply_journal_ledger_refresh(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double sum_supply_extension,
				LIST *supply_purchase_list );

LIST *purchase_service_journal_ledger_refresh(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				LIST *service_purchase_list );

LIST *purchase_fixed_prepaid_asset_journal_ledger_refresh(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				LIST *fixed_asset_purchase_list,
				LIST *prepaid_asset_purchase_list );

LIST *purchase_specific_inventory_get_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time );

double purchase_get_sum_specific_inventory_unit_cost(
				LIST *specific_inventory_purchase_list );

double purchase_order_get_amount_due(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time );

double purchase_order_get_total_payment(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time );

char *purchase_fixed_asset_get_select(
				void );

char *purchase_fixed_asset_get_update_sys_string(
				char *application_name );

void purchase_fixed_asset_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *asset_name,
				char *serial_number,
				double accumulated_depreciation,
				double database_accumulated_depreciation );

void purchase_prepaid_asset_propagate(
				PURCHASE_PREPAID_ASSET *purchase_prepaid_asset,
				char *purchase_date_time,
				char *application_name,
				char *fund_name );

void purchase_depreciation_update_and_transaction_propagate(
				PURCHASE_FIXED_ASSET *purchase_fixed_asset,
				char *arrived_date_time,
				char *application_name,
				char *fund_name );

void purchase_fixed_asset_depreciation_delete(
				PURCHASE_FIXED_ASSET *purchase_fixed_asset,
				char *application_name,
				char *fund_name );

void purchase_prepaid_asset_accrual_delete(
				PURCHASE_PREPAID_ASSET *purchase_prepaid_asset,
				char *application_name,
				char *fund_name );

void purchase_fixed_asset_list_depreciation_method_update(
				LIST *fixed_asset_purchase_list,
				char *arrived_date_time,
				char *application_name,
				char *fund_name );

void purchase_fixed_asset_list_depreciation_delete(
				LIST *purchase_fixed_asset_list,
				char *application_name,
				char *fund_name );

LIST *purchase_get_asset_account_name_list(
				char *application_name );

double purchase_get_asset_account_debit_amount(
				LIST *fixed_asset_purchase_list,
				LIST *prepaid_asset_purchase_list,
				char *asset_account_name );

LIST *purchase_order_inventory_journal_ledger_refresh(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double purchase_amount,
				LIST *inventory_purchase_list );

PURCHASE_PREPAID_ASSET *purchase_prepaid_asset_fetch(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *asset_name );

LIST *purchase_prepaid_asset_get_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time );

LIST *purchase_asset_account_journal_ledger_refresh(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				LIST *purchase_asset_account_list );

void purchase_prepaid_asset_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *asset_name,
				double accumulated_accrual,
				double database_accumulated_accrual );

char *purchase_prepaid_asset_get_update_sys_string(
				char *application_name );

void purchase_depreciation_update_and_transaction_propagate(
				PURCHASE_FIXED_ASSET *purchase_fixed_asset,
				char *arrived_date_time,
				char *application_name,
				char *fund_name );

void purchase_accrual_update_and_transaction_propagate(
				PURCHASE_PREPAID_ASSET *purchase_prepaid_asset,
				char *arrived_date_string,
				char *application_name );

char *purchase_fixed_asset_get_account_name(
				char *application_name,
				char *asset_name );

boolean purchase_prepaid_asset_get_account_names(
				char **asset_account_name,
				char **expense_account_name,
				char *application_name,
				char *asset_name );

PURCHASE_ORDER *purchase_order_transaction_date_time_fetch(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time );

void purchase_vendor_payment_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *payment_date_time,
				double payment_amount,
				int check_number,
				char *transaction_date_time );

char *purchase_order_display(	PURCHASE_ORDER *purchase_order );

char *purchase_order_list_display(
				LIST *purchase_order_list );

LIST *purchase_inventory_distinct_account_extract(
				LIST *inventory_purchase_list );

#endif

