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
#include "ledger.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *account_name;
	double debit_amount;
} PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT;

typedef struct
{
	char *full_name;
	char *street_address;
	double check_amount;
} PRINT_CHECKS_ENTITY_CHECK_AMOUNT;

typedef struct
{
	LIST *current_liability_account_list;
	LIST *entity_check_amount_list;
	LIST *entity_account_debit_list;
} PRINT_CHECKS;

/* Operations */
/* ---------- */
PRINT_CHECKS *print_checks_new(	void );

PRINT_CHECKS_ENTITY_CHECK_AMOUNT *print_checks_entity_check_amount_new(
				char *full_name,
				char *street_address );

PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *print_checks_entity_account_debit_new(
				char *full_name,
				char *street_address,
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

PRINT_CHECKS_ENTITY_CHECK_AMOUNT *print_checks_get_entity_check_amount(
				char *application_name,
				char *full_name,
				char *street_address );

LIST *print_checks_get_entity_account_debit_list(
				LIST *current_liability_account_list,
				LIST *entity_check_amount_list );

PRINT_CHECKS_ENTITY_ACCOUNT_DEBIT *
	print_checks_get_or_set_entity_account_debit(
				LIST *entity_account_debit_list,
				char *full_name,
				char *street_address,
				char *account_name );

#endif
