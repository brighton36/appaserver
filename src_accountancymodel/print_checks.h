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
	double check_amount;
	int check_number;
	LIST *entity_account_debit_list;
	LIST *purchase_order_list;
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
				int starting_check_number );

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
				LIST *current_liability_account_list );

ENTITY_ACCOUNT_DEBIT *
	print_checks_get_or_set_entity_account_debit(
				LIST *entity_account_debit_list,
				char *account_name );

LIST *print_checks_get_entity_account_debit_list(
				double check_amount,
				LIST *current_liability_account_list,
				char *full_name,
				char *street_address );

void print_checks_set_journal_ledger_entity_account_debit_list(
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
				LIST *print_checks_entity_check_amount_list );

LIST *print_checks_get_purchase_order_list(
				char *application_name,
				double check_amount,
				LIST *current_liability_account_list,
				char *full_name,
				char *street_address );

void print_checks_set_journal_ledger_purchase_order_list(
				LIST *purchase_order_list,
				double *remaining_check_amount,
				char *application_name,
				LIST *journal_ledger_list,
				char *full_name,
				char *street_address,
				char *account_name );

#endif
