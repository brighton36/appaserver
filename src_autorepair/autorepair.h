/* src_autorepair/autorepair.h				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef AUTOREPAIR_H
#define AUTOREPAIR_H

/* Constants */
/* --------- */
#define COMPANY_NAME_KEY		"company_name"
#define COMPANY_ADDRESS_KEY		"company_address"
#define COMPANY_SUITE_KEY		"company_suite"
#define COMPANY_CITY_KEY		"company_city"
#define COMPANY_STATE_KEY		"company_state"
#define COMPANY_ZIP_CODE_KEY		"company_zip_code"
#define COMPANY_PHONE_NUMBER_KEY	"company_phone_number"
#define COMPANY_EMAIL_ADDRESS_KEY	"company_email_address"
#define LOGO_FILENAME_KEY		"logo_filename"

#define LEDGER_CASH_ACCOUNT_KEY			"ledger_cash_account"
#define LEDGER_SALES_REVENUE_ACCOUNT_KEY	"ledger_sales_revenue_account"
#define LEDGER_SERVICE_REVENUE_ACCOUNT_KEY	"ledger_service_revenue_account"
#define LEDGER_SALES_TAX_PAYABLE_ACCOUNT_KEY	\
					"ledger_sales_tax_payable_account"
#define LEDGER_COST_OF_GOODS_SOLD_ACCOUNT_KEY	\
					"ledger_cost_of_goods_sold_account"
#define LEDGER_INVENTORY_ACCOUNT_KEY		"ledger_inventory_account"
#define LEDGER_RECEIVABLE_ACCOUNT_KEY		"ledger_receivable_account"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *completed_date_time;
	char *vehicle_make;
	char *vehicle_model;
	char *vehicle_trim;
	int vehicle_year;
	int odometer_miles;
	char *mechanic_full_name;
	char *symptom;
} AUTOREPAIR_CUSTOMER_SALE;

/* Prototypes */
/* ---------- */
AUTOREPAIR_CUSTOMER_SALE *autorepair_customer_sale_new(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *completed_date_time,
				char *vehicle_make,
				char *vehicle_model,
				char *vehicle_trim,
				int vehicle_year );

double autorepair_get_total_payment(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );

double autorepair_get_sales_tax(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );

boolean autorepair_customer_sale_load(
				int *odometer_miles,
				char **symptom,
				char **mechanic_full_name,
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );
#endif
