/* -------------------------------------------------------------------- */
/* src_accountancymodel/accrual.h					*/
/* -------------------------------------------------------------------- */
/* This is the appaserver accrual ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ACCRUAL_H
#define ACCRUAL_H

#include "ledger.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define ACCRUAL_MEMO		"Accrual"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *asset_name;
	char *accrual_date;
	double accrual_amount;
	double database_accrual_amount;
	char *transaction_date_time;
	char *database_transaction_date_time;
	TRANSACTION *transaction;
} ACCRUAL;

/* Operations */
/* ---------- */
ACCRUAL *accrual_calloc( void );

ACCRUAL *accrual_fetch(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date );

void accrual_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date,
			double accrual_amount,
			double database_accrual_amount,
			char *transaction_date_time,
			char *database_transaction_date_time );

ACCRUAL *accrual_parse(
			char *application_name,
			char *input_buffer );

char *accrual_get_select(
			void );

char *accrual_get_update_sys_string(
			char *application_name );

double accrual_get_amount(
			double extension,
			double accrual_period_years,
			char *prior_accrual_date_string,
			char *accrual_date_string,
			double accumulated_accrual );

LIST *accrual_journal_ledger_refresh(
			char *application_name,
			char *fund_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double accrual_amount );

double accrual_get_fraction_of_year(
			char *prior_accrual_date_string,
			char *accrual_date_string );

LIST *accrual_fetch_list(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name );

/* Returns new accumulated_accrual */
/* ------------------------------------ */
double accrual_list_set(
			LIST *accrual_list,
			char *purchase_date,
			double extension,
			double accrual_period_years );

void accrual_list_update(
			LIST *accrual_list,
			char *application_name,
			char *fund_name );

void accrual_list_delete(
			LIST *accrual_list,
			char *application_name,
			char *fund_name );

void accrual_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date );

ACCRUAL *accrual_list_seek(
			LIST *accrual_list,
			char *accrual_date );

#endif

