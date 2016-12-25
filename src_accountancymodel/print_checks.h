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
	double drop_down_check_amount;
	int check_number;
	LIST *entity_account_debit_list;
	LIST *purchase_order_list;
	double loss_amount;
	char *transaction_date_time;
} ENTITY_CHECK_AMOUNT;

typedef struct
{
	LIST *current_liability_account_list;
	LIST *entity_check_amount_list;
} PRINT_CHECKS;

/* Operations */
/* ---------- */
PRINT_CHECKS *print_checks_new(	char *application_name,
				LIST *full_name_list,
				LIST *street_address_list,
				int starting_check_number,
				double dialog_box_check_amount );

ENTITY_CHECK_AMOUNT *print_checks_entity_check_amount_new(
				char *full_name,
				char *street_address );

ENTITY_ACCOUNT_DEBIT *print_checks_entity_account_debit_new(
				char *account_name );

LIST *print_checks_get_current_liability_account_list(
				char *application_name );

LIST *print_checks_get_after_balance_zero_journal_ledger_list(
				char *application_name,
				char *account_name );

LIST *print_checks_get_entity_list(
				char *application_name,
				char *full_name,
				char *street_address );

ENTITY_CHECK_AMOUNT *print_checks_get_entity_check_amount(
				char *application_name,
				char *full_name,
				char *street_address );

void print_checks_set_entity_account_debit_list(
				LIST *entity_check_amount_list,
				char *application_name,
				LIST *current_liability_account_list,
				double dialog_box_check_amount );

ENTITY_ACCOUNT_DEBIT *
	print_checks_get_or_set_entity_account_debit(
				LIST *entity_account_debit_list,
				char *account_name );

LIST *print_checks_get_entity_account_debit_list(
				double *loss_amount,
				LIST *current_liability_account_list,
				char *full_name,
				char *street_address,
				double dialog_box_check_amount,
				double drop_down_check_amount );

void print_checks_set_single_check_entity_account_debit_list(
				LIST *entity_account_debit_list,
				double *remaining_check_amount,
				LIST *journal_ledger_list,
				char *full_name,
				char *street_address,
				char *account_name );

LIST *print_checks_get_entity_check_amount_list(
				char *application_name,
				LIST *full_name_list,
				LIST *street_address_list,
				int starting_check_number );

void print_checks_insert_transaction_journal_ledger(
				char *application_name,
				LIST *entity_check_amount_list,
				double dialog_box_check_amount );

LIST *print_checks_get_purchase_order_list(
				char *application_name,
				LIST *current_liability_account_list,
				char *full_name,
				char *street_address,
				double dialog_box_check_amount,
				double drop_down_check_amount );

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

void print_checks_subtract_purchase_order_amount_due(
				LIST *entity_check_amount_list,
				char *application_name );

ENTITY_ACCOUNT_DEBIT *print_checks_entity_account_debit_seek(
				LIST *entity_account_debit_list,
				char *account_payable_account );

void print_checks_subtract_amount_due(
				LIST *entity_account_debit_list,
				char *account_payable_account,
				double amount_due );

void print_checks_set_transaction_date_time(
				LIST *entity_check_amount_list );

#endif
