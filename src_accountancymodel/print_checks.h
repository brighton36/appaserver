/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/print_checks.h			*/
/* -------------------------------------------------------------------- */
/* This is the appaserver print_checks ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PRINT_CHECKS_H
#define PRINT_CHECKS_H

/* Includes */
/* -------- */
#include "list.h"
#include "purchase.h"
#include "ledger.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PRINT_CHECKS_MEMO	"Automatic Check"

/* Structures */
/* ---------- */
typedef struct
{
	char *account_name;
	double debit_amount;
} ENTITY_ACCOUNT_DEBIT;

typedef struct
{
	char *full_name;
	char *street_address;
	double sum_credit_amount_check_amount;
	int check_number;
	LIST *entity_account_debit_list;
	LIST *purchase_order_list;
	double loss_amount;
	/* char *transaction_date_time; */
} ENTITY_CHECK_AMOUNT;

typedef struct
{
	LIST *current_liability_account_list;
	LIST *entity_check_amount_list;
	double dialog_box_check_amount;
} PRINT_CHECKS;

/* Operations */
/* ---------- */
PRINT_CHECKS *print_checks_new(	char *application_name,
				char *fund_name,
				LIST *full_name_list,
				LIST *street_address_list,
				int starting_check_number,
				double dialog_box_check_amount,
				char *sales_tax_payable_full_name,
				char *sales_tax_payable_street_address );

ENTITY_CHECK_AMOUNT *print_checks_entity_check_amount_new(
				char *full_name,
				char *street_address );

ENTITY_ACCOUNT_DEBIT *print_checks_entity_account_debit_new(
				char *account_name );

LIST *print_checks_get_current_liability_account_list(
				char *application_name );

LIST *print_checks_get_entity_list(
				char *application_name,
				char *full_name,
				char *street_address );

ENTITY_CHECK_AMOUNT *print_checks_get_entity_check_amount(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				LIST *current_liability_account_list,
				double dialog_box_check_amount,
				char *sales_tax_payable_full_name,
				char *sales_tax_payable_street_address );

ENTITY_ACCOUNT_DEBIT *
	print_checks_get_or_set_entity_account_debit(
				LIST *entity_account_debit_list,
				char *account_name );

LIST *print_checks_get_entity_account_debit_list(
				double *remaining_check_amount,
				LIST *current_liability_account_list,
				char *full_name,
				char *street_address );

void print_checks_set_entity_account_debit_list(
				LIST *entity_account_debit_list,
				double *remaining_check_amount,
				LIST *journal_ledger_list,
				char *full_name,
				char *street_address,
				char *account_name );

LIST *print_checks_get_entity_check_amount_list(
				char *application_name,
				char *fund_name,
				LIST *full_name_list,
				LIST *street_address_list,
				int starting_check_number,
				LIST *current_liability_account_list,
				double dialog_box_check_amount,
				char *sales_tax_payable_full_name,
				char *sales_tax_payable_street_address );

LIST *print_checks_fetch_purchase_order_list(
				double *remaining_check_amount,
				char *application_name,
				LIST *current_liability_account_list,
				char *full_name,
				char *street_address );

void print_checks_set_purchase_order_list(
				LIST *purchase_order_list,
				double *remaining_check_amount,
				char *application_name,
				LIST *journal_ledger_list,
				char *full_name,
				char *street_address );

double print_checks_get_entity_account_credit_balance(
				LIST *journal_ledger_list,
				char *full_name,
				char *street_address );

ENTITY_ACCOUNT_DEBIT *print_checks_entity_account_debit_seek(
				LIST *entity_account_debit_list,
				char *account_payable_account );

void print_checks_decrement_debit_amount(
				LIST *entity_account_debit_list,
				double purchase_order_amount_due );

boolean print_checks_insert_entity_check_amount_list(
				char *application_name,
				char *fund_name,
				LIST *entity_check_amount_list,
				double dialog_box_check_amount,
				char *memo );

char *print_checks_entity_check_amount_list_display(
				LIST *entity_check_amount_list );

char *print_checks_current_liability_account_list_display(
				LIST *current_liability_account_list );

char *print_checks_display(	PRINT_CHECKS *print_checks );

int print_checks_insert_vendor_payment(
				char **propagate_transaction_date_time,
				double *remaining_check_amount,
				LIST *purchase_order_list,
				char *application_name,
				char *uncleared_checks_account,
				char *account_payable_account,
				int seconds_to_add,
				int check_number,
				char *memo );

int print_checks_insert_entity_check_amount(
				LIST *distinct_account_name_list,
				char **propagate_transaction_date_time,
				char *application_name,
				ENTITY_CHECK_AMOUNT *entity_check_amount,
				double check_amount,
				char *uncleared_checks_account,
				char *account_payable_account,
				int seconds_to_add,
				char *fund_name,
				char *memo );

void print_checks_insert_entity_account_debit_list(
				LIST *distinct_account_name_list,
				char **propagate_transaction_date_time,
				double remaining_check_amount,
				char *application_name,
				LIST *entity_account_debit_list,
				int check_number,
				char *uncleared_checks_account,
				int seconds_to_add,
				char *full_name,
				char *street_address,
				double loss_amount,
				char *fund_name,
				char *memo );

char *print_checks_entity_account_debit_list_display(
				LIST *entity_account_debit_list );

void print_checks_set_sales_tax_payable(
				ENTITY_CHECK_AMOUNT *entity_check_amount,
				char *application_name,
				char *fund_name,
				double check_amount );

#endif
