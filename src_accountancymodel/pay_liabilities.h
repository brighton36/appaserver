/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/pay_liabilities.h		*/
/* -------------------------------------------------------------------- */
/* This is the appaserver pay_liabilities ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PAY_LIABILITIES_H
#define PAY_LIABILITIES_H

/* Includes */
/* -------- */
#include "list.h"
#include "purchase.h"
#include "ledger.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PAY_LIABILITIES_MEMO	"Automatic Payment"

/* Structures */
/* ---------- */
typedef struct
{
	LIST *transaction_list;
	LIST *vendor_payment_list;
} PAY_LIABILITIES_OUTPUT;

typedef struct
{
	char *full_name;
	char *street_address;
	double sum_balance;
	double payment_amount;
	double loss_amount;
	int check_number;
	LIST *current_liability_account_list;
	LIST *purchase_order_list;
} ENTITY_PAYABLE;

typedef struct
{
	LIST *current_liability_entity_list;
	LIST *liability_account_entity_list;
	LIST *entity_payable_list;
} PAY_LIABILITIES_PROCESS;

typedef struct
{
	LIST *liability_account_entity_list;
	double dialog_box_payment_amount;
	char *credit_account_name;
	char *loss_account_name;
	LIST *entity_payable_list;
	LIST *current_liability_account_list;
	LIST *purchase_order_list;
	int starting_check_number;
} PAY_LIABILITIES_INPUT;

typedef struct
{
	PAY_LIABILITIES_INPUT input;
	PAY_LIABILITIES_PROCESS process;
	PAY_LIABILITIES_OUTPUT output;
} PAY_LIABILITIES;

/* Operations */
/* ---------- */
PAY_LIABILITIES *pay_liabilities_calloc(
				void );

PAY_LIABILITIES *pay_liabilities_new(
				char *application_name,
				char *fund_name,
				LIST *full_name_list,
				LIST *street_address_list,
				int starting_check_number,
				double dialog_box_payment_amount,
				char *memo );

LIST *pay_liabilities_input_get_entity_payable_list(
				char *application_name,
				char *fund_name,
				LIST *full_name_list,
				LIST *street_address_list );

ENTITY_PAYABLE *pay_liabilities_entity_payable_calloc(
				void );

ENTITY_PAYABLE *pay_liabilities_entity_payable_new(
				char *full_name,
				char *street_address );

LIST *pay_liabilities_fetch_purchase_order_list(
				char *application_name );

double pay_liabilities_fetch_sum_balance(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address );

ENTITY_PAYABLE *pay_liabilities_entity_payable_seek(
				LIST *entity_payable_list,
				char *full_name,
				char *street_address );

LIST *pay_liabilities_get_liability_account_entity_list(
				LIST *input_liability_account_entity_list,
				LIST *input_entity_payable_list,
				double dialog_box_payment_amount,
				int starting_check_number );

LIST *pay_liabilities_fetch_liability_account_entity_list(
				char *application_name );

LIST *pay_liabilities_fetch_current_liability_account_list(
				char *application_name,
				char *fund_name );

LIST *pay_liabilities_distribute_liability_account_list(
				LIST *liability_account_list,
				double entity_payment_amount );

LIST *pay_liabilities_process_get_entity_payable_list(
				LIST *input_entity_payable_list,
				LIST *current_liability_entity_list,
				LIST *input_purchase_order_list,
				double dialog_box_payment_amount,
				int starting_check_number );

LIST *pay_liabilities_distribute_purchase_order_list(
				LIST *input_purchase_order_list,
				double payment_amount,
				char *full_name,
				char *street_address );

LIST *pay_liabilities_get_current_liability_entity_list(
				LIST *current_liability_account_list );

LIST *pay_liabilities_output_get_liability_account_transaction_list(
				LIST *liability_account_entity_list,
				char *credit_account_name,
				char *loss_account_name,
				char *memo );

LIST *pay_liabilities_output_get_entity_payable_transaction_list(
				LIST *entity_payable_list,
				char *credit_account_name,
				char *loss_account_name,
				char *memo,
				int seconds_to_add );

LIST *pay_liabilities_output_get_vendor_payment_list(
				LIST *entity_payable_list,
				LIST *transaction_list );

#endif
