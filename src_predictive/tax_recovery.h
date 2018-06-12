/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax_recovery.h			*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TAX_RECOVERY_H
#define TAX_RECOVERY_H

#include "list.h"
#include "boolean.h"
#include "html_table.h"
#include "date.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *asset_name;
	char *serial_number;
	char *tax_service_placement_date;
	double tax_cost_basis;
	char *tax_recovery_period;
	double tax_recovery_period_years;
	char *disposal_date;
	int recovery_year;
	double recovery_amount;
	double recovery_percent;
} TAX_RECOVERY;

/* Operations */
/* ---------- */
TAX_RECOVERY *tax_recovery_new(	void );

TAX_RECOVERY *tax_recovery_input_buffer_parse(
				char *input_buffer );

double tax_recovery_calculate_recovery_amount(
				double *recovery_percent,
				double cost_basis,
				char *service_placement_date_string,
				char *sale_date_string,
				double recovery_period_years,
				int current_year );

boolean tax_recovery_parse(
				int *service_month,
				int *service_year,
				int *sale_month,
				int *sale_year,
				char *service_placement_date_string,
				char *sale_date_string );

int tax_recovery_fetch_max_recovery_year(
				char *application_name,
				char *folder_name );

FILE *tax_recovery_get_input_pipe(
				char *application_name,
				int recovery_year,
				char *folder_name );

char *tax_recovery_get_select(	void );

char *tax_recovery_get_order_by(void );

char *tax_recovery_get_filter_where(
				char *minimum_disposal_date );

#endif
