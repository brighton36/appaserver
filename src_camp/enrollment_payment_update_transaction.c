/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_camp/enrollment_payment_update_transaction.c	*/
/* -------------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "ledger.h"
#include "camp.h"

/* Constants */
/* --------- */
#define ENROLLMENT_PAYMENT_MEMO		"Enrollment Payment"

/* Prototypes */
/* ---------- */
void enrollment_payment_update_transaction(
				char *application_name,
				char *fund_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				char *payment_date_time );

void enrollment_payment_update_transaction_predelete(
				char *application_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				char *payment_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *camp_begin_date;
	char *camp_title;
	char *full_name;
	char *street_address;
	char *payment_date_time;
	char *state;

	/* exit() if it fails. */
	/* ------------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s fund camp_begin_date camp_title full_name street_address payment_date_time state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 1 ];
	camp_begin_date = argv[ 2 ];
	camp_title = argv[ 3 ];
	full_name = argv[ 4 ];
	street_address = argv[ 5 ];
	payment_date_time = argv[ 6 ];
	state = argv[ 7 ];

	/* ------------------------------------------------------------ */
	/* ENROLLMENT_PAYMENT.transaction_date_time DOES exist, 	*/
	/* so execute predelete.					*/
	/* ------------------------------------------------------------ */
	if ( strcmp( state, "predelete" ) == 0 )
	{
		enrollment_payment_update_transaction_predelete(
			application_name,
			camp_begin_date,
			camp_title,
			full_name,
			street_address,
			payment_date_time );
	}
	else
	if ( strcmp( state, "delete" ) == 0
	||   strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0 )
	{
		enrollment_payment_update_transaction(
			application_name,
			fund_name,
			camp_begin_date,
			camp_title,
			full_name,
			street_address,
			payment_date_time );
	}

	return 0;

} /* main() */

void enrollment_payment_update_transaction(
			char *application_name,
			char *fund_name,
			char *camp_begin_date,
			char *camp_title,
			char *full_name,
			char *street_address,
			char *payment_date_time )
{
	CAMP *camp;
	char *existing_transaction_date_time;
	ENROLLMENT_PAYMENT *payment;

	if ( ! ( camp =
			camp_fetch(
				application_name,
				camp_begin_date,
				camp_title ) ) )
	{
		return;
	}

	if ( ! ( camp->camp_enrollment =
			camp_enrollment_fetch(
				application_name,
				camp->camp_begin_date,
				camp->camp_title,
				full_name,
				street_address,
				camp->enrollment_cost ) ) )
	{
		return;
	}

	/* -------------------------------------------- */
	/* This fails when deleting a parent.		*/
	/* Still, need to update CAMP_ENROLLMENT.	*/
	/* -------------------------------------------- */
	payment = camp_enrollment_payment_seek(
			camp->camp_enrollment->camp_enrollment_payment_list,
			payment_date_time );

	if ( payment )
	{
		if ( payment->camp_enrollment_payment_transaction )
		{
			existing_transaction_date_time =
				payment->
					camp_enrollment_payment_transaction->
					transaction_date_time;
		}
		else
		{
			existing_transaction_date_time = (char *)0;
		}

		payment->camp_enrollment_payment_transaction =
			camp_enrollment_payment_transaction(
				application_name,
				fund_name,
				camp->camp_enrollment->full_name,
				camp->camp_enrollment->street_address,
				existing_transaction_date_time,
				payment->payment_amount );

		payment->
			camp_enrollment_payment_transaction->
			transaction_date_time =
			/* -------------------------------------- */
			/* Returns inserted transaction_date_time */
			/* -------------------------------------- */
			ledger_transaction_refresh(
				application_name,
				payment->
					camp_enrollment_payment_transaction->
					full_name,
				payment->
					camp_enrollment_payment_transaction->
					street_address,
				payment->
					camp_enrollment_payment_transaction->
					transaction_date_time,
				payment->
					camp_enrollment_payment_transaction->
					transaction_amount,
				ENROLLMENT_PAYMENT_MEMO,
				0 /* check_number */,
				1 /* lock_transaction */,
				payment->
					camp_enrollment_payment_transaction->
					journal_ledger_list );

		camp_enrollment_payment_update(
			camp->camp_begin_date,
			camp->camp_title,
			camp->camp_enrollment->full_name,
			camp->camp_enrollment->street_address,
			payment_date_time,
			payment->
				camp_enrollment_payment_transaction->
				transaction_date_time );
	}

	camp_enrollment_update(
		camp->camp_begin_date,
		camp->camp_title,
		camp->camp_enrollment->full_name,
		camp->camp_enrollment->street_address,
		camp->camp_enrollment->camp_enrollment_invoice_amount,
		camp->camp_enrollment->camp_enrollment_total_payment_amount,
		camp->camp_enrollment->camp_enrollment_amount_due,
		camp->
			camp_enrollment->
			camp_enrollment_transaction->
			transaction_date_time );

} /* enrollment_payment_update_transaction() */

void enrollment_payment_update_transaction_predelete(
			char *application_name,
			char *camp_begin_date,
			char *camp_title,
			char *full_name,
			char *street_address,
			char *payment_date_time )
{
	CAMP *camp;
	ENROLLMENT_PAYMENT *e;

	if ( ! ( camp =
			camp_fetch(
				application_name,
				camp_begin_date,
				camp_title ) ) )
	{
		return;
	}

	if ( ! ( camp->camp_enrollment =
			camp_enrollment_fetch(
				application_name,
				camp->camp_begin_date,
				camp->camp_title,
				full_name,
				street_address,
				camp->enrollment_cost ) ) )
	{
		return;
	}

	if ( ! ( e = camp_enrollment_payment_seek(
			camp->camp_enrollment->camp_enrollment_payment_list,
			payment_date_time ) ) )
	{
		return;
	}

	if ( !e->camp_enrollment_payment_transaction )
	{
		return;
	}

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			camp->camp_enrollment->full_name,
			camp->camp_enrollment->street_address,
			e->
				camp_enrollment_payment_transaction->
				transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			camp->camp_enrollment->full_name,
			camp->camp_enrollment->street_address,
			e->
				camp_enrollment_payment_transaction->
				transaction_date_time );

	ledger_journal_ledger_list_propagate( 
		e->
			camp_enrollment_payment_transaction->
			journal_ledger_list,
		application_name );

} /* enrollment_payment_update_transaction_predelete() */

