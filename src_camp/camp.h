/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_camp/camp.h					*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks CAMP ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef CAMP_H
#define CAMP_H

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
	TRANSACTION *camp_enrollment_transaction;
	char *transaction_date_time;
} ENROLLMENT;

typedef struct
{
	char *camp_begin_date;
	char *camp_title;
	double enrollment_cost;
	ENROLLMENT *enrollment;
} CAMP;

/* Operations */
/* ---------- */
TRANSACTION *camp_enrollment_transaction(
				char *application_name,
				char *fund_name,
				double enrollment_cost );

ENROLLMENT *camp_enrollment_fetch(
				char *application_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address );

ENROLLMENT *camp_enrollment_parse(
				char *input_buffer );

char *camp_enrollment_select(
				void );

LIST *camp_enrollment_journal_ledger_list(
				double *transaction_amount,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double enrollment_cost );

#endif

