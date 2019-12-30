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
	char *camp_begin_date;
	char *camp_title;
	char *full_name;
	char *street_address;
	char *payment_date_time;
	double payment_amount;
	TRANSACTION *camp_enrollment_payment_transaction;
} ENROLLMENT_PAYMENT;

typedef struct
{
	char *service_name;
	double service_price;
} SERVICE_ENROLLMENT;

typedef struct
{
	char *full_name;
	char *street_address;
	TRANSACTION *camp_enrollment_transaction;
	LIST *camp_enrollment_service_enrollment_list;
	LIST *camp_enrollment_payment_list;
	double camp_enrollment_invoice_amount;
	double camp_enrollment_total_payment_amount;
	double camp_enrollment_amount_due;
} CAMP_ENROLLMENT;

typedef struct
{
	char *camp_begin_date;
	char *camp_title;
	double enrollment_cost;
	CAMP_ENROLLMENT *camp_enrollment;
} CAMP;

/* Operations */
/* ---------- */
CAMP *camp_new(			char *camp_begin_date,
				char *camp_title );

CAMP_ENROLLMENT *camp_enrollment_new(
				char *full_name,
				char *street_address );

SERVICE_ENROLLMENT *camp_enrollment_service_enrollment_new(
				char *service_name,
				double service_price );

ENROLLMENT_PAYMENT *camp_enrollment_payment_new(
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				char *payment_date_time );

TRANSACTION *camp_enrollment_transaction(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *existing_transaction_date_time,
				double invoice_amount );

CAMP *camp_fetch(		char *application_name,
				char *camp_begin_date,
				char *camp_title );

CAMP_ENROLLMENT *camp_enrollment_fetch(
				char *application_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				double enrollment_cost );

LIST *camp_enrollment_service_enrollment_list(
				char *application_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address );

LIST *camp_enrollment_payment_list(
				char *application_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address );

void camp_enrollment_payment_update(
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				char *payment_date_time,
				char *transaction_date_time );

void camp_enrollment_update(
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				double invoice_amount,
				double total_payment,
				double amount_due,
				char *transaction_date_time );

double camp_enrollment_invoice_amount(
				double enrollment_cost,
				LIST *service_enroll_list );

double camp_enrollment_total_payment_amount(
				LIST *camp_enrollment_payment_list );

double camp_enrollment_amount_due(
				double camp_enrollment_total_payment_amount,
				double invoice_amount );

TRANSACTION *camp_enrollment_payment_transaction(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				double payment_amount );

ENROLLMENT_PAYMENT *camp_enrollment_payment_seek(
				LIST *camp_enrollment_payment_list,
				char *payment_date_time );

#endif

