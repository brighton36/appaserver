/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/investment.h	*/
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

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *account_number;
	char *date;
	char *investment_operation;
	double balance;
	double balance_change;
	char *transaction_date_time;
	TRANSACTION *transaction;
} ACCOUNT_BALANCE;

/* Operations */
/* ---------- */
ACCOUNT_BALANCE *investment_account_balance_new(
					char *full_name,
					char *street_address,
					char *account_number,
					char *date );

ACCOUNT_BALANCE *investment_account_balance_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date );

char *investment_account_balance_fetch_prior_date(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date );

void investment_account_balance_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date,
					double balance_change,
					char *transaction_date_time );

/* Returns transaction_date_time */
/* ----------------------------- */
char *investment_time_passage_transaction_insert(
					char *application_name,
					char *full_name,
					char *street_address,
					char *date,
					double balance_change,
					char *investment_account,
					char *unrealized_gain,
					char *unrealized_loss );

#endif
