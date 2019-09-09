/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_upload.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_UPLOAD_H
#define FEEDER_UPLOAD_H

#include "ledger.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */

/* Operations */
/* ---------- */

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_bank_description_original(
					char *bank_description_file,
					double bank_amount );

LIST *feeder_upload_get_possible_description_list(
					char *bank_description_file,
					char *fund_name,
					char *bank_date,
					double bank_amount,
					double bank_running_balance,
					int check_number );

char *feeder_upload_get_description_embedded(
					char *bank_description_file,
					char *fund_name,
					double bank_amount,
					double bank_running_balance );

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_bank_amount_portion(
					double bank_amount );

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_fund_portion(
					char *fund_name );

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_description_partially_embedded(
					char *bank_description_file,
					char *fund_name,
					double bank_amount );

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_check_portion(
					int check_number );

char *feeder_upload_get_like_where(	char *where,
					char *bank_date,
					char *bank_description );

/* Returns static memory */
/* --------------------- */
char *feeder_upload_trim_bank_date_description(
					char *bank_description_file );

#endif

