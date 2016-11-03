/* -------------------------------------------------------------------- */
/* src_capitolpops/dues_payment.h						*/
/* -------------------------------------------------------------------- */
/* This is the appaserver dues_payment ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DUES_PAYMENT_H
#define DUES_PAYMENT_H

#include "ledger.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define DUES_PAYMENT_MEMO		"Dues Payment"
#define DUES_PAYMENT_KEY		"dues_key"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *payment_date;
	double payment_amount;
	double database_payment_amount;
	int check_number;
	char *transaction_date_time;
	char *database_transaction_date_time;
	TRANSACTION *transaction;
} DUES_PAYMENT;

/* Operations */
/* ---------- */
DUES_PAYMENT *dues_payment_new(
			void );

DUES_PAYMENT *dues_payment_fetch(
			char *application_name,
			char *full_name,
			char *street_address,
			char *payment_date );

void dues_payment_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *payment_date,
			double payment_amount,
			double database_payment_amount,
			char *transaction_date_time,
			char *database_transaction_date_time );

DUES_PAYMENT *dues_payment_parse(
			char *input_buffer );

char *dues_payment_get_select(
			void );

char *dues_payment_get_update_sys_string(
			char *application_name );

void dues_payment_journal_ledger_refresh(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double payment_amount );

void dues_payment_journal_ledger_propagate(
			char *application_name,
			char *transaction_date_time );

char *dues_payment_get_where(
			char *full_name,
			char *street_address,
			char *payment_date );

#endif

