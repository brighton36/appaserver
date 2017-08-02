/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/customer.h			*/
/* -------------------------------------------------------------------- */
/* This is the AccountancyModel customer_sale ADT.			*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef CUSTOMER_SALE_H
#define CUSTOMER_SALE_H

#include "list.h"
#include "boolean.h"
#include "inventory.h"
#include "ledger.h"
#include "hash_table.h"
#include "entity.h"

/* Constants */
/* --------- */
#define CUSTOMER_SALE_MEMO		"Customer Sale"
#define CUSTOMER_PAYMENT_MEMO		"Customer Payment"

#define CUSTOMER_GET_AMOUNT_DUE( invoice_amount, total_payment )	\
				( invoice_amount - total_payment )

#define CUSTOMER_SALE_GET_EXTENSION( retail_price, discount_amount )	\
				( retail_price - discount_amount )

#define CUSTOMER_HOURLY_SERVICE_GET_EXTENSION(				\
		hourly_rate,						\
		work_hours,						\
		discount_amount )					\
		( ( hourly_rate * work_hours ) - discount_amount )

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *begin_date_time;
	char *end_date_time;
	double work_hours;
	double database_work_hours;
} SERVICE_WORK;

typedef struct
{
	char *service_name;
	char *description;
	double hourly_rate;
	double discount_amount;
	double extension;
	double database_extension;
	double work_hours;
	double database_work_hours;
	LIST *service_work_list;
} HOURLY_SERVICE;

typedef struct
{
	char *service_name;
	double retail_price;
	double discount_amount;
	double extension;
	double database_extension;
	double work_hours;
	double database_work_hours;
	LIST *service_work_list;
} FIXED_SERVICE;

typedef struct
{
	char *inventory_name;
	char *serial_number;
	double retail_price;
	double discount_amount;
	double extension;
	double database_extension;
	double cost_of_goods_sold;
} SPECIFIC_INVENTORY_SALE;

typedef struct
{
	char *payment_date_time;
	double payment_amount;
	char *transaction_date_time;
	char *database_transaction_date_time;
	TRANSACTION *transaction;
} CUSTOMER_PAYMENT;

typedef struct
{
	char *fund_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *completed_date_time;
	char *database_completed_date_time;
	char *arrived_date;
	char *database_arrived_date;
	char *shipped_date_time;
	char *database_shipped_date_time;
	double sales_tax;
	double database_sales_tax;
	double sum_extension;
	double database_sum_extension;
	double sum_inventory_extension;
	double sum_fixed_service_extension;
	double sum_fixed_extension;
	double sum_hourly_service_extension;
	double sum_hourly_extension;
	double invoice_amount;
	double database_invoice_amount;
	enum title_passage_rule title_passage_rule;
	double total_payment;
	double database_total_payment;
	double amount_due;
	double database_amount_due;
	char *transaction_date_time;
	char *database_transaction_date_time;
	double shipping_revenue;
	LIST *inventory_sale_list;
	LIST *specific_inventory_sale_list;
	LIST *fixed_service_sale_list;
	LIST *hourly_service_sale_list;
	LIST *payment_list;
	TRANSACTION *transaction;
	LIST *propagate_account_list;
	LIST *inventory_account_list;
	LIST *cost_account_list;
} CUSTOMER_SALE;

/* Operations */
/* ---------- */
LIST *customer_payment_get_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time );

CUSTOMER_SALE *customer_sale_calloc(	void );

CUSTOMER_SALE *customer_sale_new(	char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time );

LIST *customer_sale_specific_inventory_get_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time );

SPECIFIC_INVENTORY_SALE *customer_specific_inventory_sale_new(
					char *inventory_name,
					char *serial_number );

FIXED_SERVICE *customer_fixed_service_sale_new(
					char *service_name );

HOURLY_SERVICE *customer_hourly_service_sale_new(
					char *service_name,
					char *description );

double customer_sale_get_sum_inventory_extension(
					LIST *inventory_sale_list );

double customer_sale_get_sum_specific_inventory_extension(
					LIST *specific_inventory_sale_list );

double customer_sale_get_cost_of_goods_sold(
					LIST *inventory_sale_list );

double customer_sale_get_sum_fixed_service_extension(
					LIST *fixed_service_sale_list );

double customer_sale_get_sum_hourly_service_extension(
					LIST *hourly_service_sale_list );

void customer_sale_parse(	char **full_name,
				char **street_address,
				char **sale_date_time,
				double *sum_extension,
				double *database_sum_extension,
				double *sales_tax,
				double *database_sales_tax,
				double *invoice_amount,
				double *database_invoice_amount,
				enum title_passage_rule *title_passage_rule,
				char **completed_date_time,
				char **database_completed_date_time,
				char **arrived_date,
				char **database_arrived_date,
				char **shipped_date_time,
				char **database_shipped_date_time,
				double *total_payment,
				double *database_total_payment,
				double *amount_due,
				double *database_amount_due,
				char **transaction_date_time,
				char **database_transaction_date_time,
				double *shipping_revenue,
				char **fund_name,
				char *input_buffer );

boolean customer_sale_load(
				double *sum_extension,
				double *database_sum_extension,
				double *sales_tax,
				double *database_sales_tax,
				double *invoice_amount,
				double *database_invoice_amount,
				enum title_passage_rule *title_passage_rule,
				char **completed_date_time,
				char **database_completed_date_time,
				char **arrived_date,
				char **database_arrived_date,
				char **shipped_date_time,
				char **database_shipped_date_time,
				double *total_payment,
				double *database_total_payment,
				double *amount_due,
				double *database_amount_due,
				char **transaction_date_time,
				char **database_transaction_date_time,
				double *shipping_revenue,
				char **fund_name,
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );

void customer_sale_update(	double sum_extension,
				double database_sum_extension,
				double sales_tax,
				double database_sales_tax,
				double invoice_amount,
				double database_invoice_amount,
				char *completed_date_time,
				char *database_completed_date_time,
				char *arrived_date,
				char *database_arrived_date,
				char *shipped_date_time,
				char *database_shipped_date_time,
				double total_payment,
				double database_total_payment,
				double amount_due,
				double database_amount_due,
				char *transaction_date_time,
				char *database_transaction_date_time,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *application_name );

double customer_sale_get_invoice_amount(
				double *sum_inventory_extension,
				double *sum_fixed_service_extension,
				double *sum_hourly_service_extension,
				double *sum_extension,
				double *sales_tax,
				double shipping_revenue,
				LIST *inventory_sale_list,
				LIST *specific_inventory_sale_list,
				LIST *fixed_service_sale_list,
				LIST *hourly_service_sale_list,
				char *full_name,
				char *street_address,
				char *application_name );

void customer_inventory_sale_list_update(
				LIST *inventory_sale_list,
				char *application_name );

double customer_sale_get_tax_rate(
				char *application_name,
				char *full_name,
				char *street_address );

double customer_get_total_payment(
				LIST *payment_list );

LIST *customer_payment_get_list(char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );

CUSTOMER_PAYMENT *customer_payment_new(
				char *payment_date_time );

LIST *customer_sale_ledger_refresh(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double sum_inventory_extension,
				double sum_fixed_service_extension,
				double sum_hourly_service_extension,
				double sales_tax,
				double shipping_revenue,
				double invoice_amount );

LIST *customer_sale_ledger_cost_of_goods_sold_update(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double customer_sale_cost_of_goods_sold );

double customer_sale_fetch_cost_of_goods_sold(
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *application_name );

char *customer_get_completed_sale_date_time(
				char *application_name,
				char *completed_date_time );

char *customer_sale_fetch_transaction_date_time(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );

char *customer_sale_get_select(	char *application_name );

void customer_inventory_set_extension(
				LIST *inventory_sale_list );

LIST *customer_sale_get_historical_inventory_sale_list(
				LIST *historical_customer_sale_list,
				char *inventory_name );

CUSTOMER_SALE *customer_sale_seek(
				LIST *customer_sale_list,
				char *sale_date_time );

void customer_sale_inventory_delete(
				LIST *inventory_sale_list,
				char *inventory_name );

char *customer_sale_get_inventory_sale_join_where(
				void );

LIST *customer_get_inventory_sale_list(
				char *full_name,
				char *street_address,
				char *sale_date_time,
				HASH_TABLE *inventory_sale_hash_table,
				LIST *inventory_sale_name_list );

LIST *customer_get_inventory_customer_sale_list(
				char *application_name,
				char *inventory_name,
				HASH_TABLE *inventory_sale_hash_table,
				LIST *sale_inventory_name_list,
				HASH_TABLE *transaction_hash_table,
				HASH_TABLE *journal_ledger_hash_table,
				LIST *inventory_list );

void customer_sale_transaction_delete_with_propagate(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time );

boolean customer_sale_inventory_is_latest(
				char *application_name,
				char *inventory_name,
				char *sale_date_time );

void customer_sale_list_cost_of_goods_sold_transaction_update(
				char *application_name,
				LIST *customer_sale_list );

void customer_sale_list_inventory_transaction_update_and_propagate(
				char *application_name,
				LIST *customer_sale_list,
				char *propagate_transaction_date_time );

void customer_sale_list_cost_of_goods_sold_set(
				LIST *customer_sale_list,
				LIST *inventory_list );

LIST *customer_get_payment_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );

CUSTOMER_PAYMENT *customer_payment_seek(
				LIST *customer_payment_list,
				char *payment_date_time );

LIST *customer_payment_journal_ledger_refresh(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double payment_amount );

void customer_payment_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *transaction_date_time,
				char *database_transaction_date_time );

char *customer_payment_get_update_sys_string(
				char *application_name );

void customer_specific_inventory_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				char *serial_number,
				double extension,
				double database_extension );

SPECIFIC_INVENTORY_SALE *customer_specific_inventory_sale_seek(
				LIST *specific_inventory_sale_list,
				char *inventory_name,
				char *serial_number );

FIXED_SERVICE *customer_fixed_service_sale_seek(
				LIST *fixed_service_sale_list,
				char *service_name );

HOURLY_SERVICE *customer_hourly_service_sale_seek(
				LIST *hourly_service_sale_list,
				char *service_name,
				char *description );

double customer_sale_get_specific_inventory_cost_of_goods_sold(
				LIST *specific_inventory_sale_list );

double customer_sale_get_amount_due(	char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time );;


double customer_sale_get_total_payment(	char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time );

char *customer_sale_get_memo(		char *full_name );

void customer_sale_inventory_cost_account_list_set(
					LIST *inventory_account_list,
					LIST *cost_account_list,
					LIST *inventory_sale_list,
					LIST *inventory_list,
					boolean is_database );

LIST *customer_sale_ledger_cost_of_goods_sold_insert(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					LIST *inventory_account_list,
					LIST *cost_account_list );

void customer_fixed_service_work_update(
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time,
					char *service_name,
					char *begin_date_time,
					double work_hours,
					double database_work_hours );

void customer_hourly_service_work_update(
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time,
					char *service_name,
					char *description,
					char *begin_date_time,
					double work_hours,
					double database_work_hours );

void customer_fixed_service_sale_update(
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time,
					char *service_name,
					double extension,
					double database_extension,
					double work_hours,
					double database_work_hours );

void customer_hourly_service_sale_update(
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time,
					char *service_name,
					char *description,
					double extension,
					double database_extension,
					double work_hours,
					double database_work_hours );

LIST *customer_fixed_service_sale_get_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time );

LIST *customer_hourly_service_sale_get_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time );

LIST *customer_fixed_service_work_get_list(
					double *work_hours,
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time,
					char *service_name );

LIST *customer_hourly_service_work_get_list(
					double *work_hours,
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time,
					char *service_name,
					char *description );

SERVICE_WORK *customer_service_work_new(void );

SERVICE_WORK *customer_service_work_seek(
				LIST *service_work_list,
				char *begin_date_time );

#endif

