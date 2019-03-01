/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/bank_upload_journal_ledger_insert.c	*/
/* -------------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "application.h"
#include "application_constants.h"
#include "bank_upload.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void bank_upload_journal_ledger_insert(
			char *application_name,
			char *begin_date,
			char *fund_name,
			boolean execute );


/* Global variables */
/* ---------------- */
enum bank_upload_exception bank_upload_exception = {0};

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_date;
	char *fund_name;
	boolean execute;

	/* Exits if not found. */
	/* ------------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s begin_date fund execute_yn\n",
			 argv[ 0 ] );

		exit ( 1 );
	}

	begin_date = argv[ 1 ];
	fund_name = argv[ 2 ];
	execute = (*argv[ 3 ] == 'y');

	bank_upload_journal_ledger_insert(
			application_name,
			begin_date,
			fund_name,
			execute );

	exit( 0 );

} /* main() */

void bank_upload_journal_ledger_insert(
			char *application_name,
			char *begin_date,
			char *fund_name,
			boolean execute )
{
	BANK_UPLOAD_STRUCTURE *p;

	p = bank_upload_structure_calloc();

	p->existing_cash_journal_ledger_list =
		bank_upload_fetch_existing_cash_journal_ledger_list(
			application_name,
			begin_date /* minimum_bank_date */,
			fund_name );

	p->reoccurring_structure = reoccurring_structure_new();

	p->reoccurring_structure->
		reoccurring_transaction_list =
			reoccurring_fetch_reoccurring_transaction_list(
				application_name );

	p->table.bank_upload_table_list =
		bank_upload_fetch_bank_upload_table_list(
			application_name,
			0 /* starting_sequence_number */,
			begin_date );

	bank_upload_set_transaction(
		p->table.bank_upload_table_list,
		p->reoccurring_structure->reoccurring_transaction_list,
		p->existing_cash_journal_ledger_list,
		p->uncleared_checks_transaction_list );

	if ( execute )
	{
		bank_upload_insert_transaction(
			application_name,
			p->table.bank_upload_table_list );
	}
	else
	{
		bank_upload_transaction_text_display(
			p->table.bank_upload_table_list );
	}

} /* bank_upload_journal_ledger_insert() */

