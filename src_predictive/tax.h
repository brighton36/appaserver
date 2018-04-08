/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver tax ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TAX_H
#define TAX_H

#include "list.h"
#include "boolean.h"
#include "html_table.h"
#include "date.h"
#include "latex.h"
#include "hash_table.h"
#include "dictionary.h"
#include "ledger.h"

/* Constants */
/* --------- */
#define TAX_FORM_LINE_KEY		".tax_form_line "
#define TAX_FORM_LINE_ACCOUNT_KEY	".account "

/* Structures */
/* ---------- */
typedef struct
{
	char *account_name;
	LIST *journal_ledger_list;
	boolean accumulate_debit;
} TAX_FORM_LINE_ACCOUNT;

typedef struct
{
	char *tax_form;
	char *tax_form_line;
	char *tax_form_description;
	boolean itemize_accounts;
	LIST *account_list;
	double tax_form_line_total;
	LIST *tax_form_line_account_list;
} TAX_FORM_LINE;

typedef struct
{
	char *tax_form;
	LIST *tax_form_line_list;
} TAX_FORM;

typedef struct
{
	char *tax_form;
	LIST *tax_form_line_list;
} TAX_PROCESS;

typedef struct
{
	TAX_FORM *tax_form;
	LIST *cash_transaction_list;
} TAX_INPUT;

typedef struct
{
	TAX_INPUT tax_input;
	TAX_PROCESS tax_process;
	/* TAX_OUTPUT tax_output; */
	char *begin_date_string;
	char *end_date_string;
} TAX;

/* Operations */
/* ---------- */
TAX *tax_new(				char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *tax_form );

TAX_FORM *tax_form_new(			char *application_name,
					char *tax_form );

TAX_FORM_LINE *tax_form_line_new(	char *tax_form,
					char *tax_form_line,
					char *tax_form_description,
					boolean itemize_accounts );

LIST *tax_form_fetch_line_list(		char *application_name,
					char *tax_form );

LIST *tax_fetch_account_list(		char *application_name,
					char *tax_form_line );

#endif
