/* -------------------------------------------------------------------- */
/* src_capitolpops/donation.h						*/
/* -------------------------------------------------------------------- */
/* This is the appaserver donation ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DONATION_H
#define DONATION_H

#include "ledger.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define DONATION_MEMO		"Donation"

/* Structures */
/* ---------- */
typedef struct
{
	char *fund_name;
	double total_fund_donation_amount;
	LIST *donation_program_list;
} DONATION_FUND;

typedef struct
{
	char *full_name;
	char *street_address;
	char *donation_date;
	char *account_name;
	double donation_amount;
} DONATION_PROGRAM;

typedef struct
{
	char *full_name;
	char *street_address;
	char *donation_date;
	int check_number;
	double total_donation_amount;
	double database_total_donation_amount;
	char *transaction_date_time;
	char *database_transaction_date_time;
	TRANSACTION *transaction;
	LIST *donation_program_list;
	LIST *donation_fund_list;
} DONATION;

/* Operations */
/* ---------- */
DONATION_FUND *donation_fund_new(
			char *fund_name );

DONATION_PROGRAM *donation_program_new(
			void );
DONATION *donation_new(
			void );

LIST *donation_get_fund_list(
			char *application_name,
			LIST *donation_program_list );

DONATION *donation_fetch(
			char *application_name,
			char *full_name,
			char *street_address,
			char *donation_date );

void donation_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *donation_date,
			double total_donation_amount,
			double database_total_donation_amount,
			char *transaction_date_time,
			char *database_transaction_date_time );

DONATION *donation_parse(
			char *input_buffer );

DONATION_PROGRAM *donation_program_parse(
			char *input_buffer );

char *donation_get_select(
			void );

char *donation_program_get_select(
			void );

char *donation_get_update_sys_string(
			char *application_name );

double donation_get_total_donation_amount(
			LIST *donation_program_list );

void donation_journal_ledger_refresh_and_propagate(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *donation_fund_list,
			boolean propagate_only );

/*
void donation_journal_ledger_propagate(
			char *application_name,
			char *transaction_date_time,
			LIST *donation_fund_list );
*/

LIST *donation_fetch_donation_program_list(
			char *application_name,
			char *full_name,
			char *street_address,
			char *donation_date );

char *donation_get_where(
			char *full_name,
			char *street_address,
			char *donation_date );

DONATION_PROGRAM *donation_seek_donation_program(
			LIST *donation_program_list,
			char *account_name );

DONATION_FUND *donation_get_or_set_fund(
			LIST *donation_fund_list,
			char *fund_name );

void donation_fund_list_set_total_donation_amount(
			LIST *donation_fund_list );

#endif

