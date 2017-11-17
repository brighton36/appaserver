/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/tax.h				*/
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
	char *tax_form;
	char *tax_form_line;
	char *tax_form_description;
	boolean itemize_accounts;
	/* ------------------- */
	/* list of (ACCOUNT *) */
	/* ------------------- */
	LIST *account_list;
	double tax_form_line_total;
} TAX_FORM_LINE;

typedef struct
{
	char *tax_form;
	LIST *tax_form_line_list;
} TAX_FORM;

/* Operations */
/* ---------- */
TAX_FORM *tax_form_new(			char *tax_form );

TAX_FORM_LINE *tax_form_line_new(	char *tax_form,
					char *tax_form_line,
					char *tax_form_description,
					boolean itemize_accounts );

LIST *tax_form_fetch_line_list(		char *application_name,
					char *tax_form,
					char *as_of_date );

#endif
