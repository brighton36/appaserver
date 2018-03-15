/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/investment.h		*/
/* ---------------------------------------------------- */
/* This is the appaserver investment ADT.		*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef INVESTMENT_H
#define INVESTMENT_H

#include "folder.h"
#include "list.h"
#include "ledger.h"

/* Constants */
/* --------- */
#define ACCOUNT_BALANCE_FOLDER_NAME		"equity_account_balance"
#define INVESTMENT_ACCOUNT_FOLDER_NAME		"investment_account"

/* Must match INVESTMENT_OPERATION */
/* ------------------------------- */
#define INVESTMENT_OPERATION_PRIOR_PURCHASE	"prior_purchase"
#define INVESTMENT_OPERATION_PURCHASE		"purchase"
#define INVESTMENT_OPERATION_TIME_PASSAGE	"time_passage"
#define INVESTMENT_OPERATION_SALE		"sale"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *account_number;
	char *date_time;
	char *investment_operation;
	double share_price;
	double share_quantity_change;
	double share_quantity_balance;
	double cash_in;
	double market_value;
	double book_value_change;
	double book_value_balance;
	double moving_share_price;
	double unrealized_gain_balance;
	double unrealized_gain_change;
	double realized_gain;
	char *transaction_date_time;
	TRANSACTION *transaction;
	boolean is_latest;
	char *investment_account;
	char *fair_value_adjustment_account;
} ACCOUNT_BALANCE;

/* Operations */
/* ---------- */
ACCOUNT_BALANCE *investment_account_balance_new(
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time );

ACCOUNT_BALANCE *investment_account_balance_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time );

char *investment_account_balance_fetch_prior_date_time(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time );

void investment_account_balance_update(	ACCOUNT_BALANCE *new_account_balance,
					ACCOUNT_BALANCE *account_balance );

ACCOUNT_BALANCE *investment_account_balance_calculate(
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time,
					double share_price,
					double share_quantity_change,
					double prior_share_quantity_balance,
					double prior_book_value_balance,
					double prior_moving_share_price,
					double prior_unrealized_gain_balance,
					char *investment_operation,
					boolean first_row );

ACCOUNT_BALANCE *investment_account_balance_deposit(
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time,
					double share_price,
					double share_quantity_change,
					double prior_share_quantity_balance,
					double prior_book_value_balance,
					double prior_unrealized_gain_balance,
					boolean first_row );

ACCOUNT_BALANCE *investment_account_balance_time_passage(
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time,
					double share_price,
					double share_quantity_change,
					double prior_share_quantity_balance,
					double prior_moving_share_price,
					double prior_unrealized_gain_balance );

ACCOUNT_BALANCE *investment_account_balance_withdrawal(
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time,
					double share_price,
					double share_quantity_change,
					double prior_share_quantity_balance,
					double prior_book_value_balance,
					double prior_moving_share_price,
					double prior_unrealized_gain_balance );

ACCOUNT_BALANCE *investment_account_balance_parse(
					char *full_name,
					char *street_address,
					char *account_name,
					char *input_buffer );

LIST *investment_get_fair_value_adjustment_ledger_list(
					double transaction_amount,
					char *debit_account,
					char *credit_account );

boolean investment_account_balance_is_latest(
					char *application_name,
					char *date_time );

FILE *investment_open_update_pipe(	void );

TRANSACTION *investment_build_transaction(
					char *application_name,
					char *fund_name,
					ACCOUNT_BALANCE *account_balance );

TRANSACTION *investment_build_prior_purchase_transaction(
					char *application_name,
					char *fund_name,
					ACCOUNT_BALANCE *account_balance );

TRANSACTION *investment_build_purchase_transaction(
					char *application_name,
					char *fund_name,
					ACCOUNT_BALANCE *account_balance );

TRANSACTION *investment_build_sale_transaction(
					char *application_name,
					char *fund_name,
					ACCOUNT_BALANCE *account_balance );

TRANSACTION *investment_build_time_passage_transaction(
					char *application_name,
					char *fund_name,
					ACCOUNT_BALANCE *account_balance );

char *investment_get_memo(		char *investment_operation );

LIST *investment_fetch_account_balance_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *begin_date_time );

char *investment_account_balance_get_join(
					void );

char *investment_account_balance_get_select(
					void );

char *investment_get_update_sys_string(
					char *application_name );

void investment_transaction_date_time_update(
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time,
					char *transaction_date_time,
					char *application_name );

#endif
